// Copyright (c) 2026, sillygilly. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "ShipDeviceSector.generated.h"

USTRUCT(BlueprintType)
struct FDeviceSector
{
	GENERATED_BODY()

	// Rotation in degrees. 0 points along +X, 90 along +Y.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Rotation = 0.0f;

	// Angular width in degrees, clamped to [0, 360].
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

	FString ToDebugString() const
	{
		return FString::Printf(TEXT("R:%.0f W:%.0f"), Rotation, Width);
	}
};

constexpr float kFullSectorDegrees = 360.0f;
constexpr float kHalfSectorDegrees = 180.0f;
constexpr float kSectorAngleTol = 0.05f;

FORCEINLINE float NormalizeSectorAngle(float angle)
{
	float normalized = FMath::Fmod(angle, kFullSectorDegrees);
	if (normalized < 0.0f)
	{
		normalized += kFullSectorDegrees;
	}
	return normalized;
}

FORCEINLINE float DeltaSectorAngle(float angle, float reference)
{
	float delta = NormalizeSectorAngle(angle - reference);
	if (delta > kHalfSectorDegrees)
	{
		delta -= kFullSectorDegrees;
	}
	return delta;
}

bool IsSectorAngleNear(float angle, float expected);

// Returns the angular sector (in degrees) that is not obstructed by a cabin around a device center.
FDeviceSector FindAvailableSector(const FIntVector2& device_center, const FIntVector2& cabin_position);

// Returns true if two sectors overlap (inclusive).
bool DoSectorsOverlap(const FDeviceSector& a, const FDeviceSector& b);

// Combines two overlapping sectors into one. Returns invalid sector if they do not overlap.
FDeviceSector CombineSectors(const FDeviceSector& a, const FDeviceSector& b);

// Returns the overlap of two sectors. Returns invalid sector if they do not overlap.
FDeviceSector FindCommonSector(const FDeviceSector& a, const FDeviceSector& b);
