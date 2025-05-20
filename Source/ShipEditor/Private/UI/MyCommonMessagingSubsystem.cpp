// Copyright (c) 2024, sillygilly. All rights reserved.

#include "UI/MyCommonMessagingSubsystem.h"

#include "CommonLocalPlayer.h"
#include "Messaging/CommonGameDialog.h"
#include "Messaging/CommonGameFileDialog.h"
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
	FileDialogClassPtr = FileDialogClass.LoadSynchronous();
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

void UMyCommonMessagingSubsystem::ShowFileDialog(bool SaveDialog, FText Header, FString File, FFileDialogResultDelegate ResultCallback)
{
	if (!ensure(FileDialogClassPtr))
	{
		return;
	}
	if (UCommonLocalPlayer* LocalPlayer = GetLocalPlayer<UCommonLocalPlayer>())
	{
		if (UPrimaryGameLayout* RootLayout = LocalPlayer->GetRootUILayout())
		{
			RootLayout->PushWidgetToLayerStack<UCommonGameFileDialog>(TAG_UI_LAYER_MODAL, FileDialogClassPtr,
			    [SaveDialog, Header, File, ResultCallback](UCommonGameFileDialog& Dialog)
			    {
				    Dialog.SetupDialog(SaveDialog, Header, File, ResultCallback);
			    });
		}
	}
}
