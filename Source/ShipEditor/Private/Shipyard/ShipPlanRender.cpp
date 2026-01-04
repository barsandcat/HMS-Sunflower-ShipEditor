// Copyright (c) 2025, sillygilly. All rights reserved.

#include "ShipPlanRender.h"

#include "Components/SceneComponent.h"
#include "ShipData/ShipPartAsset.h"
#include "ShipPartInstance.h"
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

bool AShipPlanRender::TryAddParts(AShipPlanRender* other)
{
	for (const TObjectPtr<UShipPartInstance>& part_instance : other->ShipPartInstances)
	{
		if (!CanPlacePart(part_instance->PartAsset, other->Transform(part_instance->Transform)))
		{
			return false;
		}
	}

	for (const TObjectPtr<UShipPartInstance>& part_instance : other->ShipPartInstances)
	{
		AddPart(part_instance->PartAsset, other->Transform(part_instance->Transform));
	}

	return true;
}

UShipPartInstance* AShipPlanRender::TryAddPart(UShipPartAsset* part_asset, const FShipPartTransform& part_transform)
{
	check(part_asset);

	if (!CanPlacePart(part_asset, part_transform))
	{
		return nullptr;
	}

	return AddPart(part_asset, part_transform);
}

UShipPartInstance* AShipPlanRender::AddPart(UShipPartAsset* part_asset, const FShipPartTransform& part_transform)
{
	UShipPartInstance* ship_part_instance = NewObject<UShipPartInstance>(this, *(part_asset->GetName() + part_transform.Position.ToString()));
	ship_part_instance->PartAsset = part_asset;
	ship_part_instance->Transform = part_transform;

	AddPartMeshes(ship_part_instance);

	ShipPartInstances.Add(ship_part_instance);

	return ship_part_instance;
}

void AShipPlanRender::AddPartMeshes(UShipPartInstance* ship_part_instance)
{
	for (const FShipCellData& cell : ship_part_instance->PartAsset->Cells)
	{
		FIntVector2 cell_pos = Transform(ship_part_instance->Transform(cell.Position));
		switch (cell.CellType)
		{
			case ECellType::DECK:
				AddCellMesh(cell_pos, IsWall(cell_pos) ? WallMesh : FloorMesh);
				break;
			case ECellType::EMPTY_CELL:
				AddCellMesh(cell_pos, CellMesh);
				break;
			default:
				break;
		}
	}
}

void AShipPlanRender::SetPosition(const FIntVector2& position)
{
	SetActorLocation(FVector(position.X * MeshSpacing, 0.0f, position.Y * MeshSpacing));
	Transform.Position = position;
}

void AShipPlanRender::SetOverlayMaterial(UShipPartInstance* part, UMaterialInterface* material)
{
	for (const auto& cell : part->PartAsset->Cells)
	{
		if (TObjectPtr<UStaticMeshComponent> static_mesh = CellMeshComponents.FindRef(Transform(part->Transform(cell.Position))))
		{
			static_mesh->SetOverlayMaterial(material);
		}
	}
}

void AShipPlanRender::Initialize(float mesh_spacing, UMaterialInterface* material)
{
	DefaultOverlayMaterial = material;
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
	ClearMeshes();
	Transform.RotateClockwise();
	for (const TObjectPtr<UShipPartInstance>& part_instance : ShipPartInstances)
	{
		AddPartMeshes(part_instance);
	}
}

void AShipPlanRender::RotateCounterClockwise()
{
	ClearMeshes();
	Transform.RotateCounterClockwise();
	for (const TObjectPtr<UShipPartInstance>& part_instance : ShipPartInstances)
	{
		AddPartMeshes(part_instance);
	}
}

void AShipPlanRender::Flip()
{
	ClearMeshes();
	Transform.Flip();
	for (const TObjectPtr<UShipPartInstance>& part_instance : ShipPartInstances)
	{
		AddPartMeshes(part_instance);
	}
}

void AShipPlanRender::DeletePartInstance(UShipPartInstance* part)
{
	for (const auto& cell : part->PartAsset->Cells)
	{
		FIntVector2 pos = Transform(part->Transform(cell.Position));
		if (TObjectPtr<UStaticMeshComponent> static_mesh = CellMeshComponents.FindRef(pos))
		{
			static_mesh->UnregisterComponent();
			static_mesh->DestroyComponent();
			CellMeshComponents.Remove(pos);
		}
	}
	ShipPartInstances.Remove(part);
}

bool AShipPlanRender::IsWall(const FIntVector2& pos) const
{
	return pos.Y % 2 == 0 && pos.X % 2 != 0;
}

void AShipPlanRender::AddCellMesh(const FIntVector2& pos, UStaticMesh* static_mesh)
{
	if (!static_mesh || CellMeshComponents.Contains(pos))
	{
		return;
	}

	// Create mesh components at runtime and attach to the scene root
	const FString name = FString::Printf(TEXT("MeshComponent_%d_%d"), pos.X, pos.Y);
	UStaticMeshComponent* mesh = NewObject<UStaticMeshComponent>(this, *name);
	check(mesh);
	mesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	mesh->SetStaticMesh(static_mesh);
	mesh->SetWorldLocation(FVector(pos.X * MeshSpacing, 0.0f, pos.Y * MeshSpacing));
	mesh->RegisterComponent();
	mesh->UpdateComponentToWorld();
	if (DefaultOverlayMaterial)
	{
		mesh->SetOverlayMaterial(DefaultOverlayMaterial);
	}
	CellMeshComponents.Add(pos, mesh);
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
