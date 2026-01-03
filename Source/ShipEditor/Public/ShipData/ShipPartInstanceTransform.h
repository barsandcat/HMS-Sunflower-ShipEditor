// Copyright (c) 2025, sillygilly. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "ShipPartInstanceTransform.generated.h"

USTRUCT(BlueprintType)
struct FShipPartInstanceTransform
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIntVector2 Position = FIntVector2::ZeroValue;

	/* 0: no,
	1 90 to the right,
	2 180 to the right,
	3 270 to the right,
	-1 90 to the left,
	-2, 180 to the left,
	-3 270 to the left */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 XRotation = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ZRotation = false;

	void RotateClockwise();

	void RotateCounterClockwise();

	void Flip();

	FIntVector2 operator()(const FIntVector2& point) const;

	FShipPartInstanceTransform operator()(const FShipPartInstanceTransform& point) const;
	FShipPartInstanceTransform Inverse() const;
};