// Copyright (c) 2025, sillygilly. All rights reserved.

#pragma once

#include "Components/StaticMeshComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "ShipPlanRender.generated.h"

class UShipPartInstance;
class UShipPartAsset;
struct FShipPartInstanceTransform;

UCLASS()
class AShipPlanRender : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AShipPlanRender();

	void SetPosition(const FIntVector2& position);

	UShipPartInstance* TryAddPart(UShipPartAsset* part_asset, const FIntVector2& pos);
	bool CanPlacePart(UShipPartAsset* part_asset, const FShipPartInstanceTransform& part_transform) const;
	void DeletePartInstance(UShipPartInstance* part);
	void Clear();

	void SetOverlayMaterial(UShipPartInstance* part, UMaterialInterface* material);
	UShipPartInstance* GetPartInstance(const FIntVector2& pos);

	void AddDeck(const FIntVector2& pos, UShipPartInstance* ship_part_instance);
	void AddDeckMesh(const FIntVector2& pos, UStaticMesh* static_mesh);
	bool IsWall(const FIntVector2& pos) const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMesh* WallMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMesh* FloorMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	float MeshSpacing = 50.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> SceneComponent;

	UPROPERTY()
	TMap<FIntVector2, TObjectPtr<UStaticMeshComponent>> CellMeshComponents;

	UPROPERTY()
	TMap<FIntVector2, TObjectPtr<UShipPartInstance>> ShipPartInstanceMap;

	void RotateClockwise()
	{
		XRotation = (XRotation + 1) % 4;
	}

	void RotateCounterClockwise()
	{
		XRotation = (XRotation - 1) % 4;
	}

	void Flip()
	{
		ZRotation = !ZRotation;
	}

private:
	int32 XRotation = 0;
	bool ZRotation = false;
};
