// Copyright (c) 2025, sillygilly. All rights reserved.

#include "Shipyard/ShipStructure.h"

#include "Shipyard/ShipPartInstance.h"
#include "Shipyard/ShipPlanRender.h"

namespace
{

bool IsDeckIntersection(const FIntVector2& pos)
{
	return pos.X % 2 != 0 && pos.Y % 2 != 0;
}

bool IsDeckVertical(const FIntVector2& pos)
{
	return pos.X % 2 != 0 && pos.Y % 2 == 0;
}

bool IsDeckHorizontal(const FIntVector2& pos)
{
	return pos.X % 2 == 0 && pos.Y % 2 != 0;
}

bool IsCabin(const FIntVector2& pos)
{
	return pos.X % 2 == 0 && pos.Y % 2 == 0;
}

}    // namespace

FShipStructure::FShipStructure(const FShipPartTransform& render_transform, const TArray<TObjectPtr<UShipPartInstance>>& part_instances)
{
	Parts.SetNum(part_instances.Num());
	Devices.SetNum(part_instances.Num());
	for (int32 i = 0; i < Parts.Num(); i++)
	{
		UShipPartInstance* part_instance = part_instances[i];
		TSharedPtr<FShipStructurePart> part = MakeShared<FShipStructurePart>();
		Parts[i] = part;
		TSharedPtr<FShipStructureDevice> device = MakeShared<FShipStructureDevice>(part_instance->PartAsset->Device->DeviceType, part);
		Devices[i] = device;
		part->Device = device;
		for (FShipCellData& cell : part_instance->PartAsset->Cells)
		{
			if (cell.CellType == ECellType::ROOT)
			{
				device->WallConnected = false;
				if (device->DeviceType == EDeviceType::BRIDGE)
				{
					Root = render_transform(part_instance->Transform(cell.Position));
				}
			}
			Cells.Add(render_transform(part_instance->Transform(cell.Position)), MakeShared<FShipStructureCell>(cell.CellType, part, device));
		}
	}
}

bool FShipStructure::MergeStructures(const FShipStructure& structure_a, const FShipStructure& structure_b, FShipStructure& out_merged_structure)
{
	if (structure_a.Root.IsSet() && structure_b.Root.IsSet())
	{
		return false;
	}

	out_merged_structure = structure_a;
	out_merged_structure.Cells.Reserve(structure_a.Cells.Num() + structure_b.Cells.Num());
	out_merged_structure.Parts.Reserve(structure_a.Parts.Num() + structure_b.Parts.Num());
	out_merged_structure.Devices.Reserve(structure_a.Devices.Num() + structure_b.Devices.Num());
	out_merged_structure.Root = structure_a.Root.IsSet() ? structure_a.Root : structure_b.Root;

	for (const auto& i : structure_b.Parts)
	{
		out_merged_structure.Parts.Add(i);
	}
	for (const auto& i : structure_b.Devices)
	{
		out_merged_structure.Devices.Add(i);
	}

	for (const auto& i : structure_b.Cells)
	{
		if (out_merged_structure.Cells.Contains(i.Key))
		{
			return false;
		}
		else
		{
			out_merged_structure.Cells.Add(i.Key, i.Value);
		}
	}
	return true;
}

void FShipStructure::Process()
{
	if (!Root)
	{
		// No root, nothing to process
		return;
	}
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
	const FIntVector2 dirs[4] = {FIntVector2(1, 0), FIntVector2(-1, 0), FIntVector2(0, 1), FIntVector2(0, -1)};
	const FIntVector2 diagonal_dirs[4] = {FIntVector2(1, 1), FIntVector2(1, -1), FIntVector2(-1, 1), FIntVector2(-1, -1)};
	const FIntVector2 vertical_dirs[2] = {FIntVector2(0, 1), FIntVector2(0, -1)};
	const FIntVector2 horizontal_dirs[2] = {FIntVector2(1, 0), FIntVector2(-1, 0)};

	// BFS along decks to mark structural decks
	TArray<FIntVector2> deck_queue;
	deck_queue.Reserve(Cells.Num());
	int32 deck_queue_head = 0;

	TSet<FIntVector2> visited_intersections;

	// Enqueue root
	deck_queue.Add(*Root);
	visited_intersections.Add(*Root);

	// Process deck BFS
	while (deck_queue_head < deck_queue.Num())
	{
		FIntVector2 deck_pos = deck_queue[deck_queue_head++];
		TSharedPtr<FShipStructureCell> cell = Cells.FindRef(deck_pos);

		if (IsDeckIntersection(deck_pos))
		{
			if (cell && cell->CellType == ECellType::ROOT)
			{
				cell->Device->WallConnected = true;
			}

			for (const FIntVector2& d : dirs)
			{
				FIntVector2 neighbor_deck_pos = deck_pos + d;
				TSharedPtr<FShipStructureCell> neighbor_cell = Cells.FindRef(neighbor_deck_pos);
				if (neighbor_cell && neighbor_cell->Visited != 1 && neighbor_cell->CellType == ECellType::DECK)
				{
					neighbor_cell->Visited = 1;
					deck_queue.Add(neighbor_deck_pos);
				}
			}
		}

		if (cell)
		{
			cell->DeckType = EDeckType::STRUCTURAL;
			if (IsDeckVertical(deck_pos))
			{
				for (const FIntVector2& d : vertical_dirs)
				{
					FIntVector2 neighbor_deck_pos = deck_pos + d;
					if (!visited_intersections.Contains(neighbor_deck_pos))
					{
						deck_queue.Add(neighbor_deck_pos);
					}
				}
			}
			if (IsDeckHorizontal(deck_pos))
			{
				for (const FIntVector2& d : horizontal_dirs)
				{
					FIntVector2 neighbor_deck_pos = deck_pos + d;
					if (!visited_intersections.Contains(neighbor_deck_pos))
					{
						deck_queue.Add(neighbor_deck_pos);
					}
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
		FIntVector2 neighbor_cabin_pos = *Root + d;
		if (TSharedPtr<FShipStructureCell> neighbor_cell = Cells.FindRef(neighbor_cabin_pos))
		{
			if (neighbor_cell->CellType != ECellType::NONE)
			{
				neighbor_cell->Visited = 2;
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
		for (const FIntVector2& d : dirs)
		{
			FIntVector2 neighbor_cabin_pos = cabin_pos + d * 2;
			FIntVector2 neighbor_deck_pos = cabin_pos + d;

			// If neighbor exists and is empty, continue BFS
			bool no_cell = true;
			if (TSharedPtr<FShipStructureCell> neighbor_cell = Cells.FindRef(neighbor_cabin_pos))
			{
				if (neighbor_cell->CellType != ECellType::NONE && neighbor_cell->Device->WallConnected)
				{
					no_cell = false;
					if (neighbor_cell->Visited != 2)
					{
						neighbor_cell->Visited = 2;
						empty_queue.Add(neighbor_cabin_pos);
					}
				}
			}

			if (no_cell)
			{
				TSharedPtr<FShipStructureCell> armor_cell = MakeShared<FShipStructureCell>(ECellType::DECK, cell->Part, cell->Device);
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
