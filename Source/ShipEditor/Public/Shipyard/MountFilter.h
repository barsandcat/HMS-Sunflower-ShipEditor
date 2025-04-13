// Copyright (c) 2025, sillygilly. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "IPartFilter.h"

class SHIPEDITOR_API MountFilter : public IPartFilter
{
public:
	MountFilter(const INotifyFieldValueChanged::FFieldValueChangedDelegate& delegate);
	virtual bool IsAllowed(const TObjectPtr<UVMShipPart>& part) override;
	~MountFilter() = default;

private:
	const TArray<FName> MountFilterOptions = {"All mounts", "Turret mount", "Static mount"};
};
