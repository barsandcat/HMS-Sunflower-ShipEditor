// Copyright (c) 2025, sillygilly. All rights reserved.

#pragma once
#include "CoreMinimal.h"
#include "UI/ViewModels/VMShipPartFilter.h"
#include "UI/ViewModels/VMShipPart.h"


class IPartFilter
{
public:
	virtual ~IPartFilter() = default;
	IPartFilter(int32 id, const INotifyFieldValueChanged::FFieldValueChangedDelegate& delegate)
	    : FilterId(id)
	{
		VM = NewObject<UVMShipPartFilter>();
		VM->AddFieldValueChangedDelegate(UVMShipPartFilter::FFieldNotificationClassDescriptor::Selected, delegate);
		VM->SetFilterId(id);
	}
	virtual TObjectPtr<UVMShipPartFilter> GetVM() const { return VM; };
	virtual bool IsAllowed(const TObjectPtr<UVMShipPart>& part) = 0;

protected:
	TObjectPtr<UVMShipPartFilter> VM;
	const int32 FilterId = 0;
};