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

void UVMDeviceStatus::SetPosition(const FIntVector2& position)
{
	UE_MVVM_SET_PROPERTY_VALUE(Position, position);
}

FIntVector2 UVMDeviceStatus::GetPosition() const
{
	return Position;
}