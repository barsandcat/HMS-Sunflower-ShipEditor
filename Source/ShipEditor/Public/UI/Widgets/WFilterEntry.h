// Copyright (c) 2025, sillygilly. All rights reserved.

#pragma once

#include "Components/ComboBoxKey.h"
#include "CoreMinimal.h"
#include "UI/Widgets/MyCommonButtonBase.h"

#include "WFilterEntry.generated.h"

UCLASS()
class SHIPEDITOR_API UWFilterEntry : public UMyCommonButtonBase
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable)
	void SetOptions(const TArray<FName>& options_array);

	UPROPERTY(BlueprintReadWrite, Meta = (BindWidget))
	TObjectPtr<UComboBoxKey> Combobox;
};
