// Copyright (c) 2024, sillygilly. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "MVVMShipyard.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Tickable.h"
#include "UI/VMBrush.h"

#include "ShipyardSubsystem.generated.h"
class UMVVMViewModelCollectionObject;

UCLASS()
class SHIPEDITOR_API UShipyardSubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	void OnBrushIdChanged(UObject* ViewModel, UE::FieldNotification::FFieldId FieldId);

	virtual ETickableTickType GetTickableTickType() const override;
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;

private:
	void AddModelViewToGlobal(UMVVMViewModelBase* ViewModel, UClass* Class, const FName& Name);
	TObjectPtr<UMVVMShipyard> MVVMShipyard;
	TObjectPtr<UVMBrush> VMBrush;
};
