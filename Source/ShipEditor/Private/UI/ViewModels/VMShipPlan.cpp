// Copyright (c) 2025, sillygilly. All rights reserved.


#include "UI/ViewModels/VMShipPlan.h"

void UVMShipPlan::SetName(const FString& name)
{
	UE_MVVM_SET_PROPERTY_VALUE(Name, name);
}

FString UVMShipPlan::GetName() const
{
	return Name;
}
