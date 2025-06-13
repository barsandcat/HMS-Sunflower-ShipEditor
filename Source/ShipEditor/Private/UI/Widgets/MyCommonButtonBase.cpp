// Copyright (c) 2024, sillygilly. All rights reserved.

#include "UI/Widgets/MyCommonButtonBase.h"

#include "CommonActionWidget.h"

void UMyCommonButtonBase::NativePreConstruct()
{
	Super::NativePreConstruct();

	UpdateButtonStyle();
	RefreshButtonText();
}

void UMyCommonButtonBase::UpdateInputActionWidget()
{
	Super::UpdateInputActionWidget();

	UpdateButtonStyle();
	RefreshButtonText();
}

void UMyCommonButtonBase::SetButtonText(const FText& InText)
{
	OverrideButtonText = InText.IsEmpty();
	ButtonText = InText;
	RefreshButtonText();
}

void UMyCommonButtonBase::RefreshButtonText()
{
	if (OverrideButtonText || ButtonText.IsEmpty())
	{
		if (InputActionWidget)
		{
			const FText ActionDisplayText = InputActionWidget->GetDisplayText();
			if (!ActionDisplayText.IsEmpty())
			{
				UpdateButtonText(ActionDisplayText);
				return;
			}
		}
	}

	UpdateButtonText(ButtonText);
}

void UMyCommonButtonBase::OnInputMethodChanged(ECommonInputType CurrentInputType)
{
	Super::OnInputMethodChanged(CurrentInputType);

	UpdateButtonStyle();
}