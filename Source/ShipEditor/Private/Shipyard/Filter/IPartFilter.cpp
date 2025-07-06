#include "Shipyard/Filter/IPartFilter.h"

IPartFilter::IPartFilter(int32 id, const INotifyFieldValueChanged::FFieldValueChangedDelegate& delegate)
    : FilterId(id)
{
	VM = NewObject<UVMShipPartFilter>();
	VM->AddFieldValueChangedDelegate(UVMShipPartFilter::FFieldNotificationClassDescriptor::Selected, delegate);
	VM->SetFilterId(id);
}

bool IPartFilter::IsAllowed(const TObjectPtr<UVMShipPart>& part)
{
	return IsAllowed(VM->GetSelected(), part);
}

void IPartFilter::InitializeOptions(const TArray<FName>& options_array)
{
	TUVMShipPartFilterEntryArray vm_entries;
	for (const FName& option : options_array)
	{
		TObjectPtr<UVMShipPartFilterEntry> vm_entry = NewObject<UVMShipPartFilterEntry>();
		vm_entry->SetName(option);
		vm_entry->SetText(FText::FromName(option));
		vm_entries.Add(vm_entry);
	}

	VM->SetVMEntries(vm_entries);
}

void IPartFilter::UpdateMatchingPartsCount(TUVMShipPartArray& list)
{
	for (const auto& entry : VM->GetVMEntries())
	{
		int32 count = 0;
		for (const auto& part : list)
		{
			if (IsAllowed(entry->GetName(), part))
			{
				count++;
			}
		}
		entry->SetMatchingPartsCount(count);
	}
}