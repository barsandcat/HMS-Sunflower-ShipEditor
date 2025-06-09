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
}

const TUVMShipPartFilterEntryArray& UVMShipPartFilter::GetVMEntries() const
{
	return VMEntries;
}

UVMShipPartFilterEntry* UVMShipPartFilter::GetEntryByName(FName name) const
{
	for (const TObjectPtr<UVMShipPartFilterEntry>& entry : VMEntries)
	{
		if (entry->GetName() == name)
		{
			return entry;
		}
	}
	return nullptr;
}