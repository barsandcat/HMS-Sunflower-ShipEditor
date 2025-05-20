// Copyright (c) 2024, sillygilly. All rights reserved.

#include "UI/Widgets/MyCommonGameDialog.h"

#if WITH_EDITOR
#include "CommonInputSettings.h"
#include "Editor/WidgetCompilerLog.h"
#endif

#include "CommonBorder.h"
#include "CommonRichTextBlock.h"
#include "CommonTextBlock.h"
#include "Components/DynamicEntryBox.h"
#include "ICommonInputModule.h"
#include "InputAction.h"
#include "UI/Widgets/MyCommonButtonBase.h"

void UMyCommonGameDialog::SetupDialog(UCommonGameDialogDescriptor* Descriptor, FCommonMessagingResultDelegate ResultCallback)
{
	Super::SetupDialog(Descriptor, ResultCallback);

	Text_Title->SetText(Descriptor->Header);
	RichText_Description->SetText(Descriptor->Body);

	EntryBox_Buttons->Reset<UMyCommonButtonBase>([](UMyCommonButtonBase& Button)
	    { Button.OnClicked().Clear(); });

	for (const FConfirmationDialogAction& Action : Descriptor->ButtonActions)
	{
		UInputAction* input_action;

		switch (Action.Result)
		{
			case ECommonMessagingResult::Confirmed:
				input_action = ICommonInputModule::GetSettings().GetEnhancedInputClickAction();
				break;
			case ECommonMessagingResult::Declined:
				input_action = ICommonInputModule::GetSettings().GetEnhancedInputBackAction();
				break;
			case ECommonMessagingResult::Cancelled:
				input_action = CancelAction;
				break;
			default:
				ensure(false);
				continue;
		}

		UMyCommonButtonBase* Button = EntryBox_Buttons->CreateEntry<UMyCommonButtonBase>();
		Button->SetTriggeringEnhancedInputAction(input_action);
		Button->OnClicked().AddUObject(this, &ThisClass::CloseDialogWithResult, Action.Result);
		Button->SetButtonText(Action.OptionalDisplayText);
	}

	OnResultCallback = ResultCallback;
}

void UMyCommonGameDialog::KillDialog()
{
	Super::KillDialog();
}

void UMyCommonGameDialog::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Border_TapToCloseZone->OnMouseButtonDownEvent.BindDynamic(this, &UMyCommonGameDialog::HandleTapToCloseZoneMouseButtonDown);
}

void UMyCommonGameDialog::CloseDialogWithResult(ECommonMessagingResult Result)
{
	DeactivateWidget();
	OnResultCallback.ExecuteIfBound(Result);
}

FEventReply UMyCommonGameDialog::HandleTapToCloseZoneMouseButtonDown(FGeometry MyGeometry, const FPointerEvent& MouseEvent)
{
	FEventReply Reply;
	Reply.NativeReply = FReply::Unhandled();

	if (MouseEvent.IsTouchEvent() || MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		CloseDialogWithResult(ECommonMessagingResult::Declined);
		Reply.NativeReply = FReply::Handled();
	}

	return Reply;
}

#if WITH_EDITOR
void UMyCommonGameDialog::ValidateCompiledDefaults(IWidgetCompilerLog& CompileLog) const
{
	if (CancelAction.IsNull())
	{
		CompileLog.Error(FText::Format(FText::FromString(TEXT("{0} has unset property: CancelAction.")), FText::FromString(GetName())));
	}
}
#endif
