// Copyright (c) 2025, sillygilly. All rights reserved.

#include "UI/Widgets/WFilterEntry.h"

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
