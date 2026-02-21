// Copyright (c) 2025, sillygilly. All rights reserved.

#include "ShipPlanRender.h"

#include "Components/SceneComponent.h"
#include "ShipData/ShipPartAsset.h"
#include "Shipyard/ShipPartInstance.h"
#include "Shipyard/ShipStructure.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
AShipPlanRender::AShipPlanRender()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;
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
	return FShipRenderUpdate(*this, current_cells);
}

FShipStructure AShipPlanRender::CreateStructure()
{
	return FShipStructure(Transform, ShipPartInstances);
}

void AShipPlanRender::SetDeviceStatus(const FIntVector2& pos, float usage)
{
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
		case ECellType::TECHNICAL_CORRIDOR:
			SetCellMeshComponent(cell_pos_local, TechnicalCorridorMesh);
			break;
		case ECellType::TECHNICAL_CORRIDOR_ROOT:
			SetCellMeshComponent(cell_pos_local, TechnicalCorridorRootMesh);
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

FShipRenderUpdate::FShipRenderUpdate(AShipPlanRender& owner, TSet<FIntVector2> current_cells)
    : Owner(owner), CurrentCells(current_cells)
{
}

void FShipRenderUpdate::SetDeviceStatus(const FIntVector2& device_pos, float usage)
{
	Owner.SetDeviceStatus(device_pos, usage);
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
