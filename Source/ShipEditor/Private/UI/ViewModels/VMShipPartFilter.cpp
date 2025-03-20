// Copyright (c) 2025, sillygilly. All rights reserved.

#include "UI/ViewModels/VMShipPartFilter.h"

void UVMShipPartFilter::SetFilterId(int32 filter_id)
{
	UE_MVVM_SET_PROPERTY_VALUE(FilterId, filter_id);
}

int32 UVMShipPartFilter::GetFilterId() const
{
	return FilterId;
}

void UVMShipPartFilter::SetOptions(const TArray<FName>& options)
{
	UE_MVVM_SET_PROPERTY_VALUE(Options, options);
}

const TArray<FName>& UVMShipPartFilter::GetOptions() const
{
	return Options;
}

void UVMShipPartFilter::SetSelected(FName selected)
{
	UE_MVVM_SET_PROPERTY_VALUE(Selected, selected);
}

FName UVMShipPartFilter::GetSelected() const
{
	return Selected;
}
