// Copyright (c) 2026, sillygilly. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "ShipDeviceSector.generated.h"

USTRUCT(BlueprintType)
struct FDeviceSector
{
	GENERATED_BODY()

	// Rotation in radians. 0 points along +X, PI/2 along +Y.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Rotation = 0.0f;

	// Angular width in radians, clamped to [0, 2*PI].
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Width = 0.0f;

	FDeviceSector() = default;
	FDeviceSector(float rotation, float width)
	    : Rotation(rotation), Width(width)
	{
	}

	FDeviceSector& operator=(const FDeviceSector& other) = default;

	bool operator==(const FDeviceSector& other) const
	{
		return Rotation == other.Rotation && Width == other.Width;
	}

	bool operator!=(const FDeviceSector& other) const
	{
		return !(*this == other);
	}

	bool IsValid() const
	{
		return Width > 0.0f;
	}
};

namespace DeviceSectorMath
{

constexpr float kFullCircleRadians = 2.0f * PI;

FORCEINLINE float NormalizeAngleRadians(float angle)
{
	float normalized = FMath::Fmod(angle, kFullCircleRadians);
	if (normalized < 0.0f)
	{
		normalized += kFullCircleRadians;
	}
	return normalized;
}

FORCEINLINE float DeltaAngleRadians(float angle, float reference)
{
	float delta = NormalizeAngleRadians(angle - reference);
	if (delta > PI)
	{
		delta -= kFullCircleRadians;
	}
	return delta;
}

}    // namespace DeviceSectorMath

// Returns the angular sector (in radians) that is not obstructed by a cabin  around a device center.
FDeviceSector FindAvailableSector(const FIntVector2& device_center, const FIntVector2& cabin_position, float grid_size);

// Returns true if two sectors overlap (inclusive).
bool DoSectorsOverlap(const FDeviceSector& a, const FDeviceSector& b);

// Combines two overlapping sectors into one. Returns invalid sector if they do not overlap.
FDeviceSector CombineSectors(const FDeviceSector& a, const FDeviceSector& b);

// Returns the overlap of two sectors. Returns invalid sector if they do not overlap.
FDeviceSector FindCommonSector(const FDeviceSector& a, const FDeviceSector& b);
