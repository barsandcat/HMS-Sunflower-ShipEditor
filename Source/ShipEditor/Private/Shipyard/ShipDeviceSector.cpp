// Copyright (c) 2026, sillygilly. All rights reserved.

#include "Shipyard/ShipDeviceSector.h"

namespace
{

constexpr float kFullCircleRadians = DeviceSectorMath::kFullCircleRadians;
constexpr float kAngleTolerance = 1e-3f;

float ClampWidthRadians(float width)
{
	if (!FMath::IsFinite(width))
	{
		return 0.0f;
	}
	return FMath::Clamp(width, 0.0f, kFullCircleRadians);
}

bool IsNormalizedSector(const FDeviceSector& sector)
{
	return FMath::IsFinite(sector.Rotation) && FMath::IsFinite(sector.Width)
	    && sector.Rotation >= -kAngleTolerance
	    && sector.Rotation < kFullCircleRadians + kAngleTolerance
	    && sector.Width >= -kAngleTolerance
	    && sector.Width <= kFullCircleRadians + kAngleTolerance;
}

}    // namespace

FDeviceSector MakeBlockedDeviceSector(const FIntVector2& device_center, const FIntVector2& cabin_position, float grid_size)
{
	checkf(grid_size > 0.0f, TEXT("Expected positive grid size."));

	const FVector2D device_world(device_center.X * grid_size, device_center.Y * grid_size);
	const FVector2D cabin_world(cabin_position.X * grid_size, cabin_position.Y * grid_size);
	const float half_size = grid_size * 0.5f;

	const FVector2D delta = device_world - cabin_world;
	if (FMath::Abs(delta.X) <= half_size && FMath::Abs(delta.Y) <= half_size)
	{
		return {0.0f, kFullCircleRadians};
	}

	TArray<float> angles;
	angles.Reserve(4);

	const FVector2D corners[4] = {
	    cabin_world + FVector2D(-half_size, -half_size),
	    cabin_world + FVector2D(-half_size, half_size),
	    cabin_world + FVector2D(half_size, -half_size),
	    cabin_world + FVector2D(half_size, half_size),
	};

	for (const FVector2D& corner : corners)
	{
		const FVector2D to_corner = corner - device_world;
		const float angle = FMath::Atan2(to_corner.Y, to_corner.X);
		angles.Add(DeviceSectorMath::NormalizeAngleRadians(angle));
	}

	angles.Sort();

	// Max gap between angles will be an unblocked sector, i.e. sector that does not include cabin
	float max_gap = -1.0f;
	float end_of_max_gap = 0.0f;
	for (int32 i = 0; i < angles.Num(); i++)
	{
		int32 next_index = (i + 1) % angles.Num();
		float next = angles[next_index];
		if (next_index == 0)
		{
			next = next + kFullCircleRadians;
		}
		const float gap = next - angles[i];
		if (gap > max_gap)
		{
			max_gap = gap;
			end_of_max_gap = angles[next_index];
		}
	}

	// Since max_gap is the unblocked sector, invert it to get the blocked sector width
	const float width = ClampWidthRadians(kFullCircleRadians - max_gap);
	const float rotation = DeviceSectorMath::NormalizeAngleRadians(end_of_max_gap + width * 0.5f);

	return {rotation, width};
}

bool DoDeviceSectorsOverlap(const FDeviceSector& a, const FDeviceSector& b)
{
	checkf(IsNormalizedSector(a), TEXT("Expected normalized device sector A (Rotation in [0, 2*PI), Width in [0, 2*PI])."));
	checkf(IsNormalizedSector(b), TEXT("Expected normalized device sector B (Rotation in [0, 2*PI), Width in [0, 2*PI])."));

	if (a.Width >= kFullCircleRadians - kAngleTolerance || b.Width >= kFullCircleRadians - kAngleTolerance)
	{
		return true;
	}

	const float half_sum = (a.Width + b.Width) * 0.5f;
	const float distance = FMath::Abs(DeviceSectorMath::DeltaAngleRadians(a.Rotation, b.Rotation));
	return distance <= half_sum + kAngleTolerance;
}

FDeviceSector CombineDeviceSectors(const FDeviceSector& a, const FDeviceSector& b)
{
	if (!DoDeviceSectorsOverlap(a, b))
	{
		return {};
	}

	if (a.Width >= kFullCircleRadians - kAngleTolerance || b.Width >= kFullCircleRadians - kAngleTolerance)
	{
		return {0.0f, kFullCircleRadians};
	}

	// Make sure diff between rotations is less than 180 degrees
	// i.e. 350 and 10 are just 20 degrees apart, not 340 degrees
	float rotation_b = b.Rotation;
	if (rotation_b - a.Rotation > PI)
	{
		rotation_b -= kFullCircleRadians;
	}
	else if (rotation_b - a.Rotation < -PI)
	{
		rotation_b += kFullCircleRadians;
	}

	const float start_a = a.Rotation - a.Width * 0.5f;
	const float end_a = a.Rotation + a.Width * 0.5f;
	const float start_b = rotation_b - b.Width * 0.5f;
	const float end_b = rotation_b + b.Width * 0.5f;

	const float union_start = FMath::Min(start_a, start_b);
	const float union_end = FMath::Max(end_a, end_b);
	const float union_width = union_end - union_start;

	if (union_width >= kFullCircleRadians - kAngleTolerance)
	{
		return {0.0f, kFullCircleRadians};
	}

	const float union_rotation = DeviceSectorMath::NormalizeAngleRadians(union_start + union_width * 0.5f);
	return {union_rotation, ClampWidthRadians(union_width)};
}
