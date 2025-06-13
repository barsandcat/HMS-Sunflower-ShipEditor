// Copyright (c) 2025, sillygilly. All rights reserved.

#include "UI/ViewModels/VMShipPartFilterEntry.h"

void UVMShipPartFilterEntry::SetText(const FText& text)
{
	UE_MVVM_SET_PROPERTY_VALUE(Text, text);
}

FText UVMShipPartFilterEntry::GetText() const
{
	return Text;
}

void UVMShipPartFilterEntry::SetName(FName name)
{
	UE_MVVM_SET_PROPERTY_VALUE(Name, name);
}

FName UVMShipPartFilterEntry::GetName() const
{
	return Name;
}

void UVMShipPartFilterEntry::SetMatchingPartsCount(int32 count)
{
	UE_MVVM_SET_PROPERTY_VALUE(MatchingPartsCount, count);
}

int32 UVMShipPartFilterEntry::GetMatchingPartsCount() const
{
	return MatchingPartsCount;
}