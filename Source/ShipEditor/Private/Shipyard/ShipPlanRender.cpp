// Copyright (c) 2025, sillygilly. All rights reserved.

#include "ShipPlanRender.h"

#include "Components/SceneComponent.h"
#include "ShipData/ShipPartAsset.h"
#include "Shipyard/ShipCellInstance.h"
#include "Shipyard/ShipPartInstance.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
AShipPlanRender::AShipPlanRender()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;

	if (!IsValid(WallMesh))
	{
		ConstructorHelpers::FObjectFinder<UStaticMesh> wall_mesh_helper(TEXT("/Game/Wall_02.Wall_02"));
		WallMesh = wall_mesh_helper.Object;
	}

	if (!IsValid(FloorMesh))
	{
		ConstructorHelpers::FObjectFinder<UStaticMesh> floor_mesh_helper(TEXT("/Game/Floor_02.Floor_02"));
		FloorMesh = floor_mesh_helper.Object;
	}
}

bool AShipPlanRender::CopyParts(AShipPlanRender* other)
{
	// Copy part instances
	for (const TObjectPtr<UShipPartInstance>& part_instance : other->ShipPartInstances)
	{
		AddPart(part_instance->PartAsset, other->Transform(part_instance->Transform));
	}

	return true;
}

bool MergeStructures(const TMap<FIntVector2, FShipCellInstance>& structure_a,
    const TMap<FIntVector2, FShipCellInstance>& structure_b,
    TMap<FIntVector2, FShipCellInstance>& out_merged_structure)
{
	bool has_root = false;
	for (const auto& i : structure_a)
	{
		if (i.Value.CellType == ECellType::ROOT)
		{
			has_root = true;
			break;
		}
	}

	out_merged_structure = structure_a;
	for (const auto& i : structure_b)
	{
		if (out_merged_structure.Contains(i.Key))
		{
			return false;
		}
		else
		{
			if (i.Value.CellType == ECellType::ROOT)
			{
				if (has_root)
				{
					return false;
				}
				has_root = true;
			}
			out_merged_structure.Add(i.Key, i.Value);
		}
	}
	return true;
}

void ApplyStructure(const TMap<FIntVector2, FShipCellInstance>& new_structure)
{
	for (const auto& i : new_structure)
	{
		const FIntVector2& cell_pos = i.Key;
		const FShipCellInstance& cell = i.Value;
		cell.Update->SetCellMesh(cell_pos, cell.CellType, cell.DeckType);
	}
}

void ProcessStructure(TMap<FIntVector2, FShipCellInstance>& structure)
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
	for (const auto& pair : structure)
	{
		if (pair.Value.CellType == ECellType::ROOT)
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
	deck_queue.Reserve(structure.Num());
	int32 deck_queue_head = 0;

	// Enqueue deck neighbors of root
	for (const FIntVector2& d : horizontal_dirs)
	{
		FIntVector2 deck_pos = root_pos + d;
		if (FShipCellInstance* neighbor_cell = structure.Find(deck_pos))
		{
			if (neighbor_cell->CellType == ECellType::DECK)
			{
				neighbor_cell->Counter = 1;
				deck_queue.Add(deck_pos);
			}
		}
	}

	// Process deck BFS
	while (deck_queue_head < deck_queue.Num())
	{
		FIntVector2 deck_pos = deck_queue[deck_queue_head++];
		FShipCellInstance* cell = structure.Find(deck_pos);
		if (!cell)
		{
			continue;
		}

		cell->DeckType = EDeckType::STRUCTURAL;

		for (const FIntVector2& d : diagonal_dirs)
		{
			FIntVector2 neighbor_deck_pos = deck_pos + d;
			FShipCellInstance* neighbor_cell = structure.Find(neighbor_deck_pos);
			if (neighbor_cell && neighbor_cell->Counter != 1 && neighbor_cell->CellType == ECellType::DECK)
			{
				neighbor_cell->Counter = 1;
				deck_queue.Add(neighbor_deck_pos);
			}
		}

		for (const FIntVector2& d : horizontal_dirs)
		{
			if (d.X == 0 and deck_pos.Y % 2 == 0 or d.Y == 0 and deck_pos.X % 2 == 0)
			{
				FIntVector2 neighbor_deck_pos = deck_pos + d * 2;
				FShipCellInstance* neighbor_cell = structure.Find(neighbor_deck_pos);
				if (neighbor_cell && neighbor_cell->Counter != 1 && neighbor_cell->CellType == ECellType::DECK)
				{
					neighbor_cell->Counter = 1;
					deck_queue.Add(neighbor_deck_pos);
				}
			}
		}
	}

	// Any deck not visited is regular
	for (auto& pair : structure)
	{
		if (pair.Value.CellType == ECellType::DECK && pair.Value.DeckType == EDeckType::NONE)
		{
			pair.Value.DeckType = EDeckType::REGULAR;
		}
	}

	// BFS along empty cells from root; collect missing neighbors to place armor
	TArray<FIntVector2> empty_queue;
	empty_queue.Reserve(structure.Num());
	int32 empty_queue_head = 0;

	// Enqueue empty neighbors of root
	for (const FIntVector2& d : diagonal_dirs)
	{
		FIntVector2 neighbor_cabin_pos = root_pos + d;
		if (FShipCellInstance* neighbor_cell = structure.Find(neighbor_cabin_pos))
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
		FShipCellInstance* cell = structure.Find(cabin_pos);
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
			if (FShipCellInstance* neighbor_cell = structure.Find(neighbor_cabin_pos))
			{
				if (neighbor_cell->CellType == ECellType::CABIN)
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
				FShipCellInstance armor_cell;
				armor_cell.Update = cell->Update;
				armor_cell.CellType = ECellType::DECK;
				armor_cell.DeckType = EDeckType::ARMOR;
				structure.Add(neighbor_deck_pos, armor_cell);
			}
		}
	}
}

