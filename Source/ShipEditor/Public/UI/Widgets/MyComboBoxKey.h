// Copyright (c) 2025, sillygilly. All rights reserved.

#pragma once

#include "Components/ComboBoxKey.h"
#include "CoreMinimal.h"

#include "MyComboBoxKey.generated.h"

UCLASS()
class SHIPEDITOR_API UMyComboBoxKey : public UComboBoxKey
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void SetOptions(const TArray<FName>& options_array);
};
