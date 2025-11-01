// Copyright (c) 2025, sillygilly. All rights reserved.

#include "ShipPlanRender.h"

#include "Components/SceneComponent.h"
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
