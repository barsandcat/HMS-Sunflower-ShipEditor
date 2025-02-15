// Copyright (c) 2025, sillygilly. All rights reserved.

#include "UI/ViewModels/VMShipPartCategory.h"

void UVMShipPartCategory::SetName(const FText& name)
{
	UE_MVVM_SET_PROPERTY_VALUE(Name, name);
}

void UVMShipPartCategory::SetCategoryId(int32 category_id)
{
	UE_MVVM_SET_PROPERTY_VALUE(CategoryId, category_id);
}
