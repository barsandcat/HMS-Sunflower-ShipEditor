// Copyright (c) 2025, sillygilly. All rights reserved.

#include "Shipyard/Filter/MountFilter.h"

MountFilter::MountFilter(const INotifyFieldValueChanged::FFieldValueChangedDelegate& delegate)
    : IPartFilter(1, delegate)
{
	InitializeOptions(MountFilterOptions);
}

bool MountFilter::IsAllowed(const FName& option, const TObjectPtr<UVMShipPart>& part) const
{
	if (option == MountFilterOptions[1])
	{
		return part->GetDynamicMount();
	}
	if (option == MountFilterOptions[2])
	{
		return !part->GetDynamicMount();
	}
	return true;
}
