// Copyright (c) 2024, sillygilly. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MVVMShipyard.h"
#include "Tickable.h"
#include "ShipyardSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class SHIPEDITOR_API UShipyardSubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual ETickableTickType GetTickableTickType() const override;
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UMVVMShipyard> MVVMShiyard;
};
