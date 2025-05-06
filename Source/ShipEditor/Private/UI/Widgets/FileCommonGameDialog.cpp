// Copyright (c) 2025, sillygilly. All rights reserved.

#include "UI/Widgets/FileCommonGameDialog.h"

#include "CommonTextBlock.h"

void UFileCommonGameDialog::SetupDialog(UCommonGameDialogDescriptor* descriptor, FCommonMessagingResultDelegate result_callback)
{
	Super::SetupDialog(descriptor, result_callback);

	TextTitle->SetText(descriptor->Header);

	{
		UInputAction* input_action = CancelAction;
		// UMyCommonButtonBase* Button = EntryBox_Buttons->CreateEntry<UMyCommonButtonBase>();
		// Button->SetTriggeringEnhancedInputAction(input_action);
		// Button->OnClicked().AddUObject(this, &ThisClass::CloseConfirmationWindow, Action.Result);
		// Button->SetButtonText(Action.OptionalDisplayText);
	}

	OnResultCallback = result_callback;
}

void UFileCommonGameDialog::KillDialog()
{
	Super::KillDialog();
}
