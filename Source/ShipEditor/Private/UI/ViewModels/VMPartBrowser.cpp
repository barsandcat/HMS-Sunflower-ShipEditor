// Copyright (c) 2024, sillygilly. All rights reserved.

#include "UI/ViewModels/VMPartBrowser.h"

void UVMPartBrowser::SetCategoryList(TUVMShipPartCategoryArray& categories)
{
	UE_MVVM_SET_PROPERTY_VALUE(CategoryList, categories);
}

const TUVMShipPartCategoryArray& UVMPartBrowser::GetCategoryList() const
{
	return CategoryList;
}

void UVMPartBrowser::SetFilterList(TUVMShipPartFilterArray& filters)
{
	UE_MVVM_SET_PROPERTY_VALUE(FilterList, filters);
}

const TArray<TObjectPtr<UVMShipPartFilter>>& UVMPartBrowser::GetFilterList() const
{
	return FilterList;
}

void UVMPartBrowser::SetPartList(TUVMShipPartArray& part_list)
{
	UE_MVVM_SET_PROPERTY_VALUE(PartList, part_list);
}

const TUVMShipPartArray& UVMPartBrowser::GetPartList() const
{
	return PartList;
}
