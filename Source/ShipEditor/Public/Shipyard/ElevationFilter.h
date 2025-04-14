// Copyright (c) 2025, sillygilly. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "IPartFilter.h"

class SHIPEDITOR_API ElevationFilter : public IPartFilter
{
public:
	ElevationFilter(const INotifyFieldValueChanged::FFieldValueChangedDelegate& delegate);
	virtual ~ElevationFilter() = default;

protected:
	virtual bool IsAllowed(const FName& option, const TObjectPtr<UVMShipPart>& part) const override;

private:
	const TArray<FName> ElevationFilterOptions = {"All elevations", "Low elevation", "Medium elevation", "High elevation"};
};
