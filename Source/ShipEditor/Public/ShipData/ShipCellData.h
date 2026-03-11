// Copyright (c) 2025, sillygilly. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "ShipCellData.generated.h"

/*
 * Enum for defining different types of ship cells.
 * The values are set up to allow bitwise operations for checking multiple properties at once.
 * First bit - 1 for cabin
 * Second bit - 1 for deck
 * Third bit - 1 for the root
 * Every thing else to differentiate types of cabins and decks
 */

enum ECellTypeFlag : uint8
{
	TYPE_MASK = 0b000011,
	CABIN = 0b000001,
	DECK = 0b000010,
	INTERSECTION = 0b000011,
	ROOT = 0b000100,
	SUBTYPE_MASK = 0b111000,
	SUBTYPE1 = 0b001000,
	SUBTYPE2 = 0b010000,
	SUBTYPE3 = 0b011000,
	SUBTYPE4 = 0b100000,
};

UENUM(BlueprintType)
enum class ECellType : uint8
{
	NONE = 0b000000,
	CABIN = ECellTypeFlag::CABIN,
	CABIN_TECHNICAL_CORRIDOR = ECellTypeFlag::CABIN | ECellTypeFlag::SUBTYPE1,
	CABIN_TECHNICAL_CORRIDOR_ROOT = ECellTypeFlag::CABIN | ECellTypeFlag::SUBTYPE1 | ECellTypeFlag::ROOT,
	CABIN_BLOCKED = ECellTypeFlag::CABIN | ECellTypeFlag::SUBTYPE2,
	DECK = ECellTypeFlag::DECK,
	DECK_PHONE_LINE = ECellTypeFlag::DECK | ECellTypeFlag::SUBTYPE1,
	INTERSECTION_PHONE_LINE_ROOT = ECellTypeFlag::INTERSECTION | ECellTypeFlag::SUBTYPE1 | ECellTypeFlag::ROOT,
	DECK_ARMOR = ECellTypeFlag::DECK | ECellTypeFlag::SUBTYPE2,
};

FORCEINLINE bool HasCellTypeFlag(ECellType cell_type, ECellTypeFlag flag)
{
	return (static_cast<uint8>(cell_type) & static_cast<uint8>(flag)) != 0;
}

FORCEINLINE bool IsTypeCell(ECellType cell_type, ECellTypeFlag type)
{
	return (static_cast<uint8>(cell_type) & static_cast<uint8>(ECellTypeFlag::TYPE_MASK)) == type;
}

FORCEINLINE bool IsSubTypeCell(ECellType cell_type, ECellTypeFlag sub_type)
{
	return (static_cast<uint8>(cell_type) & static_cast<uint8>(ECellTypeFlag::SUBTYPE_MASK)) == sub_type;
}

FORCEINLINE bool IsCabinCell(ECellType cell_type)
{
	return IsTypeCell(cell_type, ECellTypeFlag::CABIN);
}

FORCEINLINE bool IsDeckCell(ECellType cell_type)
{
	return IsTypeCell(cell_type, ECellTypeFlag::DECK);
}

FORCEINLINE bool IsIntersectionCell(ECellType cell_type)
{
	return IsTypeCell(cell_type, ECellTypeFlag::INTERSECTION);
}

FORCEINLINE bool IsRootCell(ECellType cell_type)
{
	return HasCellTypeFlag(cell_type, ECellTypeFlag::ROOT);
}

FORCEINLINE bool IsTechnicalCorridorCell(ECellType cell_type)
{
	return IsCabinCell(cell_type) && IsSubTypeCell(cell_type, ECellTypeFlag::SUBTYPE1);
}

FORCEINLINE bool IsDeckPhoneLineCell(ECellType cell_type)
{
	return IsDeckCell(cell_type) && IsSubTypeCell(cell_type, ECellTypeFlag::SUBTYPE1);
}

FORCEINLINE bool IsIntersectionRootCell(ECellType cell_type)
{
	return IsIntersectionCell(cell_type) && IsRootCell(cell_type);
}

FORCEINLINE bool IsCabinRootCell(ECellType cell_type)
{
	return IsCabinCell(cell_type) && IsRootCell(cell_type);
}

USTRUCT(BlueprintType)
struct SHIPEDITOR_API FShipCellData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIntVector2 Position = FIntVector2::ZeroValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECellType CellType = ECellType::NONE;
};
