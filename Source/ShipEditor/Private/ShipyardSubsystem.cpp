// Copyright (c) 2024, sillygilly. All rights reserved.

#include "ShipyardSubsystem.h"

#include "MVVMGameSubsystem.h"

const double GRID_SIZE = 100.0f;

namespace
{

void AddPart(TUVMShipPartArray& List, FString Name, int32 Id)
{
	TObjectPtr<UVMShipPart> Part = NewObject<UVMShipPart>();
	Part->SetName(Name);
	Part->SetPartId(Id);
	List.Add(Part);
}

void SetOverlayMaterial(AShipPlanCell* Cell, UMaterialInterface* Material)
{
	TInlineComponentArray<UStaticMeshComponent*> StaticMeshes;
	Cell->GetComponents(StaticMeshes);
	for (UStaticMeshComponent* StaticMesh : StaticMeshes)
	{
		StaticMesh->SetOverlayMaterial(Material);
	}
}

}    // namespace

void UShipyardSubsystem::SetCursorPosition(FVector WorldPosition)
{
	if (!CursorClassPtr)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	FVector CursorPos = WorldPosition;
	int32 X = round(WorldPosition.X / GRID_SIZE);
	int32 Z = round(WorldPosition.Z / GRID_SIZE);

	CursorPos.X = X * GRID_SIZE;
	CursorPos.Z = Z * GRID_SIZE;

	if (!Cursor)
	{
		Cursor = World->SpawnActor<AActor>(CursorClassPtr, CursorPos, FRotator::ZeroRotator, {});
	}
	else
	{
		Cursor->SetActorLocation(CursorPos);
	}
}

void UShipyardSubsystem::AddModelViewToGlobal(UMVVMViewModelBase* ViewModel, UClass* Class, const FName& Name)
{
	const UWorld* World = GetWorld();
	if (!World)
		return;
	const UGameInstance* GameInstance = World->GetGameInstance();
	if (!GameInstance)
		return;
	UMVVMViewModelCollectionObject* Collection = GameInstance->GetSubsystem<UMVVMGameSubsystem>()->GetViewModelCollection();
	if (!Collection)
		return;

	FMVVMViewModelContext Context;
	Context.ContextClass = Class;
	Context.ContextName = Name;
	Collection->AddViewModelInstance(Context, ViewModel);
}

UShipyardSubsystem::UShipyardSubsystem()
    : UGameInstanceSubsystem()
{
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> SelectionYellow(TEXT("/Game/Materials/M_Selection_Yellow.M_Selection_Yellow"));
	SelectionMaterial = SelectionYellow.Object;
}

void UShipyardSubsystem::Initialize(FSubsystemCollectionBase& SubsytemCollection)
{
	Super::Initialize(SubsytemCollection);
	SubsytemCollection.InitializeDependency(UMVVMGameSubsystem::StaticClass());

	VMPartBrowser = NewObject<UVMPartBrowser>();
	VMPartBrowser->AddFieldValueChangedDelegate(UVMPartBrowser::FFieldNotificationClassDescriptor::PartId,
	    INotifyFieldValueChanged::FFieldValueChangedDelegate::CreateUObject(this, &UShipyardSubsystem::OnBrushIdChanged));
	TUVMShipPartArray List;
	AddPart(List, "BL 4-inch Mk IX", 1);
	AddPart(List, "Vickers .50 cal", 2);
	AddPart(List, "Lewis .303 cal", 3);
	VMPartBrowser->SetPartList(List);

	VMBrush = NewObject<UVMBrush>();

	AddModelViewToGlobal(VMPartBrowser, UVMPartBrowser::StaticClass(), "VMPartBrowser");
	AddModelViewToGlobal(VMBrush, UVMBrush::StaticClass(), "VMBrush");

	CursorClassPtr = CursorClass.LoadSynchronous();

	PartClassPtr = StaticLoadClass(AShipPlanCell::StaticClass(), nullptr, TEXT("/Game/BP_ShipPart.BP_ShipPart_C"), nullptr, LOAD_None, nullptr);
}

void UShipyardSubsystem::OnBrushIdChanged(UObject* ViewModel, UE::FieldNotification::FFieldId FieldId)
{
	UE_LOG(LogTemp, Warning, TEXT("OnBrushIdChanged %s %s %d"), *ViewModel->GetName(), *FieldId.GetName().ToString(), VMPartBrowser->GetPartId());
	VMBrush->SetReady(VMPartBrowser->GetPartId() != 0);
	if (BrushId == 0 && VMPartBrowser->GetPartId() != 0)
	{
		OnBrushReady.Broadcast();
	}
	if (BrushId != 0 && VMPartBrowser->GetPartId() == 0)
	{
		OnBrushCleared.Broadcast();
	}
	BrushId = VMPartBrowser->GetPartId();
}

void UShipyardSubsystem::DoBrush()
{
	UE_LOG(LogTemp, Warning, TEXT("DoBrush %d"), BrushId);
	UWorld* World = GetWorld();
	if (!World || !Cursor || !BrushId || !PartClassPtr)
	{
		return;
	}

	FVector CursorPos = Cursor->GetActorLocation();
	FIntVector2 CellId = {int32(round(CursorPos.X / GRID_SIZE)), int32(round(CursorPos.Z / GRID_SIZE))};

	if (ShipPlan.Contains(CellId))
	{
		return;
	}
	TObjectPtr<AShipPlanCell> ShipPlanCell = World->SpawnActor<AShipPlanCell>(PartClassPtr, CursorPos, {}, {});
	if (!ShipPlanCell)
	{
		return;
	}
	ShipPlanCell->PartId = BrushId;

	ShipPlan.Add(CellId, ShipPlanCell);
	VMPartBrowser->SetPartId(0);
}

void UShipyardSubsystem::Select()
{
	UE_LOG(LogTemp, Warning, TEXT("Select %d"), BrushId);
	if (!Cursor || BrushId)
	{
		return;
	}

	FVector CursorPos = Cursor->GetActorLocation();
	FIntVector2 CellId = {int32(round(CursorPos.X / GRID_SIZE)), int32(round(CursorPos.Z / GRID_SIZE))};

	if (Selection)
	{
		SetOverlayMaterial(Selection, nullptr);
		Selection = nullptr;
	}

	if (ShipPlan.Contains(CellId))
	{
		Selection = *ShipPlan.Find(CellId);
		SetOverlayMaterial(Selection, SelectionMaterial);
	}
}

void UShipyardSubsystem::Delete()
{
	UE_LOG(LogTemp, Warning, TEXT("Delete"));
	if (!Selection)
	{
		return;
	}

	FVector SelectionPos = Selection->GetActorLocation();
	FIntVector2 CellId = {int32(round(SelectionPos.X / GRID_SIZE)), int32(round(SelectionPos.Z / GRID_SIZE))};
	ShipPlan.Remove(CellId);

	Selection->Destroy();
	Selection = nullptr;
}
