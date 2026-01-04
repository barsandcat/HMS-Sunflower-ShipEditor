#include "Shipyard/ShipPartTransform.h"
#include "Tests/TestHarnessAdapter.h"

TEST_CASE_NAMED(FPartTransformTest, "Editor::PartTransform", "[Editor][PartTransform]")
{
	SECTION("Identity")
	{
		FShipPartTransform transform;
		FIntVector2 point(2, 3);

		CHECK(transform(point) == point);
	}

	SECTION("Position offset")
	{
		FShipPartTransform transform;
		transform.Position = FIntVector2(5, -2);

		CHECK(transform(FIntVector2(1, 4)) == FIntVector2(6, 2));
	}

	SECTION("Rotate clockwise")
	{
		FShipPartTransform transform;
		transform.RotateClockwise();

		CHECK(transform(FIntVector2(2, 1)) == FIntVector2(1, -2));
	}

	SECTION("Rotate counter clockwise")
	{
		FShipPartTransform transform;
		transform.RotateCounterClockwise();

		CHECK(transform(FIntVector2(2, 1)) == FIntVector2(-1, 2));
	}

	SECTION("Flip (YRotation) without rotation")
	{
		FShipPartTransform transform;
		transform.Flip();

		CHECK(transform(FIntVector2(2, 3)) == FIntVector2(-2, 3));
	}

	SECTION("Flip with rotation")
	{
		FShipPartTransform transform;
		transform.RotateClockwise();
		transform.Flip();

		CHECK(transform(FIntVector2(2, 1)) == FIntVector2(-1, -2));
	}

	SECTION("Composition operator")
	{
		FShipPartTransform a;
		a.Position = FIntVector2(2, 4);
		a.ZRotation = 1;
		a.YRotation = true;

		FShipPartTransform b;
		b.Position = FIntVector2(1, 2);
		b.ZRotation = 2;
		b.YRotation = false;

		FShipPartTransform composed = a(b);

		CHECK(composed(FIntVector2(2, 1)) == FIntVector2(1, 5));
	}

	SECTION("Inverse 1")
	{
		FShipPartTransform transform;
		transform.Position = FIntVector2(4, 5);
		transform.ZRotation = 1;
		transform.YRotation = false;

		FShipPartTransform inv = transform.Inverse();

		CHECK(inv(transform(FIntVector2(2, 1))) == FIntVector2(2, 1));
	}

	SECTION("Inverse 2")
	{
		FShipPartTransform transform;
		transform.Position = FIntVector2(4, 5);
		transform.ZRotation = 2;
		transform.YRotation = true;

		FShipPartTransform inv = transform.Inverse();

		CHECK(inv(transform(FIntVector2(2, 1))) == FIntVector2(2, 1));
	}
}
