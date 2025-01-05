// Copyright (c) 2024, sillygilly. All rights reserved.

#include "UI/ViewModels/VMBrush.h"

void UVMBrush::SetReady(bool NewReady)
{
	UE_MVVM_SET_PROPERTY_VALUE(Ready, NewReady);
}
