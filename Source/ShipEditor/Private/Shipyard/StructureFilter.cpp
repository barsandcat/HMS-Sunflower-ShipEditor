// Copyright (c) 2025, sillygilly. All rights reserved.


#include "Shipyard/StructureFilter.h"

StructureFilter::StructureFilter(const INotifyFieldValueChanged::FFieldValueChangedDelegate& delegate)
    : IPartFilter(2, delegate)
{
	InitializeOptions(StructureFilterOptions);
}

bool StructureFilter::IsAllowed(const FName& option, const TObjectPtr<UVMShipPart>& part) const
{
	if (option == StructureFilterOptions[1])
	{
		return part->GetLoadbearing();
	}
	if (option == StructureFilterOptions[2])
	{
		return !part->GetLoadbearing();
	}
	return true;
}
