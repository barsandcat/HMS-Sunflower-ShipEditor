// Copyright (c) 2025, sillygilly. All rights reserved.


#include "Shipyard/MountFilter.h"

MountFilter::MountFilter(const INotifyFieldValueChanged::FFieldValueChangedDelegate& delegate)
    : IPartFilter(1, delegate)
{
	VM->SetOptions(MountFilterOptions);
}

bool MountFilter::IsFiltered(const TObjectPtr<UVMShipPart>& part)
{
	if (VM->GetSelected() == MountFilterOptions[1])
	{
		return !part->GetDynamicMount();
	}
	if (VM->GetSelected() == MountFilterOptions[2])
	{
		return part->GetDynamicMount();
	}
	return false;
}

