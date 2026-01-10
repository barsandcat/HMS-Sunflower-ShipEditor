// Copyright (c) 2025, sillygilly. All rights reserved.

#include "Shipyard/ShipCellInstance.h"

#include "Shipyard/ShipPartInstance.h"
#include "Shipyard/ShipPlanRender.h"

FShipStructure::FShipStructure(const FShipPartTransform& render_transform, const TArray<TObjectPtr<UShipPartInstance>>& part_instances)
{
	Parts.SetNum(part_instances.Num());
	for (int32 i = 0; i < Parts.Num(); i++)
	{
		UShipPartInstance* part_instance = part_instances[i];
		TSharedPtr<FShipStructurePart> part = MakeShared<FShipStructurePart>();
		Parts[i] = part;
		part->LoadBearing = part_instance->PartAsset->LoadBearing;
		for (FShipCellData& cell : part_instance->PartAsset->Cells)
		{
			Cells.Add(render_transform(part_instance->Transform(cell.Position)), MakeShared<FShipStructureCell>(cell.CellType, part));
		}
	}
}

bool FShipStructure::MergeStructures(const FShipStructure& structure_a, const FShipStructure& structure_b, FShipStructure& out_merged_structure)
{
	bool has_root = false;
	for (const auto& i : structure_a.Cells)
	{
		if (i.Value->CellType == ECellType::ROOT)
		{
			has_root = true;
			break;
		}
	}

	out_merged_structure = structure_a;
	out_merged_structure.Cells.Reserve(structure_a.Cells.Num() + structure_b.Cells.Num());
	out_merged_structure.Parts.Reserve(structure_a.Parts.Num() + structure_b.Parts.Num());

	for (const auto& i : structure_b.Parts)
	{
		out_merged_structure.Parts.Add(i);
	}

	for (const auto& i : structure_b.Cells)
	{
		if (out_merged_structure.Cells.Contains(i.Key))
		{
			return false;
		}
		else
		{
			if (i.Value->CellType == ECellType::ROOT)
			{
				if (has_root)
				{
					return false;
				}
				has_root = true;
			}
			out_merged_structure.Cells.Add(i.Key, i.Value);
		}
	}
	return true;
}

