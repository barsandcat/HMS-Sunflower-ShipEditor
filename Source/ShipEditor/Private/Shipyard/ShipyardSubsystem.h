// Copyright (c) 2024, sillygilly. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Misc/Optional.h"
#include "ShipPlanCell.h"
#include "Shipyard/Filter/IPartFilter.h"
#include "Shipyard/ShipPartInstance.h"
#include "Subsystems/WorldSubsystem.h"
#include "UI/ViewModels/VMBrush.h"
#include "UI/ViewModels/VMPartBrowser.h"
#include "UI/ViewModels/VMShipPlan.h"

#include <set>

#include "ShipyardSubsystem.generated.h"

class AShipPlanRender;
class UShipPartAsset;
class UShipPartInstance;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBrushEvent);

UCLASS(config = Game)
class SHIPEDITOR_API UShipyardSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	UShipyardSubsystem();
	virtual void Initialize(FSubsystemCollectionBase& collection) override;

	UFUNCTION(BlueprintCallable, Category = "Shipyard")
	void DoBrush();

	UFUNCTION(BlueprintCallable, Category = "Shipyard")
	void Select();

	UFUNCTION(BlueprintCallable, Category = "Shipyard")
	void Delete();

	UFUNCTION(BlueprintCallable, Category = "Shipyard")
	void Grab();

	UFUNCTION(BlueprintCallable, Category = "Shipyard")
	void RotateBrushClockwise();

	UFUNCTION(BlueprintCallable, Category = "Shipyard")
	void RotateBrushCounterClockwise();

	UFUNCTION(BlueprintCallable, Category = "Shipyard")
	void FlipBrush();

	UFUNCTION(BlueprintCallable, Category = "Shipyard")
	void SetBrushId(FName brush_id);

	UFUNCTION(BlueprintCallable, Category = "Shipyard")
	void SaveShipPlan(const FString& name);

	UFUNCTION(BlueprintCallable, Category = "Shipyard")
	void LoadShipPlan(const FString& name);

	UPROPERTY(BlueprintAssignable, Category = "Shipyard")
	FBrushEvent OnBrushReady;

	UPROPERTY(BlueprintAssignable, Category = "Shipyard")
	FBrushEvent OnBrushCleared;

	UFUNCTION(BlueprintCallable, Category = "Shipyard")
	void UpdatePartList();

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UVMPartBrowser> VMPartBrowser;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UVMBrush> VMBrush;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UVMShipPlan> VMShipPlan;

	void SetCursorPosition(const TOptional<FVector>& world_position);
	void SetBrushPosition(const TOptional<FVector>& world_position);

	UPROPERTY()
	TSubclassOf<AActor> CursorClassPtr;

	UPROPERTY(config)
	TSoftClassPtr<AActor> CursorClass;

	void OnCategorySelected(UObject* view_model, UE::FieldNotification::FFieldId field_id);
	void OnFilterSelected(UObject* view_model, UE::FieldNotification::FFieldId field_id);

private:
	UPROPERTY()
	TObjectPtr<AShipPlanRender> ShipPlanRender;

	UPROPERTY()
	TObjectPtr<AShipPlanRender> PreviewRender;

	TArray<TSharedPtr<IPartFilter>> FilterList;
	std::set<int32> GetSelectedCategories() const;
	bool IsAllowedByFiters(const TObjectPtr<UVMShipPart>& part_vm) const;
	void AddCategory(TUVMShipPartCategoryArray& list, const FText& name, int32 id);

	void LoadAllShipPartAssetsAsync();
	void OnShipPartAssetsLoaded();

	FName BrushId = NAME_None;
	// TObjectPtr<AShipPlanCell> Brush;
	TObjectPtr<AActor> Cursor;
	TObjectPtr<UShipPartInstance> Selection;

	TObjectPtr<UMaterialInterface> SelectionMaterial;
	TObjectPtr<UMaterialInterface> PreviewMaterial;

	TMap<FName, UShipPartAsset*> PartAssetMap;
	TArray<FPrimaryAssetId> ShipPartAssetIds;

	TUVMShipPartArray PartList;
};
