// Copyright (c) 2025, sillygilly. All rights reserved.

#include "UI/ViewModels/VMShipPartCategory.h"

void UVMShipPartCategory::SetName(const FText& name)
{
	UE_MVVM_SET_PROPERTY_VALUE(Name, name);
}

const FText& UVMShipPartCategory::GetName() const
{
	return Name;
}

void UVMShipPartCategory::SetCategoryId(int32 category_id)
{
	UE_MVVM_SET_PROPERTY_VALUE(CategoryId, category_id);
}

int32 UVMShipPartCategory::GetCategoryId() const
{
	return CategoryId;
}

void UVMShipPartCategory::SetSelected(bool selected)
{
	UE_MVVM_SET_PROPERTY_VALUE(Selected, selected);
}

bool UVMShipPartCategory::GetSelected() const
{
	return Selected;
}
