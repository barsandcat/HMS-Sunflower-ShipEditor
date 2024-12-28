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
