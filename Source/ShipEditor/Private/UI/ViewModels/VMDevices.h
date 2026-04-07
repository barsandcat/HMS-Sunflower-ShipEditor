// Copyright (c) 2025, sillygilly. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "VMDeviceStatus.h"

#include "VMDevices.generated.h"

UCLASS()
class UVMDevices : public UMVVMViewModelBase
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure, FieldNotify)
	TArray<UVMDeviceStatus*> GetDeviceList() const
	{
		TArray<UVMDeviceStatus*> result;
		DeviceMap.GenerateValueArray(result);
		return result;
	}

	TSet<FIntVector2> GetCurrentCells() const
	{
		TSet<FIntVector2> result;
		DeviceMap.GetKeys(result);
		return result;
	}

	void UpdateDeviceStatus(const FText& name, const FIntVector2& device_pos, float usage, const FDeviceSector& available_sector, const FDeviceSector& obstructed_sector);
	void RemoveDevice(const FIntVector2& device_pos);

private:
	UPROPERTY(Transient)
	TMap<FIntVector2, UVMDeviceStatus*> DeviceMap;
};
