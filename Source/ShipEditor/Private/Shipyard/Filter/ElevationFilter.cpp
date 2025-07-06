// Copyright (c) 2025, sillygilly. All rights reserved.

#include "Shipyard/Filter/ElevationFilter.h"

ElevationFilter::ElevationFilter(const INotifyFieldValueChanged::FFieldValueChangedDelegate& delegate)
    : IPartFilter(3, delegate)
{
	InitializeOptions(ElevationFilterOptions);
}

bool ElevationFilter::IsAllowed(const FName& option, const TObjectPtr<UVMShipPart>& part) const
{
	if (option == ElevationFilterOptions[1])
	{
		return part->GetElevation() == 0;
	}
	if (option == ElevationFilterOptions[2])
	{
		return part->GetElevation() == 1;
	}
	if (option == ElevationFilterOptions[3])
	{
		return part->GetElevation() == 2;
	}
	return true;
}
