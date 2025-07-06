// Copyright (c) 2025, sillygilly. All rights reserved.

#pragma once

#include "Components/ComboBoxKey.h"
#include "CoreMinimal.h"
#include "UI/Widgets/MyCommonButtonBase.h"

#include "WFilterEntry.generated.h"

class UVMShipPartFilterEntry;

UCLASS()
class SHIPEDITOR_API UWFilterEntry : public UMyCommonButtonBase
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void SetVMEntries(const TArray<UVMShipPartFilterEntry*>& entries);

	UPROPERTY(BlueprintReadWrite, Meta = (BindWidget))
	TObjectPtr<UComboBoxKey> Combobox;
};
