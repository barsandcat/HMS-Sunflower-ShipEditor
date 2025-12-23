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

	for (const FShipDeckData& deck : part_asset->Decks)
	{
		AddDeck(part_transform(deck.Position), ship_part_instance);
	}

	return ship_part_instance;
}

void AShipPlanRender::SetPosition(const FIntVector2& position)
{
	SetActorLocation(FVector(position.X * MeshSpacing, 0.0f, position.Y * MeshSpacing));
}

void AShipPlanRender::SetOverlayMaterial(UShipPartInstance* part, UMaterialInterface* material)
{
	for (const auto& deck : part->PartAsset->Decks)
	{
		UStaticMeshComponent* static_mesh = DeckMeshComponents.FindRef(part->Transform(deck.Position));
		static_mesh->SetOverlayMaterial(material);
	}
}

bool AShipPlanRender::CanPlacePart(UShipPartAsset* part_asset, const FShipPartInstanceTransform& part_transform) const
{
	check(part_asset);

	for (const FShipDeckData& deck : part_asset->Decks)
	{
		if (ShipPartInstanceMap.Contains(part_transform(deck.Position)))
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
	for (const auto& deck : part->PartAsset->Decks)
	{
		FIntVector2 pos = part->Transform(deck.Position);
		UStaticMeshComponent* static_mesh = DeckMeshComponents.FindRef(pos);
		if (static_mesh)
		{
			static_mesh->UnregisterComponent();
			static_mesh->DestroyComponent();
			DeckMeshComponents.Remove(pos);
		}
		ShipPartInstanceMap.Remove(pos);
	}
}

bool AShipPlanRender::IsWall(const FIntVector2& pos) const
{
	return pos.Y % 2 != 0 && pos.X % 2 == 0;
}

void AShipPlanRender::AddDeck(const FIntVector2& pos, UShipPartInstance* ship_part_instance)
{
	if (pos.X % 2 == 0 && pos.Y % 2 == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Wrong coord for the deck %s %d,%d"), *(ship_part_instance->PartAsset.GetName()), pos.X, pos.Y);
	}
	AddDeckMesh(pos, IsWall(pos) ? WallMesh : FloorMesh);
	ShipPartInstanceMap.Add(pos, ship_part_instance);
}

void AShipPlanRender::AddDeckMesh(const FIntVector2& pos, UStaticMesh* static_mesh)
{
	if (!static_mesh || DeckMeshComponents.Contains(pos))
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
	DeckMeshComponents.Add(pos, mesh);
}

void AShipPlanRender::Clear()
{
	for (auto& [key, mesh] : DeckMeshComponents)
	{
		if (mesh)
		{
			mesh->UnregisterComponent();
			mesh->DestroyComponent();
		}
	}
	DeckMeshComponents.Empty();

	ShipPartInstanceMap.Empty();
}