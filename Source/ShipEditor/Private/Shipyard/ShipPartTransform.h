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
		1: 90 counter clockwise,
		2: 180 counter clockwise,
		3: 270 counter clockwise,
		-1: 90 clockwise,
		-2: 180 clockwise,
		-3: 270 clockwise */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Rotation = 0;

	/* Rotation around Y axis, true = 180 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Mirror = false;

	FShipPartTransform() = default;
	~FShipPartTransform() = default;
	FShipPartTransform(const FIntVector2& position, int32 rotation, bool mirror);

	void RotateClockwise();

	void RotateCounterClockwise();

	void Flip();

	FIntVector2 operator()(const FIntVector2& point) const;

	FShipPartTransform operator()(const FShipPartTransform& point) const;
	FShipPartTransform Inverse() const;

	FRotator ToRotator() const;
};
