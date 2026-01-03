// Copyright (c) 2025, sillygilly. All rights reserved.

#include "ShipData/ShipPartInstanceTransform.h"

void FShipPartInstanceTransform::RotateClockwise()
{
	ZRotation = (ZRotation + 1) % 4;
}

void FShipPartInstanceTransform::RotateCounterClockwise()
{
	ZRotation = (ZRotation - 1) % 4;
}

void FShipPartInstanceTransform::Flip()
{
	YRotation = !YRotation;
}

FIntVector2 FShipPartInstanceTransform::operator()(const FIntVector2& point) const
{
	FIntVector2 result = point;

	if (YRotation)
	{
		result.X = -result.X;
	}

	switch (ZRotation)
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

	return result + Position;
}

FShipPartInstanceTransform FShipPartInstanceTransform::operator()(const FShipPartInstanceTransform& t) const
{
	FShipPartInstanceTransform result;
	result.Position = (*this)(t.Position);
	result.YRotation = YRotation != t.YRotation;
	result.ZRotation = (t.ZRotation + ZRotation) % 4;
	return result;
}

FShipPartInstanceTransform FShipPartInstanceTransform::Inverse() const
{
	FShipPartInstanceTransform result;
	result.Position = FIntVector2::ZeroValue - Position;
	result.ZRotation = -ZRotation;
	result.YRotation = YRotation;
	return result;
}
