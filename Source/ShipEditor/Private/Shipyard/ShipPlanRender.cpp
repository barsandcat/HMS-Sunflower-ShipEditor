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

	static ConstructorHelpers::FObjectFinder<UStaticMesh> floor_mesh_helper(TEXT("/Game/Floor_01.Floor_01"));
	FloorMesh = floor_mesh_helper.Object;
}

UShipPartInstance* AShipPlanRender::TryAddPart(UShipPartAsset* part_asset, const FIntVector2& pos)
{
	check(part_asset);
	if (!CanPlacePart(part_asset, pos))
	{
		return nullptr;
	}

	UShipPartInstance* ship_part_instance = NewObject<UShipPartInstance>(this);
	ship_part_instance->PartAsset = part_asset;
	ship_part_instance->Position = pos;

	for (const auto& wall : part_asset->Walls)
	{
		if (wall.Value)
		{
			AddWall(pos + wall.Key);
			ShipPartInstanceMap.Add(pos + wall.Key, ship_part_instance);
		}
	}

	for (const auto& floor : part_asset->Floors)
	{
		if (floor.Value)
		{
			AddFloor(pos + floor.Key);
			ShipPartInstanceMap.Add(pos + floor.Key, ship_part_instance);
		}
	}

	return ship_part_instance;
}

void AShipPlanRender::SetPosition(const FIntVector2& position)
{
	SetActorLocation(FVector(position.X * MeshSpacing, 0.0f, position.Y * MeshSpacing));
}

void AShipPlanRender::SetOverlayMaterial(UShipPartInstance* part, UMaterialInterface* material)
{
	for (const auto& wall : part->PartAsset->Walls)
	{
		if (wall.Value)
		{
			UStaticMeshComponent* static_mesh = WallMeshComponents.FindRef(part->Position + wall.Key);
			static_mesh->SetOverlayMaterial(material);
		}
	}

	for (const auto& floor : part->PartAsset->Floors)
	{
		if (floor.Value)
		{
			UStaticMeshComponent* static_mesh = FloorMeshComponents.FindRef(part->Position + floor.Key);
			static_mesh->SetOverlayMaterial(material);
		}
	}
}

bool AShipPlanRender::CanPlacePart(UShipPartAsset* part_asset, const FIntVector2& pos) const
{
	check(part_asset);

	for (const auto& wall : part_asset->Walls)
	{
		if (wall.Value)
		{
			if (ShipPartInstanceMap.Contains(pos + wall.Key))
			{
				return false;
			}
		}
	}

	for (const auto& floor : part_asset->Floors)
	{
		if (floor.Value)
		{
			if (ShipPartInstanceMap.Contains(pos + floor.Key))
			{
				return false;
			}
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
	for (const auto& wall : part->PartAsset->Walls)
	{
		if (wall.Value)
		{
			UStaticMeshComponent* static_mesh = WallMeshComponents.FindRef(part->Position + wall.Key);
			if (static_mesh)
			{
				static_mesh->UnregisterComponent();
				static_mesh->DestroyComponent();
				WallMeshComponents.Remove(part->Position + wall.Key);
			}
			ShipPartInstanceMap.Remove(part->Position + wall.Key);
		}
	}

	for (const auto& floor : part->PartAsset->Floors)
	{
		if (floor.Value)
		{
			UStaticMeshComponent* static_mesh = FloorMeshComponents.FindRef(part->Position + floor.Key);
			if (static_mesh)
			{
				static_mesh->UnregisterComponent();
				static_mesh->DestroyComponent();
				FloorMeshComponents.Remove(part->Position + floor.Key);
			}
			ShipPartInstanceMap.Remove(part->Position + floor.Key);
		}
	}
}

void AShipPlanRender::AddFloor(const FIntVector2& pos)
{
	if (FloorMeshComponents.Contains(pos))
	{
		return;
	}

	// Create mesh components at runtime and attach to the scene root

	const FString name = FString::Printf(TEXT("MeshComponent_Floor_%d_%d"), pos.X, pos.Y);
	UStaticMeshComponent* mesh = NewObject<UStaticMeshComponent>(this, *name);
	if (mesh)
	{
		mesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
		if (FloorMesh)
		{
			mesh->SetStaticMesh(FloorMesh);
		}
		mesh->SetRelativeLocation(FVector(pos.X * MeshSpacing, 0.0f, pos.Y * MeshSpacing));
		mesh->RegisterComponent();
		mesh->UpdateComponentToWorld();
		FloorMeshComponents.Add(pos, mesh);
	}
}

void AShipPlanRender::AddWall(const FIntVector2& pos)
{
	if (WallMeshComponents.Contains(pos))
	{
		return;
	}

	// Create mesh components at runtime and attach to the scene root

	const FString name = FString::Printf(TEXT("MeshComponent_%d_%d"), pos.X, pos.Y);
	UStaticMeshComponent* mesh = NewObject<UStaticMeshComponent>(this, *name);
	if (mesh)
	{
		mesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
		if (WallMesh)
		{
			mesh->SetStaticMesh(WallMesh);
		}
		mesh->SetRelativeLocation(FVector(pos.X * MeshSpacing, 0.0f, pos.Y * MeshSpacing));
		mesh->RegisterComponent();
		mesh->UpdateComponentToWorld();
		WallMeshComponents.Add(pos, mesh);
	}
}

void AShipPlanRender::Clear()
{
	for (auto& [key, mesh] : WallMeshComponents)
	{
		if (mesh)
		{
			mesh->UnregisterComponent();
			mesh->DestroyComponent();
		}
	}
	WallMeshComponents.Empty();

	for (auto& [key, mesh] : FloorMeshComponents)
	{
		if (mesh)
		{
			mesh->UnregisterComponent();
			mesh->DestroyComponent();
		}
	}
	FloorMeshComponents.Empty();

	ShipPartInstanceMap.Empty();
}