// Copyright (c) 2024, sillygilly. All rights reserved.

#include "ShipyardSubsystem.h"

#include "MVVMGameSubsystem.h"

namespace
{

void AddPart(TArray<TObjectPtr<UPartObject>>& List, FString Name, int32 Id)
{
	TObjectPtr<UPartObject> Part = NewObject<UPartObject>();
	Part->SetName(Name);
	Part->SetPartId(Id);
	List.Add(Part);
}

}    // namespace

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

void UShipyardSubsystem::Initialize(FSubsystemCollectionBase& SubsytemCollection)
{
	SubsytemCollection.InitializeDependency(UMVVMGameSubsystem::StaticClass());

	MVVMShipyard = NewObject<UMVVMShipyard>();
	MVVMShipyard->AddFieldValueChangedDelegate(UMVVMShipyard::FFieldNotificationClassDescriptor::BrushId,
	    INotifyFieldValueChanged::FFieldValueChangedDelegate::CreateUObject(this, &UShipyardSubsystem::OnBrushIdChanged));
	TArray<TObjectPtr<UPartObject>> List;
	AddPart(List, "BL 4-inch Mk IX", 1);
	AddPart(List, "Vickers .50 cal", 2);
	AddPart(List, "Lewis .303 cal", 3);
	MVVMShipyard->SetPartList(List);

	VMBrush = NewObject<UVMBrush>();

	AddModelViewToGlobal(MVVMShipyard, UMVVMShipyard::StaticClass(), "Shipyard");
	AddModelViewToGlobal(VMBrush, UVMBrush::StaticClass(), "Brush");
}

void UShipyardSubsystem::OnBrushIdChanged(UObject* ViewModel, UE::FieldNotification::FFieldId FieldId)
{
	UE_LOG(LogTemp, Warning, TEXT("OnBrushIdChanged %s %s %d"), *ViewModel->GetName(), *FieldId.GetName().ToString(), MVVMShipyard->GetBrushId());

	VMBrush->SetReady(MVVMShipyard->GetBrushId() != 0);
}

ETickableTickType UShipyardSubsystem::GetTickableTickType() const
{
	return HasAnyFlags(RF_ClassDefaultObject) ? ETickableTickType::Never : ETickableTickType::Always;
}

void UShipyardSubsystem::Tick(float DeltaTime)
{
	MVVMShipyard->SetTest(DeltaTime);
}

TStatId UShipyardSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UShipyardSubsystem, STATGROUP_Tickables);
}
