// Copyright (c) 2025, sillygilly. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "ShipCellData.generated.h"

UENUM(BlueprintType)
enum class ECellType : uint8
{
	NONE = 0,
	CABIN = 1,
	DECK = 2,
	ROOT = 3,
};

UENUM(BlueprintType)
enum class EDeckType : uint8
{
	NONE = 0,
	REGULAR = 1,
	STRUCTURAL = 2,
	ARMOR = 3,
};

USTRUCT(BlueprintType)
struct SHIPEDITOR_API FShipCellData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIntVector2 Position = FIntVector2::ZeroValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECellType CellType = ECellType::NONE;
};
