// Copyright (c) 2024, sillygilly. All rights reserved.

#include "ShipyardSubsystem.h"

#include "Engine/AssetManager.h"
#include "JsonObjectConverter.h"
#include "MVVMGameSubsystem.h"
#include "ShipData/ShipPartAsset.h"
#include "ShipData/ShipPlanData.h"
#include "ShipPlanRender.h"
#include "Shipyard/Filter/ElevationFilter.h"
#include "Shipyard/Filter/MountFilter.h"
#include "Shipyard/Filter/StructureFilter.h"
#include "Shipyard/ShipPartInstance.h"

namespace
{

const FName GrabBrushId = "GrabBrushId";

void AddPart(TUVMShipPartArray& list, const FText& name, FName id, int32 category_id, int32 elevation, bool dynamic, bool load_bearing)
{
	TObjectPtr<UVMShipPart> part = NewObject<UVMShipPart>();
	part->SetName(name);
	part->SetPartId(id);
	part->SetCategoryId(category_id);
	part->SetElevation(elevation);
	part->SetDynamicMount(dynamic);
	part->SetLoadBearing(load_bearing);
	list.Add(part);
}

}    // namespace

FIntVector2 UShipyardSubsystem::CursorPosToCellId(const FVector& world_pos)
{
	return {int32(round(world_pos.X / (GridSize * 2))) * 2, int32(round(world_pos.Z / (GridSize * 2))) * 2};
}

FVector UShipyardSubsystem::CellIdToCursorPos(const FIntVector2& cell_id)
{
	return {(float) (cell_id.X * GridSize), 0.0f, (float) (cell_id.Y * GridSize)};
}

void UShipyardSubsystem::AddCategory(TUVMShipPartCategoryArray& list, const FText& name, int32 id)
{
	TObjectPtr<UVMShipPartCategory> category = NewObject<UVMShipPartCategory>();

	category->AddFieldValueChangedDelegate(UVMShipPartCategory::FFieldNotificationClassDescriptor::Selected,
	    INotifyFieldValueChanged::FFieldValueChangedDelegate::CreateUObject(this, &UShipyardSubsystem::OnCategorySelected));

	category->SetName(name);
	category->SetCategoryId(id);
	list.Add(category);
}

void UShipyardSubsystem::OnCategorySelected(UObject* view_model, UE::FieldNotification::FFieldId field_id)
{
	UE_LOG(LogTemp, Warning, TEXT("OnCategorySelected %s %s"), *view_model->GetName(), *field_id.GetName().ToString());
	UpdatePartList();
}

void UShipyardSubsystem::OnFilterSelected(UObject* view_model, UE::FieldNotification::FFieldId field_id)
{
	UE_LOG(LogTemp, Warning, TEXT("OnFilterSelected %s %s"), *view_model->GetName(), *field_id.GetName().ToString());
	UpdatePartList();
}

void UShipyardSubsystem::SetCursorPosition(const TOptional<FVector>& world_position)
{
	if (!CursorClass)
	{
		return;
	}

	UWorld* world = GetWorld();
	if (!world)
	{
		return;
	}

	if (world_position)
	{
		FVector cursor_pos = CellIdToCursorPos(CursorPosToCellId(*world_position));
		cursor_pos.Y = world_position->Y;

		if (!Cursor)
		{
			Cursor = world->SpawnActor<AActor>(CursorClass, cursor_pos, FRotator::ZeroRotator, {});
		}
		else
		{
			Cursor->SetActorLocation(cursor_pos);
		}
	}
	else
	{
		if (Cursor)
		{
			Cursor->Destroy();
			Cursor = nullptr;
		}
	}
}

void UShipyardSubsystem::UpdateShipPlanAndPreviewMeshes()
{
	FShipRenderUpdate preview_update = PreviewRender->CreateRenderUpdate();
	FShipRenderUpdate ship_update = ShipPlanRender->CreateRenderUpdate();

	// Try to merge structure
	TMap<FIntVector2, FShipCellInstance> new_structure;
	TMap<FIntVector2, FShipCellInstance> ship_structure = ship_update.GetStructure();
	TMap<FIntVector2, FShipCellInstance> preview_structure = preview_update.GetStructure();
	if (MergeStructures(ship_structure, preview_structure, new_structure))
	{
		PreviewRender->SetOk(true);
		// ProcessStructure(new_structure);
		SetMeshes(new_structure);
	}
	else
	{
		PreviewRender->SetOk(false);
		SetMeshes(preview_structure);
		// ProcessStructure(ship_structure);
		SetMeshes(ship_structure);
	}
}

