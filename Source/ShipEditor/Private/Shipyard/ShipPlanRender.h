// Copyright (c) 2025, sillygilly. All rights reserved.

#pragma once

#include "Components/StaticMeshComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "ShipPlanRender.generated.h"

class UShipPartInstance;
class UShipPartAsset;

UCLASS()
class AShipPlanRender : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShipPlanRender();

	bool TryAddPart(UShipPartAsset* part_asset, const FIntVector2& pos);
	bool CanPlacePart(UShipPartAsset* part_asset, const FIntVector2& pos) const;

	void AddWall(const FIntVector2& pos);
	void AddFloor(const FIntVector2& pos);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMesh* WallMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMesh* FloorMesh;

	UPROPERTY()
	TMap<FIntVector2, UStaticMeshComponent*> WallMeshComponents;

	UPROPERTY()
	TMap<FIntVector2, UStaticMeshComponent*> FloorMeshComponents;

	UPROPERTY()
	TMap<FIntVector2, UShipPartInstance*> ShipPartInstanceMap;

	UPROPERTY(BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> SceneComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	float MeshSpacing = 100.0f;
};
