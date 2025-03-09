// Copyright (c) 2025, sillygilly. All rights reserved.

#include "UI/Widgets/MyComboBoxKey.h"

void UMyComboBoxKey::SetOptions(const TArray<FName>& options_array)
{
	ClearOptions();
	for (const FName& option : options_array)
	{
		AddOption(option);
	}
}
