// Copyright (c) 2024, sillygilly. All rights reserved.

#include "UI/ViewModels/VMShipPart.h"

void UVMShipPart::SetName(const FText& NewName)
{
	UE_MVVM_SET_PROPERTY_VALUE(Name, NewName);
}

void UVMShipPart::SetPartId(int32 NewPartId)
{
	UE_MVVM_SET_PROPERTY_VALUE(PartId, NewPartId);
}
