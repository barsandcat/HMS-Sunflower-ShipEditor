// Copyright (c) 2025, sillygilly. All rights reserved.

#include "Shipyard/ShipPartTransform.h"

namespace
{
FIntVector2 RotatePoint(const FIntVector2& point, int32 z_rotation, bool y_rotation)
{
	FIntVector2 result = point;
	switch (z_rotation)
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

	if (y_rotation)
	{
		result.X = -result.X;
	}

	return result;
}
int32 AddZRotation(int32 z_rotation_a, int32 z_rotation_b, bool y_rotation)
{
	return (z_rotation_a + (y_rotation ? -z_rotation_b : z_rotation_b)) % 4;
}

}    // namespace

FShipPartTransform::FShipPartTransform(const FIntVector2& position, int32 z_rotation, bool y_rotation)
    : Position(position)
    , ZRotation(z_rotation)
    , YRotation(y_rotation)
{
}

void FShipPartTransform::RotateClockwise()
{
	ZRotation = AddZRotation(ZRotation, 1, YRotation);
}

void FShipPartTransform::RotateCounterClockwise()
{
	ZRotation = AddZRotation(ZRotation, -1, YRotation);
}

void FShipPartTransform::Flip()
{
	YRotation = !YRotation;
}

FIntVector2 FShipPartTransform::operator()(const FIntVector2& point) const
{
	return RotatePoint(point, ZRotation, YRotation) + Position;
}

FShipPartTransform FShipPartTransform::operator()(const FShipPartTransform& t) const
{
	return {(*this)(t.Position), AddZRotation(t.ZRotation, ZRotation, t.YRotation), YRotation != t.YRotation};
}

FShipPartTransform FShipPartTransform::Inverse() const
{
	return {RotatePoint(FIntVector2::ZeroValue - Position, -ZRotation, YRotation), -ZRotation, YRotation};
}
