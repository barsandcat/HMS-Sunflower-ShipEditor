// Copyright (c) 2026, sillygilly. All rights reserved.

#include "Shipyard/ShipDeviceSector.h"
#include "Tests/TestHarnessAdapter.h"

TEST_CASE_NAMED(FDeviceSectorTest, "ShipEditor::DeviceSector", "[ShipEditor][DeviceSector]")
{
	SECTION("Available sector for cabin to the right")
	{
		FDeviceSector sector = FindAvailableSector(FIntVector2(0, 0), FIntVector2(2, 0));

		CHECK(IsSectorAngleNear(sector.Rotation, 180));
		CHECK(IsSectorAngleNear(sector.Width, 270));
	}

	SECTION("Available sector for cabin above")
	{
		FDeviceSector sector = FindAvailableSector(FIntVector2(0, 0), FIntVector2(0, 2));

		CHECK(IsSectorAngleNear(sector.Rotation, 270));
		CHECK(IsSectorAngleNear(sector.Width, 270));
	}

	SECTION("Available sector symmetry")
	{
		FDeviceSector sector1 = FindAvailableSector(FIntVector2(0, 0), FIntVector2(2, 2));
		FDeviceSector sector2 = FindAvailableSector(FIntVector2(0, 0), FIntVector2(-2, 2));
		FDeviceSector sector3 = FindAvailableSector(FIntVector2(0, 0), FIntVector2(2, -2));
		FDeviceSector sector4 = FindAvailableSector(FIntVector2(0, 0), FIntVector2(-2, -2));

		CHECK(IsSectorAngleNear(sector1.Width, sector2.Width));
		CHECK(IsSectorAngleNear(sector1.Width, sector3.Width));
		CHECK(IsSectorAngleNear(sector1.Width, sector4.Width));
	}

	SECTION("Device inside cabin returns invalid sector")
	{
		FDeviceSector sector = FindAvailableSector(FIntVector2(0, 0), FIntVector2(0, 0));

		CHECK(!sector.IsValid());
	}

	SECTION("Overlap detection across wrap")
	{
		const FDeviceSector a(350, 30);    // 350deg center, 30deg width
		const FDeviceSector b(10, 30);     // 10deg center, 30deg width
		const FDeviceSector c(90, 20);     // 90deg center, 20deg width
		const FDeviceSector d(200, 30);    // 200deg center, 30deg width

		CHECK(DoSectorsOverlap(a, b));
		CHECK(!DoSectorsOverlap(c, d));
	}

	SECTION("Combine overlapping sectors")
	{
		const FDeviceSector a(350, 30);
		const FDeviceSector b(10, 30);

		FDeviceSector combined = CombineSectors(a, b);
		CHECK(combined.IsValid());
		CHECK(IsSectorAngleNear(combined.Rotation, 0.0f));
		CHECK(IsSectorAngleNear(combined.Width, 50));
	}

	SECTION("Combine two 180 degree sectors into full circle")
	{
		const FDeviceSector a(0.0f, 180);
		const FDeviceSector b(180, 180);

		FDeviceSector combined = CombineSectors(a, b);
		CHECK(combined.IsValid());
		CHECK(IsSectorAngleNear(combined.Rotation, 0.0f));
		CHECK(IsSectorAngleNear(combined.Width, 360));
	}

	SECTION("Combine fails for non-overlapping sectors")
	{
		const FDeviceSector a(90, 20);
		const FDeviceSector b(200, 30);

		FDeviceSector combined = CombineSectors(a, b);
		CHECK(!combined.IsValid());
	}

	SECTION("Find common sector for overlapping sectors")
	{
		const FDeviceSector a(0.0f, 30);
		const FDeviceSector b(10, 30);
		const FDeviceSector d(350, 30);

		FDeviceSector common_ab = FindCommonSector(a, b);
		FDeviceSector common_ad = FindCommonSector(a, d);
		CHECK(common_ab.IsValid());
		CHECK(common_ad.IsValid());
		CHECK(IsSectorAngleNear(common_ab.Rotation, 5));
		CHECK(IsSectorAngleNear(common_ad.Rotation, 355));
		CHECK(IsSectorAngleNear(common_ab.Width, common_ad.Width));
	}

	SECTION("Find common sector returns invalid when no overlap")
	{
		const FDeviceSector a(90, 20);
		const FDeviceSector b(200, 30);

		FDeviceSector common = FindCommonSector(a, b);
		CHECK(!common.IsValid());
	}

	SECTION("Find common sector with full circle")
	{
		const FDeviceSector a(0.0f, 360);
		const FDeviceSector b(10, 30);

		FDeviceSector common = FindCommonSector(a, b);
		CHECK(common.IsValid());
		CHECK(IsSectorAngleNear(common.Rotation, 10));
		CHECK(IsSectorAngleNear(common.Width, 30));
	}
}
