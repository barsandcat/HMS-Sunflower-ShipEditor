// Copyright (c) 2025, sillygilly. All rights reserved.

#include "Shipyard/ShipDeviceSector.h"
#include "Shipyard/ShipStructure.h"
#include "Tests/TestHarnessAdapter.h"

namespace
{

constexpr float kUsageTol = 0.001f;

TSharedPtr<FShipStructureDevice> MakeDevice(FShipStructure& structure,
    EDeviceType type,
    const FIntVector3& pos,
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
	TSharedPtr<FShipStructureDevice> device =
	    MakeShared<FShipStructureDevice>(stats, FShipPartTransform({pos.X, pos.Y}, 0, false), sector_rotation, nullptr);
	structure.Devices.Add(device);
	return device;
}

void AddCell(FShipStructure& structure, const FIntVector3& pos, ECellType type, const TSharedPtr<FShipStructureDevice>& device)
{
	structure.Cells.Add(pos, MakeShared<FShipStructureCell>(type, device, nullptr));
}

TSharedPtr<FShipStructureDevice> AddBridgeRoot(FShipStructure& structure, const FIntVector3& pos)
{
	TSharedPtr<FShipStructureDevice> bridge = MakeDevice(structure, EDeviceType::BRIDGE, pos);
	bridge->RequiresPhoneConnection = true;
	structure.Root = pos;
	AddCell(structure, pos, ECellType::INTERSECTION_PHONE_LINE_ROOT, bridge);
	return bridge;
}

}    // namespace

