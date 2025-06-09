// Copyright (c) 2025, sillygilly. All rights reserved.

#include "UI/Widgets/WFilterEntry.h"
#include "UI/ViewModels/VMShipPartFilterEntry.h"

void UWFilterEntry::SetVMEntries(const TArray<UVMShipPartFilterEntry*>& entries)
{
	if (!Combobox)
	{
		return;
	}

	Combobox->ClearOptions();

	for (const UVMShipPartFilterEntry* entry : entries)
	{
		Combobox->AddOption(entry->GetName());
	}

	if (entries.IsEmpty())
	{
		return;
	}

	Combobox->SetSelectedOption(entries[0]->GetName());
}
