// Copyright Epic Games, Inc. All Rights Reserved.

#include "Messaging/CommonMessagingSubsystem.h"

#include "CommonLocalPlayer.h"
#include "Engine/GameInstance.h"
#include "Engine/LocalPlayer.h"
#include "Messaging/CommonGameDialog.h"
#include "NativeGameplayTags.h"
#include "PrimaryGameLayout.h"
#include "UObject/UObjectHash.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(CommonMessagingSubsystem)

class FSubsystemCollectionBase;
class UClass;

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_UI_LAYER_MODAL, "UI.Layer.Modal");

void UCommonMessagingSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UCommonMessagingSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

bool UCommonMessagingSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!CastChecked<ULocalPlayer>(Outer)->GetGameInstance()->IsDedicatedServerInstance())
	{
		TArray<UClass*> ChildClasses;
		GetDerivedClasses(GetClass(), ChildClasses, false);

		// Only create an instance if there is no override implementation defined elsewhere
		return ChildClasses.Num() == 0;
	}

	return false;
}

void UCommonMessagingSubsystem::ShowConfirmation(UCommonGameDialogDescriptor* DialogDescriptor, FCommonMessagingResultDelegate ResultCallback)
{
	if (UCommonLocalPlayer* LocalPlayer = GetLocalPlayer<UCommonLocalPlayer>())
	{
		if (UPrimaryGameLayout* RootLayout = LocalPlayer->GetRootUILayout())
		{
			RootLayout->PushWidgetToLayerStack<UCommonGameDialog>(TAG_UI_LAYER_MODAL, ConfirmationDialogClassPtr, [DialogDescriptor, ResultCallback](UCommonGameDialog& Dialog)
			    { Dialog.SetupDialog(DialogDescriptor, ResultCallback); });
		}
	}
}

void UCommonMessagingSubsystem::ShowError(UCommonGameDialogDescriptor* DialogDescriptor, FCommonMessagingResultDelegate ResultCallback)
{
	if (UCommonLocalPlayer* LocalPlayer = GetLocalPlayer<UCommonLocalPlayer>())
	{
		if (UPrimaryGameLayout* RootLayout = LocalPlayer->GetRootUILayout())
		{
			RootLayout->PushWidgetToLayerStack<UCommonGameDialog>(TAG_UI_LAYER_MODAL, ErrorDialogClassPtr, [DialogDescriptor, ResultCallback](UCommonGameDialog& Dialog)
			    { Dialog.SetupDialog(DialogDescriptor, ResultCallback); });
		}
	}
}
