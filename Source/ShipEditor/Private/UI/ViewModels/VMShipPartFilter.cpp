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

TArray<FName> UVMShipPartFilter::GetOptions() const
{
	TArray<FName> Options;
	for (const TObjectPtr<UVMShipPartFilterEntry>& entry : VMEntries)
	{
		Options.Add(entry->GetName());
	}
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

void UVMShipPartFilter::SetVMEntries(const TUVMShipPartFilterEntryArray& entries)
{
	UE_MVVM_SET_PROPERTY_VALUE(VMEntries, entries);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetOptions);
}

const TUVMShipPartFilterEntryArray& UVMShipPartFilter::GetVMEntries() const
{
	return VMEntries;
}
