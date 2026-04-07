// Copyright (c) 2025, sillygilly. All rights reserved.

#include "UI/ViewModels/VMDeviceStatus.h"

void UVMDeviceStatus::SetName(const FText& name)
{
	UE_MVVM_SET_PROPERTY_VALUE(Name, name);
}

FText UVMDeviceStatus::GetName() const
{
	return Name;
}

void UVMDeviceStatus::SetUsage(float usage)
{
	UE_MVVM_SET_PROPERTY_VALUE(Usage, usage);
}

float UVMDeviceStatus::GetUsage() const
{
	return Usage;
}

void UVMDeviceStatus::SetCellId(const FIntVector2& cell_id)
{
	UE_MVVM_SET_PROPERTY_VALUE(CellId, cell_id);
}

FIntVector2 UVMDeviceStatus::GetCellId() const
{
	return CellId;
}

void UVMDeviceStatus::SetAvailableSector(const FDeviceSector& sector)
{
	UE_MVVM_SET_PROPERTY_VALUE(AvailableSector, sector);
}

FDeviceSector UVMDeviceStatus::GetAvailableSector() const
{
	return AvailableSector;
}

void UVMDeviceStatus::SetObstructedSector(const FDeviceSector& sector)
{
	UE_MVVM_SET_PROPERTY_VALUE(ObstructedSector, sector);
}

FDeviceSector UVMDeviceStatus::GetObstructedSector() const
{
	return ObstructedSector;
}