void AShipPlanRender::SetPart(UShipPartAsset* part_asset, const FShipPartTransform& part_transform)
{
	check(ShipPartInstances.IsEmpty());
	check(CellMeshComponents.IsEmpty());
	AddPart(part_asset, part_transform);
}

void AShipPlanRender::AddPart(UShipPartAsset* part_asset, const FShipPartTransform& part_transform)
{
	UShipPartInstance* ship_part_instance = NewObject<UShipPartInstance>(this, *(part_asset->GetName() + part_transform.Position.ToString()));
	ship_part_instance->PartAsset = part_asset;
	ship_part_instance->Transform = part_transform;
	ShipPartInstances.Add(ship_part_instance);
}

FShipRenderUpdate AShipPlanRender::CreateRenderUpdate()
{
	TSet<FIntVector2> current_cells;
	for (const auto& pair : CellMeshComponents)
	{
		current_cells.Add(pair.Key);
	}
	return FShipRenderUpdate(*this, ShipPartInstances, current_cells);
}

void AShipPlanRender::SetCellMesh(const FIntVector2& cell_pos_local, ECellType cell_type, EDeckType deck_type)
{
	switch (cell_type)
	{
		case ECellType::DECK:
			switch (deck_type)
			{
				case EDeckType::STRUCTURAL:
					SetCellMeshComponent(cell_pos_local, IsWall(Transform(cell_pos_local)) ? StructuralWallMesh : StructuralFloorMesh);
					break;
				case EDeckType::ARMOR:
					SetCellMeshComponent(cell_pos_local, IsWall(Transform(cell_pos_local)) ? ArmorWallMesh : ArmorFloorMesh);
					break;
				default:
					SetCellMeshComponent(cell_pos_local, IsWall(Transform(cell_pos_local)) ? WallMesh : FloorMesh);
					break;
			}
			break;
		case ECellType::CABIN:
			SetCellMeshComponent(cell_pos_local, CellMesh);
			break;
		case ECellType::NONE:
			RemoveCellMeshComponent(cell_pos_local);
			break;
		default:
			break;
	}
}

bool AShipPlanRender::SetPosition(const FIntVector2& position)
{
	if (Transform.Position == position)
	{
		return false;
	}

	SetActorLocation(FVector(position.X * MeshSpacing, GetActorLocation().Y, position.Y * MeshSpacing));
	Transform.Position = position;
	return true;
}

void AShipPlanRender::SetOverlayMaterial(UShipPartInstance* part, UMaterialInterface* material)
{
	for (const auto& cell : part->PartAsset->Cells)
	{
		if (TObjectPtr<UStaticMeshComponent> static_mesh = CellMeshComponents.FindRef(part->Transform(cell.Position)))
		{
			static_mesh->SetOverlayMaterial(material);
		}
	}
}

void AShipPlanRender::Initialize(float mesh_spacing, UMaterialInterface* ok_material, UMaterialInterface* error_material)
{
	OkOverlayMaterial = ok_material;
	ErrorOverlayMaterial = error_material;
	MeshSpacing = mesh_spacing;
}

bool AShipPlanRender::CanPlacePart(UShipPartAsset* part_asset, const FShipPartTransform& part_transform) const
{
	check(part_asset);

	for (const FShipCellData& cell : part_asset->Cells)
	{
		FIntVector2 pos = Transform(part_transform(cell.Position));
		if (UShipPartInstance* part_instance = GetPartInstance(pos))
		{
			UE_LOG(LogTemp, Warning, TEXT("Cannot place part %s at %d,%d, collides with part %s"),
			    *(part_asset->PartName.ToString()), pos.X, pos.Y, *(part_instance->GetName()));
			return false;
		}
	}

	return true;
}

UShipPartInstance* AShipPlanRender::GetPartInstance(const FIntVector2& pos) const
{
	for (TObjectPtr<UShipPartInstance> part_instance : ShipPartInstances)
	{
		for (const FShipCellData& cell : part_instance->PartAsset->Cells)
		{
			if (Transform(part_instance->Transform(cell.Position)) == pos)
			{
				return part_instance;
			}
		}
	}
	return nullptr;
}

