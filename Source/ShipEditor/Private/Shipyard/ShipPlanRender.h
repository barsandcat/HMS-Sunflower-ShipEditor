// Copyright (c) 2025, sillygilly. All rights reserved.

#pragma once

#include "Components/StaticMeshComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Shipyard/ShipCellInstance.h"
#include "Shipyard/ShipPartTransform.h"

#include "ShipPlanRender.generated.h"

class UShipPartInstance;
class UShipPartAsset;
struct FShipPartTransform;

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
	TMap<FIntVector2, FShipCellInstance> GetStructure();

	bool TryAddParts(AShipPlanRender* other);
	UShipPartInstance* AddPart(UShipPartAsset* part_asset, const FShipPartTransform& part_transform);

	void DeletePartInstance(UShipPartInstance* part);
	void Clear();

	void SetOverlayMaterial(UShipPartInstance* part, UMaterialInterface* material);
	void Initialize(float mesh_spacing, UMaterialInterface* material);
	UShipPartInstance* GetPartInstance(const FIntVector2& pos) const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyCategory")
	TObjectPtr<UStaticMesh> WallMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyCategory")
	TObjectPtr<UStaticMesh> FloorMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyCategory")
	TObjectPtr<UStaticMesh> CellMesh;

	UPROPERTY(BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> SceneComponent;

	const FShipPartTransform& GetPartTransform() const { return Transform; }
	void SetPartTransform(const FShipPartTransform& transform) { Transform = transform; }
	void AddCellMesh(const FIntVector2& cell_pos, ECellType cell_type);
	void AddPartMeshes(UShipPartInstance* ship_part_instance);

private:
	void AddCellMeshComponent(const FIntVector2& pos, UStaticMesh* static_mesh);
	bool IsWall(const FIntVector2& pos) const;
	void ClearMeshes();
	bool CanPlacePart(UShipPartAsset* part_asset, const FShipPartTransform& part_transform) const;

	UPROPERTY()
	TMap<FIntVector2, TObjectPtr<UStaticMeshComponent>> CellMeshComponents;

	UPROPERTY()
	TArray<TObjectPtr<UShipPartInstance>> ShipPartInstances;

	UPROPERTY()
	TObjectPtr<UMaterialInterface> DefaultOverlayMaterial;

	UPROPERTY()
	float MeshSpacing = 50.0f;

	FShipPartTransform Transform;
};

void AddMeshes(TMap<FIntVector2, FShipCellInstance>& new_structure);
