// Copyright (c) 2024, sillygilly. All rights reserved.

#include "UI/MyCommonMessagingSubsystem.h"

#include "CommonLocalPlayer.h"
#include "Messaging/CommonGameDialog.h"
#include "NativeGameplayTags.h"
#include "PrimaryGameLayout.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

class FSubsystemCollectionBase;

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_UI_LAYER_MODAL, "UI.Layer.Modal");

void UMyCommonMessagingSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	ConfirmationDialogClassPtr = ConfirmationDialogClass.LoadSynchronous();
	ErrorDialogClassPtr = ErrorDialogClass.LoadSynchronous();
	OpenFileDialogClassPtr = OpenFileDialogClass.LoadSynchronous();
	SaveFileDialogClassPtr = SaveFileDialogClass.LoadSynchronous();
}

void UMyCommonMessagingSubsystem::ShowConfirmation(UCommonGameDialogDescriptor* DialogDescriptor, FCommonMessagingResultDelegate ResultCallback)
{
	if (!ensure(ConfirmationDialogClassPtr))
	{
		return;
	}
	if (UCommonLocalPlayer* LocalPlayer = GetLocalPlayer<UCommonLocalPlayer>())
	{
		if (UPrimaryGameLayout* RootLayout = LocalPlayer->GetRootUILayout())
		{
			RootLayout->PushWidgetToLayerStack<UCommonGameDialog>(TAG_UI_LAYER_MODAL, ConfirmationDialogClassPtr, [DialogDescriptor, ResultCallback](UCommonGameDialog& Dialog)
			    { Dialog.SetupDialog(DialogDescriptor, ResultCallback); });
		}
	}
}

void UMyCommonMessagingSubsystem::ShowError(UCommonGameDialogDescriptor* DialogDescriptor, FCommonMessagingResultDelegate ResultCallback)
{
	if (!ensure(ErrorDialogClassPtr))
	{
		return;
	}

	if (UCommonLocalPlayer* LocalPlayer = GetLocalPlayer<UCommonLocalPlayer>())
	{
		if (UPrimaryGameLayout* RootLayout = LocalPlayer->GetRootUILayout())
		{
			RootLayout->PushWidgetToLayerStack<UCommonGameDialog>(TAG_UI_LAYER_MODAL, ErrorDialogClassPtr, [DialogDescriptor, ResultCallback](UCommonGameDialog& Dialog)
			    { Dialog.SetupDialog(DialogDescriptor, ResultCallback); });
		}
	}
}

void UMyCommonMessagingSubsystem::ShowOpenFileDialog(UCommonGameDialogDescriptor* DialogDescriptor, FCommonMessagingResultDelegate ResultCallback)
{
	if (!ensure(OpenFileDialogClassPtr))
	{
		return;
	}
	if (UCommonLocalPlayer* LocalPlayer = GetLocalPlayer<UCommonLocalPlayer>())
	{
		if (UPrimaryGameLayout* RootLayout = LocalPlayer->GetRootUILayout())
		{
			RootLayout->PushWidgetToLayerStack<UCommonGameDialog>(TAG_UI_LAYER_MODAL, OpenFileDialogClassPtr, [DialogDescriptor, ResultCallback](UCommonGameDialog& Dialog)
			    { Dialog.SetupDialog(DialogDescriptor, ResultCallback); });
		}
	}
}

void UMyCommonMessagingSubsystem::ShowSaveFileDialog(UCommonGameDialogDescriptor* DialogDescriptor, FCommonMessagingResultDelegate ResultCallback)
{
	if (!ensure(SaveFileDialogClassPtr))
	{
		return;
	}
	if (UCommonLocalPlayer* LocalPlayer = GetLocalPlayer<UCommonLocalPlayer>())
	{
		if (UPrimaryGameLayout* RootLayout = LocalPlayer->GetRootUILayout())
		{
			RootLayout->PushWidgetToLayerStack<UCommonGameDialog>(TAG_UI_LAYER_MODAL, SaveFileDialogClassPtr, [DialogDescriptor, ResultCallback](UCommonGameDialog& Dialog)
			    { Dialog.SetupDialog(DialogDescriptor, ResultCallback); });
		}
	}
}