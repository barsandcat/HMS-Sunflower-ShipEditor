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
		FShipPartTransform transform({5, -2}, 0, false);

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

	SECTION("Composition operator 1")
	{
		FShipPartTransform p({0, 0}, 1, false);
		FShipPartTransform r({0, 0}, 1, false);
		FShipPartTransform composed = r(p);

		CHECK(composed(FIntVector2(2, 1)) == FIntVector2(-2, -1));
	}

	SECTION("Composition operator 2")
	{
		FShipPartTransform p({0, 0}, 1, false);
		FShipPartTransform r({0, 0}, 0, true);
		FShipPartTransform composed = r(p);

		CHECK(composed(FIntVector2(2, 1)) == FIntVector2(-1, -2));
	}

	SECTION("Composition operator 3")
	{
		FShipPartTransform p({0, 0}, 0, true);
		FShipPartTransform r({0, 0}, 1, false);
		FShipPartTransform composed = r(p);

		CHECK(composed(FIntVector2(2, 1)) == FIntVector2(1, 2));
	}

	SECTION("Composition operator 4")
	{
		FShipPartTransform p({0, 0}, 0, true);
		FShipPartTransform r({0, 0}, 1, true);
		FShipPartTransform composed = r(p);

		CHECK(composed(FIntVector2(2, 1)) == FIntVector2(-1, 2));
	}

	SECTION("Composition operator 5")
	{
		FShipPartTransform p({0, 0}, 1, true);
		FShipPartTransform r({0, 0}, 1, true);
		FShipPartTransform composed = r(p);

		CHECK(composed(FIntVector2(2, 1)) == FIntVector2(2, 1));
	}

	SECTION("Inverse 1")
	{
		FShipPartTransform transform({4, 5}, 1, false);
		FShipPartTransform inv = transform.Inverse();

		CHECK(inv(transform(FIntVector2(2, 1))) == FIntVector2(2, 1));
	}

	SECTION("Inverse 2")
	{
		FShipPartTransform transform({4, 5}, 2, true);
		FShipPartTransform inv = transform.Inverse();

		CHECK(inv(transform(FIntVector2(2, 1))) == FIntVector2(2, 1));
	}
}
