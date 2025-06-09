// Copyright (c) 2025, sillygilly. All rights reserved.

#include "UI/Widgets/WFilterEntry.h"
#include "UI/ViewModels/VMShipPartFilterEntry.h"

void UWFilterEntry::SetVMEntries(const TArray<UVMShipPartFilterEntry*>& entries)
{
	if (!Combobox)
	{
		return;
	}
	check(entries.Num() > 0);

	Combobox->ClearOptions();
	for (const UVMShipPartFilterEntry* entry : entries)
	{
		Combobox->AddOption(entry->GetName());
	}
	Combobox->SetSelectedOption(entries[0]->GetName());
}
