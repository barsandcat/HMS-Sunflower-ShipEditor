// Copyright (c) 2025, sillygilly. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "IPartFilter.h"

class SHIPEDITOR_API ElevationFilter : public IPartFilter
{
public:
	ElevationFilter(const INotifyFieldValueChanged::FFieldValueChangedDelegate& delegate);
	virtual bool IsAllowed(const TObjectPtr<UVMShipPart>& part) override;
	virtual ~ElevationFilter() = default;

private:
	const TArray<FName> ElevationFilterOptions = {"All elevations", "Low elevation", "Medium elevation", "High elevation"};
};