void FShipStructure::Process()
{
	// Example structure:
	// Y
	// 3
	// 2  C | C | C
	// 1  -   - * -
	// 0  C | C | C
	//-1
	//-2
	//   -2-1 0 1 2 X
	//
	// Find root (*), should be only one
	// Iterate BFS from root along decks (|, -) setting deck type to structural
	// Iterate BFS from root along empty cells (C). If cell has no adjecent cell - add armor in that spot

	// Directions: 4-neighborhood
	const FIntVector2 horizontal_dirs[4] = {FIntVector2(1, 0), FIntVector2(-1, 0), FIntVector2(0, 1), FIntVector2(0, -1)};
	const FIntVector2 diagonal_dirs[4] = {FIntVector2(1, 1), FIntVector2(1, -1), FIntVector2(-1, 1), FIntVector2(-1, -1)};

	// Find root
	bool has_root = false;
	FIntVector2 root_pos = FIntVector2::ZeroValue;
	for (const auto& pair : Cells)
	{
		if (pair.Value->CellType == ECellType::ROOT)
		{
			root_pos = pair.Key;
			has_root = true;
			break;
		}
	}

	if (!has_root)
	{
		// No root found, nothing to process
		return;
	}

	// BFS along decks to mark structural decks
	TArray<FIntVector2> deck_queue;
	deck_queue.Reserve(Cells.Num());
	int32 deck_queue_head = 0;

	// Enqueue deck neighbors of root
	for (const FIntVector2& d : horizontal_dirs)
	{
		FIntVector2 deck_pos = root_pos + d;
		if (TSharedPtr<FShipStructureCell> neighbor_cell = Cells.FindRef(deck_pos))
		{
			if (neighbor_cell->CellType == ECellType::DECK)
			{
				neighbor_cell->Counter = 1;
				neighbor_cell->Part->Counter = 1;
				deck_queue.Add(deck_pos);
			}
		}
	}

	// Process deck BFS
	while (deck_queue_head < deck_queue.Num())
	{
		FIntVector2 deck_pos = deck_queue[deck_queue_head++];
		TSharedPtr<FShipStructureCell> cell = Cells.FindRef(deck_pos);
		if (!cell)
		{
			continue;
		}

		cell->DeckType = EDeckType::STRUCTURAL;

		for (const FIntVector2& d : diagonal_dirs)
		{
			FIntVector2 neighbor_deck_pos = deck_pos + d;
			TSharedPtr<FShipStructureCell> neighbor_cell = Cells.FindRef(neighbor_deck_pos);
			if (neighbor_cell && neighbor_cell->Counter != 1 && neighbor_cell->CellType == ECellType::DECK)
			{
				neighbor_cell->Counter = 1;
				neighbor_cell->Part->Counter = 1;
				deck_queue.Add(neighbor_deck_pos);
			}
		}

		for (const FIntVector2& d : horizontal_dirs)
		{
			if (d.X == 0 and deck_pos.Y % 2 == 0 or d.Y == 0 and deck_pos.X % 2 == 0)
			{
				FIntVector2 neighbor_deck_pos = deck_pos + d * 2;
				TSharedPtr<FShipStructureCell> neighbor_cell = Cells.FindRef(neighbor_deck_pos);
				if (neighbor_cell && neighbor_cell->Counter != 1 && neighbor_cell->CellType == ECellType::DECK)
				{
					neighbor_cell->Counter = 1;
					neighbor_cell->Part->Counter = 1;
					deck_queue.Add(neighbor_deck_pos);
				}
			}
		}
	}

	// Any deck not visited is regular
	for (auto& pair : Cells)
	{
		if (pair.Value->CellType == ECellType::DECK && pair.Value->DeckType == EDeckType::NONE)
		{
			pair.Value->DeckType = EDeckType::REGULAR;
		}
	}

	// BFS along empty cells from root; collect missing neighbors to place armor
	TArray<FIntVector2> empty_queue;
	empty_queue.Reserve(Cells.Num());
	int32 empty_queue_head = 0;

	// Enqueue empty neighbors of root
	for (const FIntVector2& d : diagonal_dirs)
	{
		FIntVector2 neighbor_cabin_pos = root_pos + d;
		if (TSharedPtr<FShipStructureCell> neighbor_cell = Cells.FindRef(neighbor_cabin_pos))
		{
			if (neighbor_cell->CellType == ECellType::CABIN)
			{
				neighbor_cell->Counter = 2;
				empty_queue.Add(neighbor_cabin_pos);
			}
		}
	}

	while (empty_queue_head < empty_queue.Num())
	{
		FIntVector2 cabin_pos = empty_queue[empty_queue_head++];
		TSharedPtr<FShipStructureCell> cell = Cells.FindRef(cabin_pos);
		if (!cell)
		{
			continue;
		}

		// For each neighbor of this empty cell, if there is no cell present in structure => add armor there
		for (const FIntVector2& d : horizontal_dirs)
		{
			FIntVector2 neighbor_cabin_pos = cabin_pos + d * 2;
			FIntVector2 neighbor_deck_pos = cabin_pos + d;

			// If neighbor exists and is empty, continue BFS
			bool no_cell = true;
			if (TSharedPtr<FShipStructureCell> neighbor_cell = Cells.FindRef(neighbor_cabin_pos))
			{
				if (neighbor_cell->CellType == ECellType::CABIN && (neighbor_cell->Part->Counter == 1 or neighbor_cell->Part->LoadBearing == false))
				{
					no_cell = false;
					if (neighbor_cell->Counter != 2)
					{
						neighbor_cell->Counter = 2;
						empty_queue.Add(neighbor_cabin_pos);
					}
				}
			}

			if (no_cell)
			{
				TSharedPtr<FShipStructureCell> armor_cell = MakeShared<FShipStructureCell>(ECellType::DECK, cell->Part);
				armor_cell->DeckType = EDeckType::ARMOR;
				Cells.Add(neighbor_deck_pos, armor_cell);
			}
		}
	}
}

void FShipStructure::SetUpdate(FShipRenderUpdate* update)
{
	for (TSharedPtr<FShipStructurePart>& part : Parts)
	{
		part->Update = update;
	}
}

void FShipStructure::CallUpdate() const
{
	for (const auto& i : Cells)
	{
		const FIntVector2& cell_pos = i.Key;
		const TSharedPtr<FShipStructureCell> cell = i.Value;
		cell->Part->Update->SetCellMesh(cell_pos, cell->CellType, cell->DeckType);
	}
}
