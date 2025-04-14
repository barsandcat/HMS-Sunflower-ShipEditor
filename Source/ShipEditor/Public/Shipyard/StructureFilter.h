// Copyright (c) 2025, sillygilly. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "IPartFilter.h"

class SHIPEDITOR_API StructureFilter : public IPartFilter
{
public:
	StructureFilter(const INotifyFieldValueChanged::FFieldValueChangedDelegate& delegate);
	virtual ~StructureFilter() = default;
protected:
	virtual bool IsAllowed(const FName& option, const TObjectPtr<UVMShipPart>& part) const override;

private:
	TArray<FName> StructureFilterOptions = {"All structures", "Loadbearing structures", "No loadbearing structures"};
};