void UShipyardSubsystem::SetBrushPosition(const TOptional<FVector>& world_position)
{
	if (!world_position)
	{
		return;
	}
	if (!PreviewRender->SetPosition(CursorPosToCellId(*world_position)))
	{
		return;
	}

	// Update preview and ship meshses
	UpdateShipPlanAndPreviewMeshes();
}

UShipyardSubsystem::UShipyardSubsystem()
    : UWorldSubsystem()
{
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> selection_yellow(TEXT("/Game/Materials/M_Selection_Yellow.M_Selection_Yellow"));
	SelectionMaterial = selection_yellow.Object;
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> preview_blue(TEXT("/Game/Materials/M_Preview_Blue.M_Preview_Blue"));
	PreviewMaterial = preview_blue.Object;
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> preview_red(TEXT("/Game/Materials/M_Preview_Red.M_Preview_Red"));
	PreviewErrorMaterial = preview_red.Object;
}

void UShipyardSubsystem::Initialize(FSubsystemCollectionBase& collection)
{
	Super::Initialize(collection);

	LoadAllShipPartAssetsAsync();

	VMPartBrowser = NewObject<UVMPartBrowser>();
	VMPartBrowser->SetPartList(PartList);

	TUVMShipPartCategoryArray categories;
	AddCategory(categories, FText::FromString(TEXT("Guns")), 1);
	AddCategory(categories, FText::FromString(TEXT("Engines")), 2);
	AddCategory(categories, FText::FromString(TEXT("Fuel")), 3);
	AddCategory(categories, FText::FromString(TEXT("Power")), 4);
	AddCategory(categories, FText::FromString(TEXT("Magazines")), 5);
	AddCategory(categories, FText::FromString(TEXT("Quarters")), 6);
	VMPartBrowser->SetCategoryList(categories);

	FilterList.Add(MakeShared<MountFilter>(INotifyFieldValueChanged::FFieldValueChangedDelegate::CreateUObject(this, &UShipyardSubsystem::OnFilterSelected)));
	FilterList.Add(MakeShared<StructureFilter>(INotifyFieldValueChanged::FFieldValueChangedDelegate::CreateUObject(this, &UShipyardSubsystem::OnFilterSelected)));
	FilterList.Add(MakeShared<ElevationFilter>(INotifyFieldValueChanged::FFieldValueChangedDelegate::CreateUObject(this, &UShipyardSubsystem::OnFilterSelected)));

	TUVMShipPartFilterArray filters;
	for (const auto& filter : FilterList)
	{
		filters.Add(filter->GetVM());
	}
	VMPartBrowser->SetFilterList(filters);
	VMPartBrowser->SelectPart.AddDynamic(this, &UShipyardSubsystem::SetBrushId);

	VMBrush = NewObject<UVMBrush>();

	VMShipPlan = NewObject<UVMShipPlan>();
	VMShipPlan->SetName(FString("Flower"));
	VMShipPlan->SaveShipPlan.AddDynamic(this, &UShipyardSubsystem::SaveShipPlan);
	VMShipPlan->LoadShipPlan.AddDynamic(this, &UShipyardSubsystem::LoadShipPlan);

	CursorClass = SoftCursorClass.LoadSynchronous();
	ShipPlanRendererClass = SoftShipPlanRendererClass.LoadSynchronous();
	check(ShipPlanRendererClass);

	if (GetWorld() && GetWorld()->IsGameWorld())
	{
		FActorSpawnParameters spawn_params;
		spawn_params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		ShipPlanRender = GetWorld()->SpawnActor<AShipPlanRender>(ShipPlanRendererClass, FVector(0, 0, 0), FRotator::ZeroRotator, spawn_params);
		ShipPlanRender->Initialize(GridSize, nullptr, nullptr);
		PreviewRender = GetWorld()->SpawnActor<AShipPlanRender>(ShipPlanRendererClass, FVector(0, 500.f, 0), FRotator::ZeroRotator, spawn_params);
		PreviewRender->Initialize(GridSize, PreviewMaterial, PreviewErrorMaterial);
	}
}

void UShipyardSubsystem::LoadAllShipPartAssetsAsync()
{
	UAssetManager& asset_manager = UAssetManager::Get();

	ShipPartAssetIds.Reset();
	asset_manager.GetPrimaryAssetIdList(UShipPartAsset::StaticClass()->GetFName(), ShipPartAssetIds);

	if (ShipPartAssetIds.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No ShipPart assets found."));
		return;
	}

	asset_manager.LoadPrimaryAssets(ShipPartAssetIds, {}, FStreamableDelegate::CreateUObject(this, &UShipyardSubsystem::OnShipPartAssetsLoaded));
}

