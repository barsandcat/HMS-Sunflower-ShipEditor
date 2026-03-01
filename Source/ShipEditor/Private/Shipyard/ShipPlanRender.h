// Copyright (c) 2025, sillygilly. All rights reserved.

#pragma once

#include "Components/StaticMeshComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Shipyard/ShipPartTransform.h"
#include "Shipyard/ShipStructure.h"

#include "ShipPlanRender.generated.h"

class UShipPartInstance;
class UShipPartAsset;
struct FShipPartTransform;
class AShipPlanRender;

class FShipRenderUpdate
{
public:
	FShipRenderUpdate() = delete;
	FShipRenderUpdate(AShipPlanRender& owner, TSet<FIntVector2> current_cells);
	void SetDeviceStatus(const FIntVector2& device_pos, float usage);
	void SetCellMesh(const FIntVector2& cell_pos, ECellType cell_type, EDeckType deck_type);
	~FShipRenderUpdate();

private:
	AShipPlanRender& Owner;
	TSet<FIntVector2> CurrentCells;
};

UCLASS()
class AShipPlanRender : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AShipPlanRender();

	bool SetPosition(const FIntVector2& position);
	void RotateClockwise();
	void RotateCounterClockwise();
	void Flip();

	void SetOk(bool ok);
	bool IsOk() const;

	bool CopyParts(AShipPlanRender* other);

	void SetPart(UShipPartAsset* part_asset, const FShipPartTransform& part_transform);

	void DeletePartInstance(UShipPartInstance* part);
	void Clear();

	void SetOverlayMaterial(UShipPartInstance* part, UMaterialInterface* material);
	void Initialize(float mesh_spacing, UMaterialInterface* ok_material, UMaterialInterface* error_material);
	UShipPartInstance* GetPartInstance(const FIntVector2& pos) const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyCategory")
	TObjectPtr<UStaticMesh> WallMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyCategory")
	TObjectPtr<UStaticMesh> FloorMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyCategory")
	TObjectPtr<UStaticMesh> StructuralWallMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyCategory")
	TObjectPtr<UStaticMesh> StructuralFloorMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyCategory")
	TObjectPtr<UStaticMesh> ArmorWallMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyCategory")
	TObjectPtr<UStaticMesh> ArmorFloorMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyCategory")
	TObjectPtr<UStaticMesh> CellMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyCategory")
	TObjectPtr<UStaticMesh> TechnicalCorridorMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyCategory")
	TObjectPtr<UStaticMesh> TechnicalCorridorRootMesh;

	UPROPERTY(BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> SceneComponent;

	const FShipPartTransform& GetPartTransform() const { return Transform; }
	void SetPartTransform(const FShipPartTransform& transform) { Transform = transform; }

	FShipRenderUpdate CreateRenderUpdate();
	FShipStructure CreateStructure();
	void SetCellMesh(const FIntVector2& cell_pos_local, ECellType cell_type, EDeckType deck_type);

private:
	void AddPart(UShipPartAsset* part_asset, const FShipPartTransform& part_transform);
	void SetCellMeshComponent(const FIntVector2& cell_pos_local, UStaticMesh* static_mesh);
	void RemoveCellMeshComponent(const FIntVector2& cell_pos_local);
	bool IsWall(const FIntVector2& cell_pos) const;
	UMaterialInterface* GetRenderOverlayMaterial() const;
	void ClearMeshes();
	bool CanPlacePart(UShipPartAsset* part_asset, const FShipPartTransform& part_transform) const;

	UPROPERTY()
	TMap<FIntVector2, TObjectPtr<UStaticMeshComponent>> CellMeshComponents;

	UPROPERTY()
	TArray<TObjectPtr<UShipPartInstance>> ShipPartInstances;

	UPROPERTY()
	TObjectPtr<UMaterialInterface> OkOverlayMaterial;

	UPROPERTY()
	TObjectPtr<UMaterialInterface> ErrorOverlayMaterial;

	UPROPERTY()
	float MeshSpacing = 50.0f;

	bool Ok = true;

	FShipPartTransform Transform;
};