void AShipPlanRender::RotateClockwise()
{
	Transform.RotateClockwise();
}

void AShipPlanRender::RotateCounterClockwise()
{
	Transform.RotateCounterClockwise();
}

void AShipPlanRender::Flip()
{
	Transform.Flip();
}

void AShipPlanRender::SetOk(bool ok)
{
	Ok = ok;
}

bool AShipPlanRender::IsOk() const
{
	return Ok;
}

void AShipPlanRender::DeletePartInstance(UShipPartInstance* part)
{
	for (const auto& cell : part->PartAsset->Cells)
	{
		FIntVector2 cell_pos_local = part->Transform(cell.Position);
		if (TObjectPtr<UStaticMeshComponent> static_mesh = CellMeshComponents.FindRef(cell_pos_local))
		{
			static_mesh->UnregisterComponent();
			static_mesh->DestroyComponent();
			CellMeshComponents.Remove(cell_pos_local);
		}
	}
	ShipPartInstances.Remove(part);
}

bool AShipPlanRender::IsWall(const FIntVector2& cell_pos) const
{
	return cell_pos.Y % 2 == 0 && cell_pos.X % 2 != 0;
}

UMaterialInterface* AShipPlanRender::GetRenderOverlayMaterial() const
{
	return IsOk() ? OkOverlayMaterial : ErrorOverlayMaterial;
}

void AShipPlanRender::SetCellMeshComponent(const FIntVector2& cell_pos_local, UStaticMesh* static_mesh)
{
	if (!static_mesh)
	{
		return;
	}

	UStaticMeshComponent* mesh = CellMeshComponents.FindRef(cell_pos_local);
	if (!mesh)
	{
		// Create mesh components at runtime and attach to the scene root
		const FString name = FString::Printf(TEXT("%s_MeshComponent_%d_%d"), *GetName(), cell_pos_local.X, cell_pos_local.Y);
		mesh = NewObject<UStaticMeshComponent>(this, *name);
		mesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
		mesh->RegisterComponent();
		CellMeshComponents.Add(cell_pos_local, mesh);
	}
	check(mesh);

	mesh->SetStaticMesh(static_mesh);
	FIntVector2 cell_pos = Transform(cell_pos_local);
	mesh->SetWorldLocation(FVector(cell_pos.X * MeshSpacing, GetActorLocation().Y, cell_pos.Y * MeshSpacing));
	mesh->UpdateComponentToWorld();
	if (auto* overlay_material = GetRenderOverlayMaterial())
	{
		mesh->SetOverlayMaterial(overlay_material);
	}
}

void AShipPlanRender::RemoveCellMeshComponent(const FIntVector2& cell_pos_local)
{
	if (TObjectPtr<UStaticMeshComponent> mesh = CellMeshComponents.FindRef(cell_pos_local))
	{
		mesh->UnregisterComponent();
		mesh->DestroyComponent();
		CellMeshComponents.Remove(cell_pos_local);
	}
}

void AShipPlanRender::Clear()
{
	ClearMeshes();

	ShipPartInstances.Empty();
}

void AShipPlanRender::ClearMeshes()
{
	for (auto& [key, mesh] : CellMeshComponents)
	{
		if (mesh)
		{
			mesh->UnregisterComponent();
			mesh->DestroyComponent();
		}
	}
	CellMeshComponents.Empty();
}

FShipRenderUpdate::FShipRenderUpdate(AShipPlanRender& owner, const TArray<TObjectPtr<UShipPartInstance>>& parts, TSet<FIntVector2> current_cells)
    : Owner(owner), ShipPartInstances(parts), CurrentCells(current_cells)
{
}

TMap<FIntVector2, FShipCellInstance> FShipRenderUpdate::GetStructure()
{
	TMap<FIntVector2, FShipCellInstance> structure;
	const FShipPartTransform& render_transform = Owner.GetPartTransform();
	for (const TObjectPtr<UShipPartInstance>& part_instance : ShipPartInstances)
	{
		for (FShipCellData& cell : part_instance->PartAsset->Cells)
		{
			structure.Add(render_transform(part_instance->Transform(cell.Position)), FShipCellInstance(cell.CellType, this));
		}
	}
	return structure;
}

void FShipRenderUpdate::SetCellMesh(const FIntVector2& cell_pos, ECellType cell_type, EDeckType deck_type)
{
	FIntVector2 cell_pos_local = Owner.GetPartTransform().Inverse()(cell_pos);
	CurrentCells.Remove(cell_pos_local);
	Owner.SetCellMesh(cell_pos_local, cell_type, deck_type);
}

FShipRenderUpdate::~FShipRenderUpdate()
{
	for (const FIntVector2& cell_pos_local : CurrentCells)
	{
		Owner.SetCellMesh(cell_pos_local, ECellType::NONE, EDeckType::NONE);
	}
}
