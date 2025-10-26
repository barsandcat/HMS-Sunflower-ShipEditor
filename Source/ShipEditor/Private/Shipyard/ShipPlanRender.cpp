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

	static ConstructorHelpers::FObjectFinder<UStaticMesh> wall_mesh_helper(TEXT("/Game/Wall_01.Wall_01"));
	WallMesh = wall_mesh_helper.Object;
}

void AShipPlanRender::AddWall(const FInt32Point& pos)
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
		mesh->RegisterComponent();
		mesh->SetRelativeLocation(FVector(pos.X * MeshSpacing, pos.Y * MeshSpacing, 0.0f));
		WallMeshComponents.Add(pos, mesh);
	}
}
