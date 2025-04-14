// Copyright (c) 2025, sillygilly. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "IPartFilter.h"

class SHIPEDITOR_API MountFilter : public IPartFilter
{
public:
	MountFilter(const INotifyFieldValueChanged::FFieldValueChangedDelegate& delegate);
	~MountFilter() = default;

protected:
	virtual bool IsAllowed(const FName& option, const TObjectPtr<UVMShipPart>& part) const override;

private:
	const TArray<FName> MountFilterOptions = {"All mounts", "Turret mount", "Static mount"};
};
