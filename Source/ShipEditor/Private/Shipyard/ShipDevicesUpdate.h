// Copyright (c) 2025, sillygilly. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/ViewModels/VMDevices.h"

class FShipDevicesUpdate
{
public:
	FShipDevicesUpdate() = delete;
	FShipDevicesUpdate(UVMDevices& devices_vm);
	void SetDeviceStatus(const FText& name, const FIntVector2& device_pos, float usage, const FDeviceSector& sector, const FDeviceSector& available_sector);
	~FShipDevicesUpdate();

private:
	UVMDevices& VMDevices;
	TSet<FIntVector2> CurrentCells;
};