#include "Shipyard/IPartFilter.h"

IPartFilter::IPartFilter(int32 id, const INotifyFieldValueChanged::FFieldValueChangedDelegate& delegate)
    : FilterId(id)
{
	VM = NewObject<UVMShipPartFilter>();
	VM->AddFieldValueChangedDelegate(UVMShipPartFilter::FFieldNotificationClassDescriptor::Selected, delegate);
	VM->SetFilterId(id);
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
