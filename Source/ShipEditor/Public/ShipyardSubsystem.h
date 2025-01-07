// Copyright (c) 2024, sillygilly. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UI/ViewModels/VMBrush.h"
#include "UI/ViewModels/VMPartBrowser.h"

#include "ShipyardSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBrushEvent);

UCLASS()
class SHIPEDITOR_API UShipyardSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	void OnBrushIdChanged(UObject* ViewModel, UE::FieldNotification::FFieldId FieldId);

	UFUNCTION(BlueprintCallable, Category = "Shipyard")
	void DoBrush();

	UPROPERTY(BlueprintAssignable, Category = "Shipyard")
	FBrushEvent OnBrushReady;

	UPROPERTY(BlueprintAssignable, Category = "Shipyard")
	FBrushEvent OnBrushCleared;

	UFUNCTION(BlueprintCallable, Category = "Shipyard")
	void SetCursorPosition(FVector WorldPosition);

private:
	void AddModelViewToGlobal(UMVVMViewModelBase* ViewModel, UClass* Class, const FName& Name);
	TObjectPtr<UVMPartBrowser> VMPartBrowser;
	TObjectPtr<UVMBrush> VMBrush;
	int32 BrushId = 0;
	TObjectPtr<AActor> Cursor;
};