void UShipyardSubsystem::OnShipPartAssetsLoaded()
{
	UAssetManager& asset_manager = UAssetManager::Get();

	UE_LOG(LogTemp, Log, TEXT("All ShipPart assets loaded: %d"), ShipPartAssetIds.Num());

	for (const FPrimaryAssetId& id : ShipPartAssetIds)
	{
		UShipPartAsset* ship_part_asset = asset_manager.GetPrimaryAssetObject<UShipPartAsset>(id);
		if (ship_part_asset)
		{
			UE_LOG(LogTemp, Log, TEXT("Loaded ShipPart asset: %s"), *ship_part_asset->PartName.ToString());
			PartAssetMap.Add(ship_part_asset->PartId, ship_part_asset);
			TObjectPtr<UVMShipPart> part = NewObject<UVMShipPart>();
			part->Initialize(
			    ship_part_asset->PartName,
			    ship_part_asset->PartId,
			    ship_part_asset->CategoryId,
			    ship_part_asset->Elevation,
			    ship_part_asset->DynamicMount,
			    ship_part_asset->LoadBearing);
			PartList.Add(part);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to load ShipPart asset: %s"), *id.ToString());
		}
	}
}

void UShipyardSubsystem::DoBrush()
{
	UE_LOG(LogTemp, Warning, TEXT("DoBrush %s"), *BrushId.ToString());

	if (BrushId == NAME_None || !PreviewRender || !PreviewRender->IsOk())
	{
		return;
	}

	// Copy part instances
	ShipPlanRender->CopyParts(PreviewRender);

	// Update ship meshes
	{
		FShipRenderUpdate ship_update = ShipPlanRender->CreateRenderUpdate();
		TMap<FIntVector2, FShipCellInstance> new_structure = ship_update.GetStructure();
		// ProcessStructure(new_structure);
		SetMeshes(new_structure);
	}

	SetBrushId(NAME_None);
}

void UShipyardSubsystem::Select()
{
	UE_LOG(LogTemp, Warning, TEXT("Select %s"), *BrushId.ToString());
	if (!Cursor || BrushId != NAME_None || !ShipPlanRender)
	{
		return;
	}

	if (Selection)
	{
		ShipPlanRender->SetOverlayMaterial(Selection, nullptr);
		Selection = nullptr;
	}

	FIntVector2 cell_id = CursorPosToCellId(Cursor->GetActorLocation());
	Selection = ShipPlanRender->GetPartInstance(cell_id);
	if (Selection)
	{
		ShipPlanRender->SetOverlayMaterial(Selection, SelectionMaterial);
	}
}

void UShipyardSubsystem::Delete()
{
	UE_LOG(LogTemp, Warning, TEXT("Delete"));
	if (!Selection)
	{
		return;
	}
	ShipPlanRender->DeletePartInstance(Selection);
	Selection = nullptr;
}

void UShipyardSubsystem::Grab()
{
	UE_LOG(LogTemp, Warning, TEXT("Grab"));
	if (!Cursor || !ShipPlanRender)
	{
		return;
	}

	if (Selection)
	{
		ShipPlanRender->SetOverlayMaterial(Selection, nullptr);
		Selection = nullptr;
	}

	FIntVector2 cursor_cell_id = CursorPosToCellId(Cursor->GetActorLocation());
	UShipPartInstance* part_instance = ShipPlanRender->GetPartInstance(cursor_cell_id);
	if (part_instance)
	{
		SetBrushId(GrabBrushId);
		ShipPlanRender->DeletePartInstance(part_instance);
		FShipPartTransform preview_transform(cursor_cell_id, 0, false);
		FShipPartTransform part_transform = ShipPlanRender->GetPartTransform()(part_instance->Transform);

		PreviewRender->SetPartTransform(preview_transform);
		PreviewRender->SetPart(part_instance->PartAsset, preview_transform.Inverse()(part_transform));
		UpdateShipPlanAndPreviewMeshes();
	}
}

void UShipyardSubsystem::RotateBrushClockwise()
{
	UE_LOG(LogTemp, Warning, TEXT("RotateBrushClockwise"));
	if (PreviewRender)
	{
		PreviewRender->RotateClockwise();
		UpdateShipPlanAndPreviewMeshes();
	}
}

