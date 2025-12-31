// Copyright (c) 2025, sillygilly. All rights reserved.

#include "ShipPlanRender.h"

#include "Components/SceneComponent.h"
#include "ShipData/ShipPartAsset.h"
#include "ShipPartInstance.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
AShipPlanRender::AShipPlanRender()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> wall_mesh_helper(TEXT("/Game/Wall_02.Wall_02"));
	WallMesh = wall_mesh_helper.Object;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> floor_mesh_helper(TEXT("/Game/Floor_02.Floor_02"));
	FloorMesh = floor_mesh_helper.Object;
}

UShipPartInstance* AShipPlanRender::TryAddPart(UShipPartAsset* part_asset, const FIntVector2& pos)
{
	check(part_asset);

	FShipPartInstanceTransform part_transform;
	part_transform.Position = pos;

	if (!CanPlacePart(part_asset, part_transform))
	{
		return nullptr;
	}

	UShipPartInstance* ship_part_instance = NewObject<UShipPartInstance>(this);
	ship_part_instance->PartAsset = part_asset;
	ship_part_instance->Transform = part_transform;

	for (const FShipCellData& cell : part_asset->Cells)
	{
		FIntVector2 cell_pos = part_transform(cell.Position);
		if (cell.CellType == ECellType::DECK)
		{
			AddDeck(cell_pos, ship_part_instance);
		}
		ShipPartInstanceMap.Add(cell_pos, ship_part_instance);
	}

	return ship_part_instance;
}

void AShipPlanRender::SetPosition(const FIntVector2& position)
{
	SetActorLocation(FVector(position.X * MeshSpacing, 0.0f, position.Y * MeshSpacing));
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

bool AShipPlanRender::CanPlacePart(UShipPartAsset* part_asset, const FShipPartInstanceTransform& part_transform) const
{
	check(part_asset);

	for (const FShipCellData& cell : part_asset->Cells)
	{
		if (ShipPartInstanceMap.Contains(part_transform(cell.Position)))
		{
			return false;
		}
	}

	return true;
}

UShipPartInstance* AShipPlanRender::GetPartInstance(const FIntVector2& pos)
{
	return ShipPartInstanceMap.FindRef(pos);
}

void AShipPlanRender::DeletePartInstance(UShipPartInstance* part)
{
	for (const auto& cell : part->PartAsset->Cells)
	{
		FIntVector2 pos = part->Transform(cell.Position);
		if (TObjectPtr<UStaticMeshComponent> static_mesh = CellMeshComponents.FindRef(pos))
		{
			static_mesh->UnregisterComponent();
			static_mesh->DestroyComponent();
			CellMeshComponents.Remove(pos);
		}
		ShipPartInstanceMap.Remove(pos);
	}
}

bool AShipPlanRender::IsWall(const FIntVector2& pos) const
{
	return pos.Y % 2 == 0 && pos.X % 2 != 0;
}

void AShipPlanRender::AddDeck(const FIntVector2& pos, UShipPartInstance* ship_part_instance)
{
	if (pos.X % 2 == 0 && pos.Y % 2 == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Wrong coord for the deck %s %d,%d"), *(ship_part_instance->PartAsset.GetName()), pos.X, pos.Y);
	}
	AddDeckMesh(pos, IsWall(pos) ? WallMesh : FloorMesh);
}

void AShipPlanRender::AddDeckMesh(const FIntVector2& pos, UStaticMesh* static_mesh)
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
	mesh->SetRelativeLocation(FVector(pos.X * MeshSpacing, 0.0f, pos.Y * MeshSpacing));
	mesh->RegisterComponent();
	mesh->UpdateComponentToWorld();
	CellMeshComponents.Add(pos, mesh);
}

void AShipPlanRender::Clear()
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

	ShipPartInstanceMap.Empty();
}