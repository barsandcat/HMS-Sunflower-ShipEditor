// Copyright (c) 2024, sillygilly. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MVVMShipyard.h"
#include "ShipyardSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class SHIPEDITOR_API UShipyardSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UMVVMShipyard> MVVMShiyard;
};
