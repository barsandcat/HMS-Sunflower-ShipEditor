// Copyright (c) 2024, sillygilly. All rights reserved.

#include "UI/ViewModels/VMPartBrowser.h"

inline void UVMPartBrowser::SetPartId(int32 Id)
{
	UE_MVVM_SET_PROPERTY_VALUE(PartId, Id);
}

inline int32 UVMPartBrowser::GetPartId() const
{
	return PartId;
}

void UVMPartBrowser::SetPartList(TUVMShipPartArray& NewPartList)
{
	UE_MVVM_SET_PROPERTY_VALUE(PartList, NewPartList);
}
