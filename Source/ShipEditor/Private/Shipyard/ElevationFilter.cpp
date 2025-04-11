// Copyright (c) 2025, sillygilly. All rights reserved.


#include "Shipyard/ElevationFilter.h"

ElevationFilter::ElevationFilter(const INotifyFieldValueChanged::FFieldValueChangedDelegate& delegate)
    : IPartFilter(3, delegate)
{
	VM->SetOptions(ElevationFilterOptions);
}

bool ElevationFilter::IsFiltered(const TObjectPtr<UVMShipPart>& part)
{
	if (VM->GetSelected() == ElevationFilterOptions[1])
	{
		return part->GetElevation() != 0;
	}
	if (VM->GetSelected() == ElevationFilterOptions[2])
	{
		return part->GetElevation() != 1;
	}
	if (VM->GetSelected() == ElevationFilterOptions[3])
	{
		return part->GetElevation() != 2;
	}
	return false;
}
