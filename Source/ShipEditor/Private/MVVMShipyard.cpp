// Copyright (c) 2024, sillygilly. All rights reserved.

#include "MVVMShipyard.h"

#include "ShipyardSubsystem.h"

inline void UMVVMShipyard::SetTest(float NewTest)
{
	UE_MVVM_SET_PROPERTY_VALUE(Test, NewTest);
}

inline void UMVVMShipyard::SetBrushId(int32 Id)
{
	UE_MVVM_SET_PROPERTY_VALUE(BrushId, Id);
}

inline int32 UMVVMShipyard::GetBrushId() const
{
	return BrushId;
}

void UMVVMShipyard::SetPartList(TArray<TObjectPtr<UPartObject>>& NewPartList)
{
	UE_MVVM_SET_PROPERTY_VALUE(PartList, NewPartList);
}

void UPartObject::SetName(const FString& NewName)
{
	UE_MVVM_SET_PROPERTY_VALUE(Name, NewName);
}

void UPartObject::SetPartId(int32 NewPartId)
{
	UE_MVVM_SET_PROPERTY_VALUE(PartId, NewPartId);
}
