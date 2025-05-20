// Copyright (c) 2025, sillygilly. All rights reserved.

#include "UI/Widgets/MyCommonGameFileDialog.h"

#include "CommonTextBlock.h"
#include "Components/DynamicEntryBox.h"
#include "ICommonInputModule.h"
#include "InputAction.h"
#include "UI/Widgets/MyCommonButtonBase.h"

void UMyCommonGameFileDialog::SetupDialog(bool save_dialog, FText header, FString file, FFileDialogResultDelegate result_callback)
{
	File = file;
	IsSaveDialog = save_dialog;
	TextTitle->SetText(header);
	OnResultCallback = result_callback;

	EntryBox_Buttons->Reset<UMyCommonButtonBase>([](UMyCommonButtonBase& Button)
	    { Button.OnClicked().Clear(); });

	{
		UMyCommonButtonBase* Button = EntryBox_Buttons->CreateEntry<UMyCommonButtonBase>();
		Button->SetTriggeringEnhancedInputAction(ICommonInputModule::GetSettings().GetEnhancedInputClickAction());
		Button->OnClicked().AddUObject(this, &ThisClass::CloseDialogWithResult, ECommonMessagingResult::Confirmed, File);
	}

	{
		UMyCommonButtonBase* Button = EntryBox_Buttons->CreateEntry<UMyCommonButtonBase>();
		Button->SetTriggeringEnhancedInputAction(CancelAction);
		Button->OnClicked().AddUObject(this, &ThisClass::CloseDialogWithResult, ECommonMessagingResult::Cancelled, FString());
	}
}

void UMyCommonGameFileDialog::CloseDialogWithResult(ECommonMessagingResult result, FString file)
{
	DeactivateWidget();
	OnResultCallback.ExecuteIfBound(result, file);
}
