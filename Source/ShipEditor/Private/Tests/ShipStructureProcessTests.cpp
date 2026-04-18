// Copyright (c) 2025, sillygilly. All rights reserved.

#include "Shipyard/ShipDeviceSector.h"
#include "Shipyard/ShipStructure.h"
#include "Tests/TestHarnessAdapter.h"

namespace
{

constexpr float kAngleTol = 0.05f;
constexpr float kUsageTol = 0.001f;

bool IsAngleNear(float angle, float expected)
{
	return FMath::Abs(DeviceSectorMath::DeltaAngleDegrees(angle, expected)) <= kAngleTol;
}

TSharedPtr<FShipStructureDevice> MakeDevice(EDeviceType type,
    const FIntVector2& pos,
    float fuel = 0.0f,
    float ammo = 0.0f,
    float sector_width = 0.0f,
    float sector_rotation = 0.0f)
{
	FDeviceStats stats;
	stats.DeviceType = type;
	stats.FuelConsumption = fuel;
	stats.AmmoConsumption = ammo;
	stats.SectorWidth = sector_width;
	return MakeShared<FShipStructureDevice>(stats, FShipPartTransform(pos, 0, false), sector_rotation, nullptr);
}

void AddCell(FShipStructure& structure, const FIntVector2& pos, ECellType type, const TSharedPtr<FShipStructureDevice>& device)
{
	structure.Cells.Add(pos, MakeShared<FShipStructureCell>(type, device, nullptr));
}

TSharedPtr<FShipStructureDevice> AddBridgeRoot(FShipStructure& structure, const FIntVector2& pos)
{
	TSharedPtr<FShipStructureDevice> bridge = MakeDevice(EDeviceType::BRIDGE, pos);
	bridge->RequiresPhoneConnection = true;
	structure.Root = pos;
	structure.Devices.Add(bridge);
	AddCell(structure, pos, ECellType::INTERSECTION_PHONE_LINE_ROOT, bridge);
	return bridge;
}

}    // namespace