void UShipyardSubsystem::RotateBrushCounterClockwise()
{
	UE_LOG(LogTemp, Warning, TEXT("RotateBrushCounterClockwise"));
	if (PreviewRender)
	{
		PreviewRender->RotateCounterClockwise();
		UpdateShipPlanAndPreviewMeshes();
	}
}

void UShipyardSubsystem::FlipBrush()
{
	UE_LOG(LogTemp, Warning, TEXT("FlipBrush"));
	if (PreviewRender)
	{
		PreviewRender->Flip();
		UpdateShipPlanAndPreviewMeshes();
	}
}

void UShipyardSubsystem::SetBrushId(FName brush_id)
{
	UE_LOG(LogTemp, Warning, TEXT("SetBrushId %s"), *brush_id.ToString());
	VMBrush->SetReady(brush_id != NAME_None);

	if (brush_id == BrushId)
	{
		return;
	}

	if (BrushId != NAME_None)
	{
		check(PreviewRender);
		PreviewRender->Clear();

		if (brush_id == NAME_None)
		{
			OnBrushCleared.Broadcast();
		}
	}

	if (brush_id != NAME_None)
	{
		if (brush_id != GrabBrushId)
		{
			PreviewRender->SetPartTransform({CursorPosToCellId(Cursor->GetActorLocation()), 0, false});
			PreviewRender->SetPart(PartAssetMap.FindRef(brush_id), FShipPartTransform());
			UpdateShipPlanAndPreviewMeshes();
		}

		if (BrushId == NAME_None)
		{
			OnBrushReady.Broadcast();
		}
	}
	BrushId = brush_id;
}

void UShipyardSubsystem::SaveShipPlan(const FString& name)
{
	UE_LOG(LogTemp, Warning, TEXT("SaveShipPlan %s"), *name);
	FShipPlanData ship_plan_data;
	ship_plan_data.Name = "SuperFlower";
	FString file_content;

	if (!FJsonObjectConverter::UStructToJsonObjectString<FShipPlanData>(ship_plan_data, file_content, 0, 0))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed convert ship plan to %s"), *name);
		return;
	}

	if (!FFileHelper::SaveStringToFile(file_content, *name))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to save ship plan to %s"), *name);
		return;
	}

	VMShipPlan->SetName(name);
}

void UShipyardSubsystem::LoadShipPlan(const FString& name)
{
	UE_LOG(LogTemp, Warning, TEXT("LoadShipPlan %s"), *name);
	FString file_content;
	if (!FFileHelper::LoadFileToString(file_content, *name))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load ship plan from %s"), *name);
		return;
	}

	FShipPlanData ship_plan_data;
	FText reason;
	if (!FJsonObjectConverter::JsonObjectStringToUStruct<FShipPlanData>(file_content, &ship_plan_data, 0, 0, false, &reason))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load ship plan from %s. Reason: %s"), *name, *reason.ToString());
	}

	VMShipPlan->SetName(ship_plan_data.Name);
}

std::set<int32> UShipyardSubsystem::GetSelectedCategories() const
{
	std::set<int32> result;
	for (const auto& category_vm : VMPartBrowser->GetCategoryList())
	{
		if (category_vm->GetSelected())
		{
			result.insert(category_vm->GetCategoryId());
		}
	}
	return result;
}

bool UShipyardSubsystem::IsAllowedByFiters(const TObjectPtr<UVMShipPart>& part_vm) const
{
	for (const auto& filter : FilterList)
	{
		if (!filter->IsAllowed(part_vm))
		{
			return false;
		}
	}
	return true;
}

void UShipyardSubsystem::UpdatePartList()
{
	TUVMShipPartArray list;
	std::set<int32> categories = GetSelectedCategories();
	for (const auto& part_vm : PartList)
	{
		if (categories.empty() || categories.contains(part_vm->GetCategoryId()))
		{
			if (IsAllowedByFiters(part_vm))
			{
				list.Add(part_vm);
			}
		}
	}
	for (const auto& category_vm : VMPartBrowser->GetCategoryList())
	{
		int32 count = 0;
		for (const auto& part_vm : list)
		{
			if (part_vm->GetCategoryId() == category_vm->GetCategoryId())
			{
				count++;
			}
		}
		category_vm->SetMatchingPartsCount(count);
	}

	for (const auto& filter : FilterList)
	{
		filter->UpdateMatchingPartsCount(list);
	}

	VMPartBrowser->SetPartList(list);
}
