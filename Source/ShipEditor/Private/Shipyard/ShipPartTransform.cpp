// Copyright (c) 2025, sillygilly. All rights reserved.

#include "Shipyard/ShipPartTransform.h"

namespace
{
FIntVector2 RotatePoint(const FIntVector2& point, int32 rotation)
{
	FIntVector2 result = point;
	switch (rotation)
	{
		case -3:
		case 1:
			// rotate 90 degree clockwise or 270 degree counter clockwise
			// 2,1  ->  1,-2
			result = FIntVector2(point.Y, -point.X);
			break;
		case -2:
		case 2:
			// rotate 180 degree clockwise or counter clockwise
			result = FIntVector2(-point.X, -point.Y);
			break;
		case -1:
		case 3:
			// rotate 270 degree clockwise or 90 degree counter clockwise
			result = FIntVector2(-point.Y, point.X);
			break;
		default:;
	}
	return result;
}

FIntVector2 MirrorPoint(const FIntVector2& point, bool mirror)
{
	FIntVector2 result = point;
	if (mirror)
	{
		result.X = -result.X;
	}
	return result;
}

int32 AddRotation(int32 rotation_a, int32 rotation_b, bool mirror)
{
	return (rotation_a + (mirror ? -rotation_b : rotation_b)) % 4;
}

}    // namespace

FShipPartTransform::FShipPartTransform(const FIntVector2& position, int32 rotation, bool mirror)
    : Position(position)
    , Rotation(rotation)
    , Mirror(mirror)
{
}

void FShipPartTransform::RotateClockwise()
{
	Rotation = AddRotation(Rotation, 1, Mirror);
}

void FShipPartTransform::RotateCounterClockwise()
{
	Rotation = AddRotation(Rotation, -1, Mirror);
}

void FShipPartTransform::Flip()
{
	Mirror = !Mirror;
}

FIntVector2 FShipPartTransform::operator()(const FIntVector2& point) const
{
	return MirrorPoint(RotatePoint(point, Rotation), Mirror) + Position;
}

FShipPartTransform FShipPartTransform::operator()(const FShipPartTransform& t) const
{
	return {(*this)(t.Position), AddRotation(t.Rotation, Rotation, t.Mirror), Mirror != t.Mirror};
}

FShipPartTransform FShipPartTransform::Inverse() const
{
	return {RotatePoint(MirrorPoint(FIntVector2::ZeroValue - Position, Mirror), -Rotation), Mirror ? Rotation : -Rotation, Mirror};
}

FRotator FShipPartTransform::ToRotator() const
{
	return FRotator(Rotation * (Mirror ? 90.0f : -90.0f) + (Mirror ? 180.0f : 0.0f), 0.0f, 0.0f);
}
