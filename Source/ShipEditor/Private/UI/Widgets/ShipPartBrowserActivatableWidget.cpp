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
		AddOption(Catergories, "Gun");
		AddOption(Catergories, "Engine");
		AddOption(Catergories, "Fueltank");
		AddOption(Catergories, "Generator");
		AddOption(Catergories, "Quarters");
		AddOption(Catergories, "Landing gear");

		AddOption(Filters, "Any");
		AddOption(Filters, "Any");

		Dummy = NewObject<UListFilterOption>();
		// PartList->AddItem(Dummy);
		FilterList->SetListItems(Filters);
		CategoryList->SetListItems(Catergories);
		return true;
	}
	return false;
}
