// Copyright (c) 2025, sillygilly. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "ShipPartInstanceTransform.generated.h"

USTRUCT()
struct FShipPartInstanceTransform
{
	GENERATED_BODY()

	UPROPERTY()
	FIntVector2 Position;

	UPROPERTY()
	int32 XRotation = 0;

	UPROPERTY()
	bool ZRotation = false;

	void RotateClockwise();

	void RotateCounterClockwise();

	void Flip();

	FIntVector2 Transofrm(const FIntVector2& point) const;
};
