// Copyright (c) 2025, sillygilly. All rights reserved.

#pragma once
#include "CoreMinimal.h"
#include "UI/ViewModels/VMShipPartFilter.h"
#include "UI/ViewModels/VMShipPart.h"

class IPartFilter
{
public:
	virtual ~IPartFilter() = default;
	IPartFilter(int32 id, const INotifyFieldValueChanged::FFieldValueChangedDelegate& delegate);
	virtual TObjectPtr<UVMShipPartFilter> GetVM() const { return VM; };
	bool IsAllowed(const TObjectPtr<UVMShipPart>& part);

	void UpdateMatchingPartsCount(TUVMShipPartArray& list);

protected:
	virtual bool IsAllowed(const FName& option, const TObjectPtr<UVMShipPart>& part) const = 0;
	void InitializeOptions(const TArray<FName>& options_array);
	TObjectPtr<UVMShipPartFilter> VM;
	const int32 FilterId = 0;
};