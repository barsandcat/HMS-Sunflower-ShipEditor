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
	FIntVector2 Position;

	/* 0: up, 1 or -3: right, 2 or -2:down, 3 or -1:left*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 XRotation = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ZRotation = false;

	void RotateClockwise();

	void RotateCounterClockwise();

	void Flip();

	FIntVector2 operator()(const FIntVector2& point) const;

	FShipPartInstanceTransform operator()(const FShipPartInstanceTransform& point) const;
};