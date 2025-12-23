// Copyright (c) 2025, sillygilly. All rights reserved.

#include "ShipData/ShipPartInstanceTransform.h"

inline void FShipPartInstanceTransform::RotateClockwise()
{
	XRotation = (XRotation + 1) % 4;
}

inline void FShipPartInstanceTransform::RotateCounterClockwise()
{
	XRotation = (XRotation - 1) % 4;
}

inline void FShipPartInstanceTransform::Flip()
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
			// rotate 90 degree
			result = FIntVector2(-point.Y, point.X);
			break;
		case -2:
		case 2:
			// rotate 180 degree
			result = FIntVector2(-point.X, -point.Y);
			break;
		case -1:
		case 3:
			// rotate 270 degree
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
