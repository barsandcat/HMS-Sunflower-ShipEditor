// Copyright (c) 2024, sillygilly. All rights reserved.

#include "UI/ViewModels/VMPartBrowser.h"

void UVMPartBrowser::SetCategoryList(TUVMShipPartCategoryArray& categoryies)
{
	UE_MVVM_SET_PROPERTY_VALUE(CategoryList, categoryies);
}

const TUVMShipPartCategoryArray& UVMPartBrowser::GetCategoryList() const
{
	return CategoryList;
}

void UVMPartBrowser::SetPartList(TUVMShipPartArray& NewPartList)
{
	UE_MVVM_SET_PROPERTY_VALUE(PartList, NewPartList);
}
