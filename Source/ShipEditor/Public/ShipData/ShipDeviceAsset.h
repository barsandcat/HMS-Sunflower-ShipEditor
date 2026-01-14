// Copyright (c) 2025, sillygilly. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "ShipDeviceAsset.generated.h"

UENUM(BlueprintType)
enum class EDeviceType : uint8
{
	NONE = 0,
	GUN = 1,
	ENGINE = 2,
	FUEL_TANK = 3,
	POWER = 4,
	MAGAZINE = 5,
	QUARTERS = 6,
	BRIDGE = 7,
};

UCLASS()
class SHIPEDITOR_API UShipDeviceAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EDeviceType DeviceType = EDeviceType::NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Elevation = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool DynamicMount = false;
};