TEST_CASE_NAMED(FShipStructureProcessTest, "ShipEditor::ShipStructure::Process", "[ShipEditor][ShipStructure]")
{
	SECTION("Decks connected to root become DECK_PHONE_LINE")
	{
		FShipStructure structure;
		AddBridgeRoot(structure, FIntVector2(1, 1));

		TSharedPtr<FShipStructureDevice> deck_device = MakeDevice(EDeviceType::NONE, FIntVector2(1, 2));
		structure.Devices.Add(deck_device);
		AddCell(structure, FIntVector2(1, 2), ECellType::DECK, deck_device);

		structure.Process();

		TSharedPtr<FShipStructureCell> deck_cell = structure.Cells.FindRef(FIntVector2(1, 2));
		CHECK(deck_cell);
		CHECK(deck_cell->CellType == ECellType::DECK_PHONE_LINE);
	}

	SECTION("Decks not connected to root stay DECK")
	{
		FShipStructure structure;
		AddBridgeRoot(structure, FIntVector2(1, 1));

		TSharedPtr<FShipStructureDevice> deck_device = MakeDevice(EDeviceType::NONE, FIntVector2(5, 4));
		structure.Devices.Add(deck_device);
		AddCell(structure, FIntVector2(5, 4), ECellType::DECK, deck_device);

		structure.Process();

		TSharedPtr<FShipStructureCell> deck_cell = structure.Cells.FindRef(FIntVector2(5, 4));
		CHECK(deck_cell);
		CHECK(deck_cell->CellType == ECellType::DECK);
	}

	SECTION("Intersection root on phone line can phone the bridge")
	{
		FShipStructure structure;
		AddBridgeRoot(structure, FIntVector2(1, 1));

		TSharedPtr<FShipStructureDevice> phone_device = MakeDevice(EDeviceType::ENGINE, FIntVector2(3, 1));
		phone_device->RequiresPhoneConnection = true;
		structure.Devices.Add(phone_device);

		AddCell(structure, FIntVector2(2, 1), ECellType::DECK, phone_device);
		AddCell(structure, FIntVector2(3, 1), ECellType::INTERSECTION_PHONE_LINE_ROOT, phone_device);

		structure.Process();

		CHECK(phone_device->CanPhoneTheBridge);
	}

	SECTION("Intersection root not on phone line cannot phone the bridge")
	{
		FShipStructure structure;
		AddBridgeRoot(structure, FIntVector2(1, 1));

		TSharedPtr<FShipStructureDevice> phone_device = MakeDevice(EDeviceType::ENGINE, FIntVector2(5, 5));
		phone_device->RequiresPhoneConnection = true;
		structure.Devices.Add(phone_device);

		AddCell(structure, FIntVector2(5, 5), ECellType::INTERSECTION_PHONE_LINE_ROOT, phone_device);

		structure.Process();

		CHECK(!phone_device->CanPhoneTheBridge);
	}

	SECTION("Traversable cabins connected to root can reach the bridge")
	{
		FShipStructure structure;
		AddBridgeRoot(structure, FIntVector2(1, 1));

		TSharedPtr<FShipStructureDevice> cabin_device = MakeDevice(EDeviceType::QUARTERS, FIntVector2(0, 0));
		structure.Devices.Add(cabin_device);

		AddCell(structure, FIntVector2(0, 0), ECellType::CABIN, cabin_device);

		structure.Process();

		CHECK(cabin_device->CanReachTheBridge);
	}

	SECTION("CABIN_BLOCKED stops interior reach beyond it")
	{
		FShipStructure structure;
		AddBridgeRoot(structure, FIntVector2(1, 1));

		TSharedPtr<FShipStructureDevice> front_device = MakeDevice(EDeviceType::QUARTERS, FIntVector2(0, 0));
		TSharedPtr<FShipStructureDevice> blocked_device = MakeDevice(EDeviceType::QUARTERS, FIntVector2(2, 0));
		TSharedPtr<FShipStructureDevice> far_device = MakeDevice(EDeviceType::QUARTERS, FIntVector2(4, 0));
		structure.Devices.Add(front_device);
		structure.Devices.Add(blocked_device);
		structure.Devices.Add(far_device);

		AddCell(structure, FIntVector2(0, 0), ECellType::CABIN, front_device);
		AddCell(structure, FIntVector2(2, 0), ECellType::CABIN_BLOCKED, blocked_device);
		AddCell(structure, FIntVector2(4, 0), ECellType::CABIN, far_device);

		structure.Process();

		CHECK(front_device->CanReachTheBridge);
		CHECK(!far_device->CanReachTheBridge);
	}

	SECTION("Armor is placed only between reachable cabin and empty exterior")
	{
		FShipStructure structure;
		AddBridgeRoot(structure, FIntVector2(1, 1));

		TSharedPtr<FShipStructureDevice> left_device = MakeDevice(EDeviceType::QUARTERS, FIntVector2(0, 0));
		TSharedPtr<FShipStructureDevice> right_device = MakeDevice(EDeviceType::QUARTERS, FIntVector2(2, 0));
		structure.Devices.Add(left_device);
		structure.Devices.Add(right_device);

		AddCell(structure, FIntVector2(0, 0), ECellType::CABIN, left_device);
		AddCell(structure, FIntVector2(2, 0), ECellType::CABIN, right_device);

		structure.Process();

		TSharedPtr<FShipStructureCell> exterior_armor = structure.Cells.FindRef(FIntVector2(-1, 0));
		CHECK(exterior_armor);
		CHECK(exterior_armor->CellType == ECellType::DECK_ARMOR);
		CHECK(!structure.Cells.Contains(FIntVector2(1, 0)));
	}

	SECTION("Device sector is partially blocked by one cabin cell")
	{
		FShipStructure structure;
		AddBridgeRoot(structure, FIntVector2(10, 10));

		TSharedPtr<FShipStructureDevice> gun_device =
		    MakeDevice(EDeviceType::GUN, FIntVector2(0, 0), 0.0f, 0.0f, 90.f, 45.f);
		structure.Devices.Add(gun_device);

		AddCell(structure, FIntVector2(0, 0), ECellType::CABIN_BLOCKED, gun_device);
		AddCell(structure, FIntVector2(2, 0), ECellType::CABIN, gun_device);

		structure.Process();

		CHECK(gun_device->AvailableSector.IsValid());
		CHECK(IsAngleNear(gun_device->AvailableSector.Rotation, 45.f + 45.f / 2.0f));
		CHECK(FMath::IsNearlyEqual(gun_device->AvailableSector.Width, 45.f, kAngleTol));
	}

	SECTION("Split device sector keeps the side closest to the device rotation")
	{
		FShipStructure structure;
		AddBridgeRoot(structure, FIntVector2(10, 10));

		TSharedPtr<FShipStructureDevice> gun_device =
		    MakeDevice(EDeviceType::GUN, FIntVector2(0, 0), 0.0f, 0.0f, 270.f, 90.f + 45.f);
		structure.Devices.Add(gun_device);

		AddCell(structure, FIntVector2(0, 0), ECellType::CABIN_BLOCKED, gun_device);
		AddCell(structure, FIntVector2(0, 2), ECellType::CABIN, gun_device);

		structure.Process();

		CHECK(gun_device->AvailableSector.IsValid());
		CHECK(IsAngleNear(gun_device->AvailableSector.Rotation, 202.5f));
		CHECK(FMath::IsNearlyEqual(gun_device->AvailableSector.Width, 90.f + 45.f, kAngleTol));
	}

	SECTION("Disconnected corridor roots compute fuel usage separately")
	{
		FShipStructure structure;
		AddBridgeRoot(structure, FIntVector2(1, 1));

		TSharedPtr<FShipStructureDevice> producer_a = MakeDevice(EDeviceType::ENGINE, FIntVector2(0, 0), -10.0f);
		TSharedPtr<FShipStructureDevice> consumer_a = MakeDevice(EDeviceType::ENGINE, FIntVector2(0, 2), 5.0f);
		TSharedPtr<FShipStructureDevice> ignored_a = MakeDevice(EDeviceType::ENGINE, FIntVector2(0, 4), 100.0f);
		TSharedPtr<FShipStructureDevice> producer_b = MakeDevice(EDeviceType::ENGINE, FIntVector2(10, 0), -4.0f);
		TSharedPtr<FShipStructureDevice> consumer_b = MakeDevice(EDeviceType::ENGINE, FIntVector2(12, 0), 8.0f);
		structure.Devices.Add(producer_a);
		structure.Devices.Add(consumer_a);
		structure.Devices.Add(ignored_a);
		structure.Devices.Add(producer_b);
		structure.Devices.Add(consumer_b);

		AddCell(structure, FIntVector2(0, 0), ECellType::CABIN_TECHNICAL_CORRIDOR_ROOT, producer_a);
		AddCell(structure, FIntVector2(0, 2), ECellType::CABIN_TECHNICAL_CORRIDOR_ROOT, consumer_a);
		AddCell(structure, FIntVector2(0, 4), ECellType::CABIN_TECHNICAL_CORRIDOR, ignored_a);

		AddCell(structure, FIntVector2(10, 0), ECellType::CABIN_TECHNICAL_CORRIDOR_ROOT, producer_b);
		AddCell(structure, FIntVector2(12, 0), ECellType::CABIN_TECHNICAL_CORRIDOR_ROOT, consumer_b);

		structure.Process();

		CHECK(FMath::IsNearlyEqual(consumer_a->Usage, 1.0f, kUsageTol));
		CHECK(FMath::IsNearlyEqual(producer_a->Usage, 0.5f, kUsageTol));
		CHECK(FMath::IsNearlyEqual(ignored_a->Usage, 0.0f, kUsageTol));
		CHECK(FMath::IsNearlyEqual(consumer_b->Usage, 0.5f, kUsageTol));
		CHECK(FMath::IsNearlyEqual(producer_b->Usage, 1.0f, kUsageTol));
	}

	SECTION("Connected corridors with mixed IsPartOfTheShip split subnetworks")
	{
		FShipStructure structure;
		AddBridgeRoot(structure, FIntVector2(1, 1));

		TSharedPtr<FShipStructureDevice> producer_a = MakeDevice(EDeviceType::ENGINE, FIntVector2(0, 0), -10.0f);
		TSharedPtr<FShipStructureDevice> consumer_a = MakeDevice(EDeviceType::ENGINE, FIntVector2(0, 2), 5.0f);
		TSharedPtr<FShipStructureDevice> producer_b = MakeDevice(EDeviceType::ENGINE, FIntVector2(4, 0), -4.0f);
		TSharedPtr<FShipStructureDevice> consumer_b = MakeDevice(EDeviceType::ENGINE, FIntVector2(4, 2), 8.0f);
		producer_b->RequiresPhoneConnection = true;
		consumer_b->RequiresPhoneConnection = true;
		structure.Devices.Add(producer_a);
		structure.Devices.Add(consumer_a);
		structure.Devices.Add(producer_b);
		structure.Devices.Add(consumer_b);

		AddCell(structure, FIntVector2(0, 0), ECellType::CABIN_TECHNICAL_CORRIDOR_ROOT, producer_a);
		AddCell(structure, FIntVector2(0, 2), ECellType::CABIN_TECHNICAL_CORRIDOR_ROOT, consumer_a);
		AddCell(structure, FIntVector2(2, 0), ECellType::CABIN_TECHNICAL_CORRIDOR, producer_a);

		AddCell(structure, FIntVector2(4, 0), ECellType::CABIN_TECHNICAL_CORRIDOR_ROOT, producer_b);
		AddCell(structure, FIntVector2(4, 2), ECellType::CABIN_TECHNICAL_CORRIDOR_ROOT, consumer_b);

		structure.Process();

		CHECK(FMath::IsNearlyEqual(consumer_a->Usage, 1.0f, kUsageTol));
		CHECK(FMath::IsNearlyEqual(producer_a->Usage, 0.5f, kUsageTol));
		CHECK(FMath::IsNearlyEqual(consumer_b->Usage, 0.5f, kUsageTol));
		CHECK(FMath::IsNearlyEqual(producer_b->Usage, 1.0f, kUsageTol));
	}

	SECTION("Fuel usage fractions reflect production vs consumption balance")
	{
		{
			FShipStructure structure;
			AddBridgeRoot(structure, FIntVector2(1, 1));

			TSharedPtr<FShipStructureDevice> producer = MakeDevice(EDeviceType::ENGINE, FIntVector2(0, 0), -10.0f);
			TSharedPtr<FShipStructureDevice> consumer = MakeDevice(EDeviceType::ENGINE, FIntVector2(0, 2), 5.0f);
			structure.Devices.Add(producer);
			structure.Devices.Add(consumer);

			AddCell(structure, FIntVector2(0, 0), ECellType::CABIN_TECHNICAL_CORRIDOR_ROOT, producer);
			AddCell(structure, FIntVector2(0, 2), ECellType::CABIN_TECHNICAL_CORRIDOR_ROOT, consumer);

			structure.Process();

			CHECK(FMath::IsNearlyEqual(consumer->Usage, 1.0f, kUsageTol));
			CHECK(FMath::IsNearlyEqual(producer->Usage, 0.5f, kUsageTol));
		}

		{
			FShipStructure structure;
			AddBridgeRoot(structure, FIntVector2(1, 1));

			TSharedPtr<FShipStructureDevice> producer = MakeDevice(EDeviceType::ENGINE, FIntVector2(0, 0), -5.0f);
			TSharedPtr<FShipStructureDevice> consumer = MakeDevice(EDeviceType::ENGINE, FIntVector2(0, 2), 10.0f);
			structure.Devices.Add(producer);
			structure.Devices.Add(consumer);

			AddCell(structure, FIntVector2(0, 0), ECellType::CABIN_TECHNICAL_CORRIDOR_ROOT, producer);
			AddCell(structure, FIntVector2(0, 2), ECellType::CABIN_TECHNICAL_CORRIDOR_ROOT, consumer);

			structure.Process();

			CHECK(FMath::IsNearlyEqual(consumer->Usage, 0.5f, kUsageTol));
			CHECK(FMath::IsNearlyEqual(producer->Usage, 1.0f, kUsageTol));
		}
	}
}
