// Copyright (c) 2024, sillygilly. All rights reserved.

#include "UI/ViewModels/VMShipPart.h"

void UVMShipPart::SetName(const FText& name)
{
	UE_MVVM_SET_PROPERTY_VALUE(Name, name);
}

FText UVMShipPart::GetName() const
{
	return Name;
}

void UVMShipPart::SetPartId(int32 part_id)
{
	UE_MVVM_SET_PROPERTY_VALUE(PartId, part_id);
}

int32 UVMShipPart::GetPartId() const
{
	return PartId;
}

void UVMShipPart::SetCategoryId(int32 category_id)
{
	UE_MVVM_SET_PROPERTY_VALUE(CategoryId, category_id);
}

int32 UVMShipPart::GetCategoryId() const
{
	return CategoryId;
}
