// Copyright (c) 2024, sillygilly. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ShipPlanCell.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UI/ViewModels/VMBrush.h"
#include "UI/ViewModels/VMPartBrowser.h"

#include "ShipyardSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBrushEvent);

UCLASS(config = Game)
class SHIPEDITOR_API UShipyardSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UShipyardSubsystem();
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	void OnBrushIdChanged(UObject* ViewModel, UE::FieldNotification::FFieldId FieldId);

	UFUNCTION(BlueprintCallable, Category = "Shipyard")
	void DoBrush();

	UFUNCTION(BlueprintCallable, Category = "Shipyard")
	void Select();

	UFUNCTION(BlueprintCallable, Category = "Shipyard")
	void Delete();

	UFUNCTION(BlueprintCallable, Category = "Shipyard")
	void Grab();

	UPROPERTY(BlueprintAssignable, Category = "Shipyard")
	FBrushEvent OnBrushReady;

	UPROPERTY(BlueprintAssignable, Category = "Shipyard")
	FBrushEvent OnBrushCleared;

	UFUNCTION(BlueprintCallable, Category = "Shipyard")
	void SetCursorPosition(FVector WorldPosition);

	UPROPERTY()
	TSubclassOf<AActor> CursorClassPtr;

	UPROPERTY(config)
	TSoftClassPtr<AActor> CursorClass;

private:
	void AddModelViewToGlobal(UMVVMViewModelBase* ViewModel, UClass* Class, const FName& Name);
	TObjectPtr<UVMPartBrowser> VMPartBrowser;
	TObjectPtr<UVMBrush> VMBrush;
	int32 BrushId = 0;
	TObjectPtr<AActor> Cursor;
	TObjectPtr<AShipPlanCell> Selection;
	TMap<FIntVector2, TObjectPtr<AShipPlanCell>> ShipPlan;

	TSubclassOf<AShipPlanCell> PartClassPtr;
	TObjectPtr<UMaterialInterface> SelectionMaterial;
};
