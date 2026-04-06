// Copyright (c) 2026, sillygilly. All rights reserved.

#include "Shipyard/ShipDeviceSector.h"
#include "Tests/TestHarnessAdapter.h"

namespace
{

constexpr float kAngleTol = 0.001f;
constexpr float kDeg5 = PI / 36.0f;
constexpr float kDeg10 = PI / 18.0f;
constexpr float kDeg20 = PI / 9.0f;
constexpr float kDeg30 = PI / 6.0f;
constexpr float kDeg90 = PI / 2.0f;
constexpr float kDeg180 = PI;
constexpr float kDeg200 = 10.0f * PI / 9.0f;
constexpr float kDeg350 = 35.0f * PI / 18.0f;

bool IsAngleNear(float angle, float expected)
{
	return FMath::Abs(DeviceSectorMath::DeltaAngleRadians(angle, expected)) <= kAngleTol;
}

}    // namespace

TEST_CASE_NAMED(FDeviceSectorTest, "ShipEditor::DeviceSector", "[ShipEditor][DeviceSector]")
{
	SECTION("Blocked sector for cabin to the right")
	{
		const float grid_size = 1.0f;
		FDeviceSector sector = MakeBlockedDeviceSector(FIntVector2(0, 0), FIntVector2(2, 0), grid_size);

		const float expected_width = 2.0f * FMath::Atan2(0.5f * grid_size, 1.5f * grid_size);
		CHECK(IsAngleNear(sector.Rotation, 0.0f));
		CHECK(FMath::IsNearlyEqual(sector.Width, expected_width, kAngleTol));
	}

	SECTION("Blocked sector for cabin above")
	{
		const float grid_size = 1.0f;
		FDeviceSector sector = MakeBlockedDeviceSector(FIntVector2(0, 0), FIntVector2(0, 2), grid_size);

		const float expected_width = 2.0f * FMath::Atan2(0.5f * grid_size, 1.5f * grid_size);
		CHECK(IsAngleNear(sector.Rotation, PI * 0.5f));
		CHECK(FMath::IsNearlyEqual(sector.Width, expected_width, kAngleTol));
	}

	SECTION("Device inside cabin blocks full circle")
	{
		const float grid_size = 2.0f;
		FDeviceSector sector = MakeBlockedDeviceSector(FIntVector2(0, 0), FIntVector2(0, 0), grid_size);

		CHECK(FMath::IsNearlyEqual(sector.Width, 2.0f * PI, kAngleTol));
	}

	SECTION("Overlap detection across wrap")
	{
		const FDeviceSector a(kDeg350, kDeg30);    // 350deg center, 30deg width
		const FDeviceSector b(kDeg10, kDeg30);     // 10deg center, 30deg width
		const FDeviceSector c(kDeg90, kDeg20);     // 90deg center, 20deg width
		const FDeviceSector d(kDeg200, kDeg30);    // 200deg center, 30deg width

		CHECK(DoDeviceSectorsOverlap(a, b));
		CHECK(!DoDeviceSectorsOverlap(c, d));
	}

	SECTION("Combine overlapping sectors")
	{
		const FDeviceSector a(kDeg350, kDeg30);
		const FDeviceSector b(kDeg10, kDeg30);

		FDeviceSector combined = CombineDeviceSectors(a, b);
		CHECK(combined.IsValid());
		CHECK(IsAngleNear(combined.Rotation, 0.0f));
		CHECK(FMath::IsNearlyEqual(combined.Width, 5.0f * kDeg10, kAngleTol));
	}

	SECTION("Combine two 180 degree sectors into full circle")
	{
		const FDeviceSector a(0.0f, kDeg180);
		const FDeviceSector b(kDeg180, kDeg180);

		FDeviceSector combined = CombineDeviceSectors(a, b);
		CHECK(combined.IsValid());
		CHECK(IsAngleNear(combined.Rotation, 0.0f));
		CHECK(FMath::IsNearlyEqual(combined.Width, 2.0f * kDeg180, kAngleTol));
	}

	SECTION("Combine fails for non-overlapping sectors")
	{
		const FDeviceSector a(kDeg90, kDeg20);
		const FDeviceSector b(kDeg200, kDeg30);

		FDeviceSector combined = CombineDeviceSectors(a, b);
		CHECK(!combined.IsValid());
	}

	SECTION("Find common sector for overlapping sectors")
	{
		const FDeviceSector a(0.0f, kDeg30);
		const FDeviceSector b(kDeg10, kDeg30);

		FDeviceSector common = FindCommonSector(a, b);
		CHECK(common.IsValid());
		CHECK(IsAngleNear(common.Rotation, kDeg5));
		CHECK(FMath::IsNearlyEqual(common.Width, kDeg20, kAngleTol));
	}

	SECTION("Find common sector returns invalid when no overlap")
	{
		const FDeviceSector a(kDeg90, kDeg20);
		const FDeviceSector b(kDeg200, kDeg30);

		FDeviceSector common = FindCommonSector(a, b);
		CHECK(!common.IsValid());
	}

	SECTION("Find common sector with full circle")
	{
		const FDeviceSector a(0.0f, 2.0f * kDeg180);
		const FDeviceSector b(kDeg10, kDeg30);

		FDeviceSector common = FindCommonSector(a, b);
		CHECK(common.IsValid());
		CHECK(IsAngleNear(common.Rotation, kDeg10));
		CHECK(FMath::IsNearlyEqual(common.Width, kDeg30, kAngleTol));
	}
}
