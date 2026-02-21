// Copyright (c) 2025, sillygilly. All rights reserved.

#include "UI/ViewModels/VMDevices.h"

void UVMDevices::SetDeviceList(const TArray<TObjectPtr<UVMDeviceStatus>>& device_list)
{
	UE_MVVM_SET_PROPERTY_VALUE(DeviceList, device_list);
}

const TArray<TObjectPtr<UVMDeviceStatus>>& UVMDevices::GetDeviceList() const
{
	return DeviceList;
}
