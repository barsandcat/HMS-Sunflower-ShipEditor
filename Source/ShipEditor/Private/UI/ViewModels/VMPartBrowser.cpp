// Copyright (c) 2024, sillygilly. All rights reserved.

#include "UI/ViewModels/VMPartBrowser.h"

void UVMPartBrowser::SetCategoryId(int32 id)
{
	UE_MVVM_SET_PROPERTY_VALUE(CategoryId, id);
}

int32 UVMPartBrowser::GetCategoryId() const
{
	return CategoryId;
}

void UVMPartBrowser::SetCategoryList(TUVMShipPartCategoryArray& categoryies)
{
	UE_MVVM_SET_PROPERTY_VALUE(CategoryList, categoryies);
}

void UVMPartBrowser::SetPartList(TUVMShipPartArray& NewPartList)
{
	UE_MVVM_SET_PROPERTY_VALUE(PartList, NewPartList);
}
