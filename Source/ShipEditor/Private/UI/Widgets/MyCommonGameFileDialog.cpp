// Copyright (c) 2025, sillygilly. All rights reserved.

#include "UI/Widgets/MyCommonGameFileDialog.h"

#include "CommonTextBlock.h"

void UMyCommonGameFileDialog::SetupDialog(bool save_dialog, FText header, FString file, FFileDialogResultDelegate result_callback)
{
	TextTitle->SetText(header);

	{
		UInputAction* input_action = CancelAction;
		// UMyCommonButtonBase* Button = EntryBox_Buttons->CreateEntry<UMyCommonButtonBase>();
		// Button->SetTriggeringEnhancedInputAction(input_action);
		// Button->OnClicked().AddUObject(this, &ThisClass::CloseConfirmationWindow, Action.Result);
		// Button->SetButtonText(Action.OptionalDisplayText);
	}

	OnResultCallback = result_callback;
}
