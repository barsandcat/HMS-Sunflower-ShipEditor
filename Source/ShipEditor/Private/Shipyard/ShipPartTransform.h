// Copyright (c) 2025, sillygilly. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "ShipPartTransform.generated.h"

USTRUCT(BlueprintType)
struct FShipPartTransform
{
	GENERATED_BODY()

	/* X to the right, Y up*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIntVector2 Position = FIntVector2::ZeroValue;

	/* Rotation around Z axis
	0: no rotation,
	1: 90 to the right,
	2: 180 to the right,
	3: 270 to the right,
	-1: 90 to the left,
	-2: 180 to the left,
	-3: 270 to the left */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ZRotation = 0;

	/* Rotation around Y axis, true = 180 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool YRotation = false;

	FShipPartTransform() = default;
	~FShipPartTransform() = default;
	FShipPartTransform(const FIntVector2& position, int32 z_rotation, bool y_rotation);

	void RotateClockwise();

	void RotateCounterClockwise();

	void Flip();

	FIntVector2 operator()(const FIntVector2& point) const;

	FShipPartTransform operator()(const FShipPartTransform& point) const;
	FShipPartTransform Inverse() const;
};