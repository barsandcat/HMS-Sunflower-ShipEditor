// Copyright (c) 2026, sillygilly. All rights reserved.

#include "Shipyard/ShipDeviceSector.h"

namespace
{

constexpr float kAngleTolerance = 1e-3f;

float ClampWidthDegrees(float width)
{
	if (!FMath::IsFinite(width))
	{
		return 0.0f;
	}
	return FMath::Clamp(width, 0.0f, DeviceSectorMath::kFullCircleDegrees);
}

bool IsNormalizedSector(const FDeviceSector& sector)
{
	return FMath::IsFinite(sector.Rotation) && FMath::IsFinite(sector.Width) && sector.Rotation >= -kAngleTolerance && sector.Rotation < DeviceSectorMath::kFullCircleDegrees + kAngleTolerance && sector.Width >= -kAngleTolerance && sector.Width <= DeviceSectorMath::kFullCircleDegrees + kAngleTolerance;
}

}    // namespace

FDeviceSector FindAvailableSector(const FIntVector2& device_center, const FIntVector2& cabin_position)
{
	const FVector2D device_world(device_center.X, device_center.Y);
	const FVector2D cabin_world(cabin_position.X, cabin_position.Y);

	const FVector2D delta = device_world - cabin_world;
	if (FMath::Abs(delta.X) <= 1.0f && FMath::Abs(delta.Y) <= 1.0f)
	{
		return {};
	}

	TArray<float> angles;
	angles.Reserve(4);

	const FVector2D corners[4] = {
	    cabin_world + FVector2D(-1.0f, -1.0f),
	    cabin_world + FVector2D(-1.0f, 1.0f),
	    cabin_world + FVector2D(1.0f, -1.0f),
	    cabin_world + FVector2D(1.0f, 1.0f),
	};

	for (const FVector2D& corner : corners)
	{
		const FVector2D to_corner = corner - device_world;
		const float angle = FMath::RadiansToDegrees(FMath::Atan2(to_corner.Y, to_corner.X));
		angles.Add(DeviceSectorMath::NormalizeAngleDegrees(angle));
	}

	angles.Sort();

	// Max gap between angles will be an unblocked sector, i.e. sector that does not include cabin
	float max_gap = -1.0f;
	float start_of_max_gap = 0.0f;
	for (int32 i = 0; i < angles.Num(); i++)
	{
		int32 next_index = (i + 1) % angles.Num();
		float next = angles[next_index];
		if (next_index == 0)
		{
			next += DeviceSectorMath::kFullCircleDegrees;
		}
		const float current = angles[i];
		const float gap = next - current;
		if (gap > max_gap)
		{
			max_gap = gap;
			start_of_max_gap = current;
		}
	}

	const float rotation = DeviceSectorMath::NormalizeAngleDegrees(start_of_max_gap + max_gap * 0.5f);

	return {rotation, max_gap};
}

bool DoSectorsOverlap(const FDeviceSector& a, const FDeviceSector& b)
{
	checkf(IsNormalizedSector(a), TEXT("Expected normalized device sector A (Rotation in [0, 360), Width in [0, 360])."));
	checkf(IsNormalizedSector(b), TEXT("Expected normalized device sector B (Rotation in [0, 360), Width in [0, 360])."));

	if (a.Width >= DeviceSectorMath::kFullCircleDegrees - kAngleTolerance || b.Width >= DeviceSectorMath::kFullCircleDegrees - kAngleTolerance)
	{
		return true;
	}

	const float half_sum = (a.Width + b.Width) * 0.5f;
	const float distance = FMath::Abs(DeviceSectorMath::DeltaAngleDegrees(a.Rotation, b.Rotation));
	return distance <= half_sum + kAngleTolerance;
}

FDeviceSector CombineSectors(const FDeviceSector& a, const FDeviceSector& b)
{
	if (!DoSectorsOverlap(a, b))
	{
		return {};
	}

	if (a.Width >= DeviceSectorMath::kFullCircleDegrees - kAngleTolerance || b.Width >= DeviceSectorMath::kFullCircleDegrees - kAngleTolerance)
	{
		return {0.0f, DeviceSectorMath::kFullCircleDegrees};
	}

	// Make sure diff between rotations is less than 180 degrees
	// i.e. 350 and 10 are just 20 degrees apart, not 340 degrees
	float rotation_b = b.Rotation;
	if (rotation_b - a.Rotation > DeviceSectorMath::kHalfCircleDegrees)
	{
		rotation_b -= DeviceSectorMath::kFullCircleDegrees;
	}
	else if (rotation_b - a.Rotation < -DeviceSectorMath::kHalfCircleDegrees)
	{
		rotation_b += DeviceSectorMath::kFullCircleDegrees;
	}

	const float start_a = a.Rotation - a.Width * 0.5f;
	const float end_a = a.Rotation + a.Width * 0.5f;
	const float start_b = rotation_b - b.Width * 0.5f;
	const float end_b = rotation_b + b.Width * 0.5f;

	const float union_start = FMath::Min(start_a, start_b);
	const float union_end = FMath::Max(end_a, end_b);
	const float union_width = union_end - union_start;

	if (union_width >= DeviceSectorMath::kFullCircleDegrees - kAngleTolerance)
	{
		return {0.0f, DeviceSectorMath::kFullCircleDegrees};
	}

	const float union_rotation = DeviceSectorMath::NormalizeAngleDegrees(union_start + union_width * 0.5f);
	return {union_rotation, ClampWidthDegrees(union_width)};
}

FDeviceSector FindCommonSector(const FDeviceSector& a, const FDeviceSector& b)
{
	checkf(IsNormalizedSector(a), TEXT("Expected normalized device sector A (Rotation in [0, 360), Width in [0, 360])."));
	checkf(IsNormalizedSector(b), TEXT("Expected normalized device sector B (Rotation in [0, 360), Width in [0, 360])."));

	if (!DoSectorsOverlap(a, b))
	{
		return {};
	}

	if (a.Width >= DeviceSectorMath::kFullCircleDegrees - kAngleTolerance)
	{
		return b;
	}
	if (b.Width >= DeviceSectorMath::kFullCircleDegrees - kAngleTolerance)
	{
		return a;
	}

	float rotation_b = b.Rotation;
	if (rotation_b - a.Rotation > DeviceSectorMath::kHalfCircleDegrees)
	{
		rotation_b -= DeviceSectorMath::kFullCircleDegrees;
	}
	else if (rotation_b - a.Rotation < -DeviceSectorMath::kHalfCircleDegrees)
	{
		rotation_b += DeviceSectorMath::kFullCircleDegrees;
	}

	const float start_a = a.Rotation - a.Width * 0.5f;
	const float end_a = a.Rotation + a.Width * 0.5f;
	const float start_b = rotation_b - b.Width * 0.5f;
	const float end_b = rotation_b + b.Width * 0.5f;

	const float common_start = FMath::Max(start_a, start_b);
	const float common_end = FMath::Min(end_a, end_b);
	const float common_width = common_end - common_start;

	if (common_width <= kAngleTolerance)
	{
		return {};
	}

	const float common_rotation = DeviceSectorMath::NormalizeAngleDegrees(common_start + common_width * 0.5f);
	return {common_rotation, ClampWidthDegrees(common_width)};
}