TEST_CASE_NAMED(FShipStructureProcessTest, "ShipEditor::ShipStructure::Process", "[ShipEditor][ShipStructure]")
{
	SECTION("Decks connected to root become DECK_PHONE_LINE")
	{
		FShipStructure structure;
		auto bridge = AddBridgeRoot(structure, FIntVector3(1, 1, 0));
		AddCell(structure, FIntVector3(1, 2, 0), ECellType::DECK, bridge);

		structure.Process();

		TSharedPtr<FShipStructureCell> deck_cell = structure.Cells.FindRef(FIntVector3(1, 2, 0));
		CHECK(deck_cell);
		CHECK(deck_cell->CellType == ECellType::DECK_PHONE_LINE);
	}

	SECTION("Decks not connected to root stay DECK")
	{
		FShipStructure structure;
		auto bridge = AddBridgeRoot(structure, FIntVector3(1, 1, 0));

		AddCell(structure, FIntVector3(5, 4, 0), ECellType::DECK, bridge);

		structure.Process();

		TSharedPtr<FShipStructureCell> deck_cell = structure.Cells.FindRef(FIntVector3(5, 4, 0));
		CHECK(deck_cell);
		CHECK(deck_cell->CellType == ECellType::DECK);
	}

	SECTION("Intersection root on phone line can phone the bridge")
	{
		FShipStructure structure;
		AddBridgeRoot(structure, FIntVector3(1, 1, 0));

		TSharedPtr<FShipStructureDevice> phone_device = MakeDevice(structure, EDeviceType::ENGINE, FIntVector3(3, 1, 0));
		phone_device->RequiresPhoneConnection = true;

		AddCell(structure, FIntVector3(2, 1, 0), ECellType::DECK, phone_device);
		AddCell(structure, FIntVector3(3, 1, 0), ECellType::INTERSECTION_PHONE_LINE_ROOT, phone_device);

		structure.Process();

		CHECK(phone_device->CanPhoneTheBridge);
	}

	SECTION("Intersection root not on phone line cannot phone the bridge")
	{
		FShipStructure structure;
		AddBridgeRoot(structure, FIntVector3(1, 1, 0));

		TSharedPtr<FShipStructureDevice> phone_device = MakeDevice(structure, EDeviceType::ENGINE, FIntVector3(5, 5, 0));
		phone_device->RequiresPhoneConnection = true;

		AddCell(structure, FIntVector3(5, 5, 0), ECellType::INTERSECTION_PHONE_LINE_ROOT, phone_device);

		structure.Process();

		CHECK(!phone_device->CanPhoneTheBridge);
	}

	SECTION("Traversable cabins connected to root can reach the bridge")
	{
		FShipStructure structure;
		AddBridgeRoot(structure, FIntVector3(1, 1, 0));

		TSharedPtr<FShipStructureDevice> cabin_device = MakeDevice(structure, EDeviceType::QUARTERS, FIntVector3(0, 0, 0));

		AddCell(structure, FIntVector3(0, 0, 0), ECellType::CABIN, cabin_device);

		structure.Process();

		CHECK(cabin_device->CanReachTheBridge);
	}

	SECTION("CABIN_BLOCKED stops interior reach beyond it")
	{
		FShipStructure structure;
		AddBridgeRoot(structure, FIntVector3(1, 1, 0));

		TSharedPtr<FShipStructureDevice> front_device = MakeDevice(structure, EDeviceType::QUARTERS, FIntVector3(0, 0, 0));
		TSharedPtr<FShipStructureDevice> far_device = MakeDevice(structure, EDeviceType::QUARTERS, FIntVector3(4, 0, 0));

		AddCell(structure, FIntVector3(0, 0, 0), ECellType::CABIN, front_device);
		AddCell(structure, FIntVector3(2, 0, 0), ECellType::CABIN_BLOCKED, front_device);
		AddCell(structure, FIntVector3(4, 0, 0), ECellType::CABIN, far_device);

		structure.Process();

		CHECK(front_device->CanReachTheBridge);
		CHECK(!far_device->CanReachTheBridge);
	}

	SECTION("Armor is placed only between reachable cabin and empty exterior")
	{
		FShipStructure structure;
		auto bridge = AddBridgeRoot(structure, FIntVector3(1, 1, 0));

		AddCell(structure, FIntVector3(0, 0, 0), ECellType::CABIN, bridge);
		AddCell(structure, FIntVector3(2, 0, 0), ECellType::CABIN, bridge);

		structure.Process();

		CHECK(structure.GetCellType(FIntVector3(-1, 0, 0)) == ECellType::DECK_ARMOR);
		CHECK(!structure.Cells.Contains(FIntVector3(1, 0, 0)));
	}

	SECTION("Armor is not placed where holes are")
	{
		FShipStructure structure;
		auto bridge = AddBridgeRoot(structure, FIntVector3(1, 1, 0));

		AddCell(structure, FIntVector3(0, 0, 0), ECellType::CABIN, bridge);
		AddCell(structure, FIntVector3(1, 0, 0), ECellType::DECK_HOLE, bridge);

		structure.Process();

		CHECK(structure.GetCellType(FIntVector3(-1, 0, 0)) == ECellType::DECK_ARMOR);
		CHECK(structure.GetCellType(FIntVector3(1, 0, 0)) == ECellType::DECK_HOLE);
	}

	SECTION("Device sector is partially blocked by one cabin cell")
	{
		FShipStructure structure;
		AddBridgeRoot(structure, FIntVector3(10, 10, 0));

		TSharedPtr<FShipStructureDevice> gun_device =
		    MakeDevice(structure, EDeviceType::GUN, FIntVector3(0, 0, 0), 0.0f, 0.0f, 90.f, 45.f);

		AddCell(structure, FIntVector3(0, 0, 0), ECellType::CABIN_BLOCKED, gun_device);
		AddCell(structure, FIntVector3(2, 0, 0), ECellType::CABIN, gun_device);

		structure.Process();

		CHECK(gun_device->AvailableSector.IsValid());
		CHECK(IsSectorAngleNear(gun_device->AvailableSector.Rotation, 45.f + 45.f / 2.0f));
		CHECK(IsSectorAngleNear(gun_device->AvailableSector.Width, 45.f));
	}

	SECTION("Split device sector keeps the side closest to the device rotation")
	{
		FShipStructure structure;
		AddBridgeRoot(structure, FIntVector3(10, 10, 0));

		TSharedPtr<FShipStructureDevice> gun_device =
		    MakeDevice(structure, EDeviceType::GUN, FIntVector3(0, 0, 0), 0.0f, 0.0f, 270.f, 90.f + 45.f);

		AddCell(structure, FIntVector3(0, 0, 0), ECellType::CABIN_BLOCKED, gun_device);
		AddCell(structure, FIntVector3(0, 2, 0), ECellType::CABIN, gun_device);

		structure.Process();

		CHECK(gun_device->AvailableSector.IsValid());
		CHECK(IsSectorAngleNear(gun_device->AvailableSector.Rotation, 202.5f));
		CHECK(IsSectorAngleNear(gun_device->AvailableSector.Width, 90.f + 45.f));
	}

	SECTION("Disconnected corridor roots compute fuel usage separately")
	{
		FShipStructure structure;
		AddBridgeRoot(structure, FIntVector3(1, 1, 0));

		TSharedPtr<FShipStructureDevice> producer_a = MakeDevice(structure, EDeviceType::ENGINE, FIntVector3(0, 0, 0), -10.0f);
		TSharedPtr<FShipStructureDevice> consumer_a = MakeDevice(structure, EDeviceType::ENGINE, FIntVector3(0, 2, 0), 5.0f);
		TSharedPtr<FShipStructureDevice> ignored_a = MakeDevice(structure, EDeviceType::ENGINE, FIntVector3(0, 4, 0), 100.0f);
		TSharedPtr<FShipStructureDevice> producer_b = MakeDevice(structure, EDeviceType::ENGINE, FIntVector3(10, 0, 0), -4.0f);
		TSharedPtr<FShipStructureDevice> consumer_b = MakeDevice(structure, EDeviceType::ENGINE, FIntVector3(12, 0, 0), 8.0f);

		AddCell(structure, FIntVector3(0, 0, 0), ECellType::CABIN_TECHNICAL_CORRIDOR_ROOT, producer_a);
		AddCell(structure, FIntVector3(0, 2, 0), ECellType::CABIN_TECHNICAL_CORRIDOR_ROOT, consumer_a);
		AddCell(structure, FIntVector3(0, 4, 0), ECellType::CABIN_TECHNICAL_CORRIDOR, ignored_a);

		AddCell(structure, FIntVector3(10, 0, 0), ECellType::CABIN_TECHNICAL_CORRIDOR_ROOT, producer_b);
		AddCell(structure, FIntVector3(12, 0, 0), ECellType::CABIN_TECHNICAL_CORRIDOR_ROOT, consumer_b);

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
		AddBridgeRoot(structure, FIntVector3(1, 1, 0));

		TSharedPtr<FShipStructureDevice> producer_a = MakeDevice(structure, EDeviceType::ENGINE, FIntVector3(0, 0, 0), -10.0f);
		TSharedPtr<FShipStructureDevice> consumer_a = MakeDevice(structure, EDeviceType::ENGINE, FIntVector3(0, 2, 0), 5.0f);
		TSharedPtr<FShipStructureDevice> producer_b = MakeDevice(structure, EDeviceType::ENGINE, FIntVector3(4, 0, 0), -4.0f);
		TSharedPtr<FShipStructureDevice> consumer_b = MakeDevice(structure, EDeviceType::ENGINE, FIntVector3(4, 2, 0), 8.0f);
		producer_b->RequiresPhoneConnection = true;
		consumer_b->RequiresPhoneConnection = true;

		AddCell(structure, FIntVector3(0, 0, 0), ECellType::CABIN_TECHNICAL_CORRIDOR_ROOT, producer_a);
		AddCell(structure, FIntVector3(0, 2, 0), ECellType::CABIN_TECHNICAL_CORRIDOR_ROOT, consumer_a);
		AddCell(structure, FIntVector3(2, 0, 0), ECellType::CABIN_TECHNICAL_CORRIDOR, producer_a);

		AddCell(structure, FIntVector3(4, 0, 0), ECellType::CABIN_TECHNICAL_CORRIDOR_ROOT, producer_b);
		AddCell(structure, FIntVector3(4, 2, 0), ECellType::CABIN_TECHNICAL_CORRIDOR_ROOT, consumer_b);

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
			AddBridgeRoot(structure, FIntVector3(1, 1, 0));

			TSharedPtr<FShipStructureDevice> producer = MakeDevice(structure, EDeviceType::ENGINE, FIntVector3(0, 0, 0), -10.0f);
			TSharedPtr<FShipStructureDevice> consumer = MakeDevice(structure, EDeviceType::ENGINE, FIntVector3(0, 2, 0), 5.0f);

			AddCell(structure, FIntVector3(0, 0, 0), ECellType::CABIN_TECHNICAL_CORRIDOR_ROOT, producer);
			AddCell(structure, FIntVector3(0, 2, 0), ECellType::CABIN_TECHNICAL_CORRIDOR_ROOT, consumer);

			structure.Process();

			CHECK(FMath::IsNearlyEqual(consumer->Usage, 1.0f, kUsageTol));
			CHECK(FMath::IsNearlyEqual(producer->Usage, 0.5f, kUsageTol));
		}

		{
			FShipStructure structure;
			AddBridgeRoot(structure, FIntVector3(1, 1, 0));

			TSharedPtr<FShipStructureDevice> producer = MakeDevice(structure, EDeviceType::ENGINE, FIntVector3(0, 0, 0), -5.0f);
			TSharedPtr<FShipStructureDevice> consumer = MakeDevice(structure, EDeviceType::ENGINE, FIntVector3(0, 2, 0), 10.0f);

			AddCell(structure, FIntVector3(0, 0, 0), ECellType::CABIN_TECHNICAL_CORRIDOR_ROOT, producer);
			AddCell(structure, FIntVector3(0, 2, 0), ECellType::CABIN_TECHNICAL_CORRIDOR_ROOT, consumer);

			structure.Process();

			CHECK(FMath::IsNearlyEqual(consumer->Usage, 0.5f, kUsageTol));
			CHECK(FMath::IsNearlyEqual(producer->Usage, 1.0f, kUsageTol));
		}
	}
}
