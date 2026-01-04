// Copyright (c) 2025, sillygilly. All rights reserved.

#include "Shipyard/ShipPartTransform.h"

namespace
{
FIntVector2 RotatePointAroundZ(const FIntVector2& point, int32 ZRotation)
{
	switch (ZRotation)
	{
		case -3:
		case 1:
			// rotate 90 degree clockwise or 270 degree counter clockwise
			// 2,1  ->  1,-2
			return FIntVector2(point.Y, -point.X);
		case -2:
		case 2:
			// rotate 180 degree clockwise or counter clockwise
			return FIntVector2(-point.X, -point.Y);
		case -1:
		case 3:
			// rotate 270 degree clockwise or 90 degree counter clockwise
			return FIntVector2(-point.Y, point.X);
		default:;
			return point;
	}
}
}    // namespace

void FShipPartTransform::RotateClockwise()
{
	ZRotation = (ZRotation + (YRotation ? -1 : 1)) % 4;
}

void FShipPartTransform::RotateCounterClockwise()
{
	ZRotation = (ZRotation - (YRotation ? -1 : 1)) % 4;
}

void FShipPartTransform::Flip()
{
	YRotation = !YRotation;
}

FIntVector2 FShipPartTransform::operator()(const FIntVector2& point) const
{
	FIntVector2 result = RotatePointAroundZ(point, ZRotation);

	if (YRotation)
	{
		result.X = -result.X;
	}

	return result + Position;
}

FShipPartTransform FShipPartTransform::operator()(const FShipPartTransform& t) const
{
	FShipPartTransform result;
	result.Position = (*this)(t.Position);
	result.YRotation = YRotation != t.YRotation;
	result.ZRotation = (t.ZRotation + ZRotation) % 4;
	return result;
}

FShipPartTransform FShipPartTransform::Inverse() const
{
	FShipPartTransform result;
	result.Position = RotatePointAroundZ(FIntVector2::ZeroValue - Position, -ZRotation);
	if (YRotation)
	{
		result.Position.X = -result.Position.X;
	}
	result.ZRotation = -ZRotation;
	result.YRotation = YRotation;
	return result;
}
