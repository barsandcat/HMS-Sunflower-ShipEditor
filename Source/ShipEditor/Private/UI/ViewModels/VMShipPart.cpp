// Copyright (c) 2024, sillygilly. All rights reserved.

#include "UI/ViewModels/VMShipPart.h"

void UVMShipPart::Initialize(const FText& name, FName id, int32 category_id, int32 elevation, bool dynamic, bool load_bearing)
{
	SetName(name);
	SetPartId(id);
	SetCategoryId(category_id);
	SetElevation(elevation);
	SetDynamicMount(dynamic);
	SetLoadBearing(load_bearing);
}

void UVMShipPart::SetName(const FText& name)
{
	UE_MVVM_SET_PROPERTY_VALUE(Name, name);
}

FText UVMShipPart::GetName() const
{
	return Name;
}

void UVMShipPart::SetPartId(FName part_id)
{
	UE_MVVM_SET_PROPERTY_VALUE(PartId, part_id);
}

FName UVMShipPart::GetPartId() const
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

void UVMShipPart::SetElevation(int32 elevation)
{
	UE_MVVM_SET_PROPERTY_VALUE(Elevation, elevation);
}

int32 UVMShipPart::GetElevation() const
{
	return Elevation;
}

void UVMShipPart::SetDynamicMount(bool dynamic_mount)
{
	UE_MVVM_SET_PROPERTY_VALUE(DynamicMount, dynamic_mount);
}

bool UVMShipPart::GetDynamicMount() const
{
	return DynamicMount;
}

void UVMShipPart::SetLoadBearing(bool load_bearing)
{
	UE_MVVM_SET_PROPERTY_VALUE(LoadBearing, load_bearing);
}

bool UVMShipPart::GetLoadBearing() const
{
	return LoadBearing;
}
