// Copyright (c) 2025, sillygilly. All rights reserved.

#include "UI/Widgets/WFilterEntry.h"

bool UWFilterEntry::Initialize()
{
	const bool initialized = Super::Initialize();
	if (initialized)
	{
		if (Combobox)
		{
			Combobox->OnSelectionChanged.AddDynamic(this, &UWFilterEntry::HandleOnSelectionChanged);
		}
	}
	return initialized;
}

void UWFilterEntry::SetOptions(const TArray<FName>& options_array)
{
	if (!Combobox)
	{
		return;
	}
	check(options_array.Num() > 0);

	Combobox->ClearOptions();
	for (const FName& option : options_array)
	{
		Combobox->AddOption(option);
	}
	Combobox->SetSelectedOption(options_array[0]);
}

void UWFilterEntry::HandleOnSelectionChanged(FName selected_item, ESelectInfo::Type selection_type)
{
	SelectedOption = selected_item;
}
