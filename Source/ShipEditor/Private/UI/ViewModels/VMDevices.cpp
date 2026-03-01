// Copyright (c) 2025, sillygilly. All rights reserved.

#include "UI/ViewModels/VMDevices.h"

#include "VMDevices.h"

void UVMDevices::UpdateDeviceStatus(const FText& name, const FIntVector2& device_pos, float usage)
{
	if (UVMDeviceStatus* device = DeviceMap.FindRef(device_pos))
	{
		device->SetName(name);
		device->SetUsage(usage);
	}
	else
	{
		UVMDeviceStatus* new_device = NewObject<UVMDeviceStatus>();
		new_device->SetCellId(device_pos);
		new_device->SetName(name);
		new_device->SetUsage(usage);
		DeviceMap.Add(device_pos, new_device);
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetDeviceList);
	}
}

void UVMDevices::RemoveDevice(const FIntVector2& device_pos)
{
	DeviceMap.Remove(device_pos);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetDeviceList);
}
