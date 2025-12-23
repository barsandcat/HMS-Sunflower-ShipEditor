// Copyright (c) 2025, sillygilly. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "ShipDeckData.generated.h"

USTRUCT(BlueprintType)
struct SHIPEDITOR_API FShipDeckData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIntVector2 Position;
};
