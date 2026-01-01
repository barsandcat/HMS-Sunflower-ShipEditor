// Copyright (c) 2025, sillygilly. All rights reserved.

#pragma once

#include "Components/StaticMeshComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShipData/ShipPartInstanceTransform.h"

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
	void RotateClockwise();
	void RotateCounterClockwise();
	void Flip();

	bool TryAddParts(AShipPlanRender* other);
	UShipPartInstance* TryAddPart(UShipPartAsset* part_asset, const FShipPartInstanceTransform& part_transform);

	void DeletePartInstance(UShipPartInstance* part);
	void Clear();

	void SetOverlayMaterial(UShipPartInstance* part, UMaterialInterface* material);
	void SetDefaultOverlayMaterial(UMaterialInterface* material);
	UShipPartInstance* GetPartInstance(const FIntVector2& pos) const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMesh* WallMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMesh* FloorMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	float MeshSpacing = 50.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> SceneComponent;

private:
	void AddDeck(const FIntVector2& pos, UShipPartInstance* ship_part_instance);
	void AddDeckMesh(const FIntVector2& pos, UStaticMesh* static_mesh);
	bool IsWall(const FIntVector2& pos) const;
	void ClearMeshes();
	UShipPartInstance* AddPart(UShipPartAsset* part_asset, const FShipPartInstanceTransform& part_transform);
	void AddPartMeshes(UShipPartInstance* ship_part_instance);
	bool CanPlacePart(UShipPartAsset* part_asset, const FShipPartInstanceTransform& part_transform) const;

	UPROPERTY()
	TMap<FIntVector2, TObjectPtr<UStaticMeshComponent>> CellMeshComponents;

	UPROPERTY()
	TArray<TObjectPtr<UShipPartInstance>> ShipPartInstances;

	UPROPERTY()
	TObjectPtr<UMaterialInterface> DefaultOverlayMaterial;

	FShipPartInstanceTransform Transform;
};
