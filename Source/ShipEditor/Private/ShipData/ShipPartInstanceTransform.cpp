// Copyright (c) 2025, sillygilly. All rights reserved.

#include "ShipData/ShipPartInstanceTransform.h"

void FShipPartInstanceTransform::RotateClockwise()
{
	XRotation = (XRotation + 1) % 4;
}

void FShipPartInstanceTransform::RotateCounterClockwise()
{
	XRotation = (XRotation - 1) % 4;
}

void FShipPartInstanceTransform::Flip()
{
	ZRotation = !ZRotation;
}

FIntVector2 FShipPartInstanceTransform::operator()(const FIntVector2& point) const
{
	FIntVector2 result = point;

	switch (XRotation)
	{
		case -3:
		case 1:
			// rotate 90 degree clockwise or 270 degree counter clockwise
			result = FIntVector2(-point.Y, point.X);
			break;
		case -2:
		case 2:
			// rotate 180 degree clockwise or counter clockwise
			result = FIntVector2(-point.X, -point.Y);
			break;
		case -1:
		case 3:
			// rotate 270 degree clockwise or 90 degree counter clockwise
			result = FIntVector2(point.Y, -point.X);
			break;
		default:;
	}

	if (ZRotation)
	{
		result.Y = -result.Y;
	}

	return result + Position;
}

FShipPartInstanceTransform FShipPartInstanceTransform::operator()(const FShipPartInstanceTransform& t) const
{
	FShipPartInstanceTransform result;
	result.Position = (*this)(t.Position);
	result.ZRotation = ZRotation != t.ZRotation;
	result.XRotation = (t.XRotation + XRotation) % 4;
	return result;
}

FShipPartInstanceTransform FShipPartInstanceTransform::Inverse() const
{
	FShipPartInstanceTransform result;
	result.Position = FIntVector2::ZeroValue - Position;
	result.XRotation = -XRotation;
	result.ZRotation = ZRotation;
	return result;
}
