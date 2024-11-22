// Copyright (c) 2024, sillygilly. All rights reserved.

#include "UI/Widgets/ShipPartBrowserActivatableWidget.h"

bool UShipPartBrowserActivatableWidget::Initialize()
{
	if (Super::Initialize())
	{
		Dummy = NewObject<UDummy>();
		PartList->AddItem(Dummy);
		FilterList->AddItem(Dummy);
		CategoryList->AddItem(Dummy);
		return true;
	}
	return false;
}
