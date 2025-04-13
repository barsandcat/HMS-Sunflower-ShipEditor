// Copyright (c) 2024, sillygilly. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Misc/Optional.h"
#include "ShipPlanCell.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UI/ViewModels/VMBrush.h"
#include "UI/ViewModels/VMPartBrowser.h"
#include "Shipyard/IPartFilter.h"

#include <set>

#include "ShipyardSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBrushEvent);

UCLASS(config = Game)
class SHIPEDITOR_API UShipyardSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UShipyardSubsystem();
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, Category = "Shipyard")
	void DoBrush();

	UFUNCTION(BlueprintCallable, Category = "Shipyard")
	void Select();

	UFUNCTION(BlueprintCallable, Category = "Shipyard")
	void Delete();

	UFUNCTION(BlueprintCallable, Category = "Shipyard")
	void Grab();

	UFUNCTION(BlueprintCallable, Category = "Shipyard")
	void SetBrushId(int32 brush_id);

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

	void SetCursorPosition(const TOptional<FVector>& WorldPosition);
	void SetBrushPosition(const TOptional<FVector>& WorldPosition);

	UPROPERTY()
	TSubclassOf<AActor> CursorClassPtr;

	UPROPERTY(config)
	TSoftClassPtr<AActor> CursorClass;

	void OnCategorySelected(UObject* ViewModel, UE::FieldNotification::FFieldId FieldId);
	void OnFilterSelected(UObject* ViewModel, UE::FieldNotification::FFieldId FieldId);

private:
	TArray<TSharedPtr<IPartFilter>> FilterList;
	std::set<int32> GetSelectedCategories() const;
	bool IsAllowedByFiters(const TObjectPtr<UVMShipPart>& part_vm) const;
	void AddCategory(TUVMShipPartCategoryArray& list, const FText& name, int32 id);
	void AddFilter(TUVMShipPartFilterArray& list, const TArray<FName>& options, int32 id);
	TSubclassOf<AShipPlanCell> GetPartClass(int32 part_id) const;
	int32 BrushId = 0;
	TObjectPtr<AShipPlanCell> Brush;
	TObjectPtr<AActor> Cursor;
	TObjectPtr<AShipPlanCell> Selection;
	TMap<FIntVector2, TObjectPtr<AShipPlanCell>> ShipPlan;

	TObjectPtr<UMaterialInterface> SelectionMaterial;
	TObjectPtr<UMaterialInterface> PreviewMaterial;
	TMap<int32, TSubclassOf<AShipPlanCell>> PartClassMap;

	TUVMShipPartArray PartList;
};
