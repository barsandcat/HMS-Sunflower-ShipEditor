// Copyright (c) 2026, sillygilly. All rights reserved.

#pragma once

#include "CoreMinimal.h"

struct FDeviceSector
{
	// Rotation in radians. 0 points along +X, PI/2 along +Y.
	float Rotation = 0.0f;

	// Angular width in radians, clamped to [0, 2*PI].
	float Width = 0.0f;

	FDeviceSector() = default;
	FDeviceSector(float rotation, float width)
	    : Rotation(rotation), Width(width)
	{
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

// Returns the angular sector (in radians) blocked by a cabin cell from a device center.
FDeviceSector MakeBlockedDeviceSector(const FIntVector2& device_center, const FIntVector2& cabin_position, float grid_size);

// Returns true if two sectors overlap (inclusive).
bool DoDeviceSectorsOverlap(const FDeviceSector& a, const FDeviceSector& b);

// Combines two overlapping sectors into one. Returns false if they do not overlap.
// Combines two overlapping sectors into one. Returns invalid sector if they do not overlap.
FDeviceSector CombineDeviceSectors(const FDeviceSector& a, const FDeviceSector& b);
