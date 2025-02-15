// Copyright (c) 2024, sillygilly. All rights reserved.

#include "UI/Widgets/ShipPartBrowserActivatableWidget.h"

namespace
{

void AddOption(TArray<TObjectPtr<UListFilterOption>>& List, FString OptionName)
{
	TObjectPtr<UListFilterOption> Option = NewObject<UListFilterOption>();
	Option->Name = OptionName;
	List.Add(Option);
}

}    // namespace
bool UShipPartBrowserActivatableWidget::Initialize()
{
	if (Super::Initialize())
	{
		AddOption(Filters, "Any");
		AddOption(Filters, "Any");

		FilterList->SetListItems(Filters);
		return true;
	}
	return false;
}
