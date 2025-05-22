// Copyright (c) 2025, sillygilly. All rights reserved.

#include "UI/Widgets/MyCommonGameFileDialog.h"

#include "CommonTextBlock.h"
#include "Components/DynamicEntryBox.h"
#include "ICommonInputModule.h"
#include "InputAction.h"
#include "UI/Widgets/MyCommonButtonBase.h"

#define LOCTEXT_NAMESPACE "Messaging"

void UMyCommonGameFileDialog::SetupDialog(bool save_dialog, FText header, FString file, FFileDialogResultDelegate result_callback)
{
	File = file;
	IsSaveDialog = save_dialog;
	TextTitle->SetText(header);
	OnResultCallback = result_callback;

	EntryBoxButtons->Reset<UMyCommonButtonBase>([](UMyCommonButtonBase& Button)
	    { Button.OnClicked().Clear(); });

	if (IsSaveDialog)
	{
		UMyCommonButtonBase* Button = EntryBoxButtons->CreateEntry<UMyCommonButtonBase>();
		Button->SetTriggeringEnhancedInputAction(ICommonInputModule::GetSettings().GetEnhancedInputClickAction());
		Button->OnClicked().AddUObject(this, &ThisClass::OnConfirmClicked);
		Button->SetButtonText(LOCTEXT("Ok", "Ok"));
	}

	{
		UMyCommonButtonBase* Button = EntryBoxButtons->CreateEntry<UMyCommonButtonBase>();
		Button->SetTriggeringEnhancedInputAction(CancelAction);
		Button->OnClicked().AddUObject(this, &ThisClass::OnCancelClicked);
		Button->SetButtonText(LOCTEXT("Cancel", "Cancel"));
	}

	OnSetup(save_dialog, file);
}

void UMyCommonGameFileDialog::CloseDialogWithResult(ECommonMessagingResult result, FString file)
{
	DeactivateWidget();
	OnResultCallback.ExecuteIfBound(result, file);
}
