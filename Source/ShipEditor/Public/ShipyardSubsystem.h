// Copyright (c) 2024, sillygilly. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UI/ViewModels/VMBrush.h"
#include "UI/ViewModels/VMPartBrowser.h"

#include "ShipyardSubsystem.generated.h"

UCLASS()
class SHIPEDITOR_API UShipyardSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	void OnBrushIdChanged(UObject* ViewModel, UE::FieldNotification::FFieldId FieldId);

	UFUNCTION(BlueprintCallable, Category = "Shipyard")
	void DoBrush();

private:
	void AddModelViewToGlobal(UMVVMViewModelBase* ViewModel, UClass* Class, const FName& Name);
	TObjectPtr<UVMPartBrowser> VMPartBrowser;
	TObjectPtr<UVMBrush> VMBrush;
};
