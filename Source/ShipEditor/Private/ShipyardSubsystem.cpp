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

	if (const UWorld* World = GetWorld())
	{
		if (const UGameInstance* GameInstance = World->GetGameInstance())
		{
			UMVVMViewModelCollectionObject* Collection = GameInstance->GetSubsystem<UMVVMGameSubsystem>()->GetViewModelCollection();
			if (Collection)
			{
				FMVVMViewModelContext Context;
				Context.ContextClass = UMVVMShipyard::StaticClass();
				Context.ContextName = "Shipyard";
				Collection->AddViewModelInstance(Context, MVVMShipyard);
			}
		}
	}
}

void UShipyardSubsystem::OnBrushIdChanged(UObject* ViewModel, UE::FieldNotification::FFieldId FieldId)
{
	UE_LOG(LogTemp, Warning, TEXT("OnBrushIdChanged %s %s %d"), *ViewModel->GetName(), *FieldId.GetName().ToString(), MVVMShipyard->GetBrushId());
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
