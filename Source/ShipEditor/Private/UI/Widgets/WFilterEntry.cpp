// Copyright (c) 2025, sillygilly. All rights reserved.

#include "UI/Widgets/WFilterEntry.h"

bool UWFilterEntry::Initialize()
{
	const bool initialized = Super::Initialize();
	if (initialized)
	{
		Combobox->OnSelectionChanged.AddDynamic(this, &UWFilterEntry::HandleOnSelectionChanged);
	}
	return initialized;
}

void UWFilterEntry::SetOptions(const TArray<FName>& options_array)
{
	Combobox->ClearOptions();
	for (const FName& option : options_array)
	{
		Combobox->AddOption(option);
	}
}

void UWFilterEntry::HandleOnSelectionChanged(FName selected_item, ESelectInfo::Type selection_type)
{
	SelectedOption = selected_item;
}
