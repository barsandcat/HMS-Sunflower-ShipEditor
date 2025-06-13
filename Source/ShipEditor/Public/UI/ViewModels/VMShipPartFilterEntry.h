// Copyright (c) 2025, sillygilly. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"

#include "VMShipPartFilterEntry.generated.h"

UCLASS()
class SHIPEDITOR_API UVMShipPartFilterEntry : public UMVVMViewModelBase
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter)
	FText Text;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter)
	FName Name;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter)
	int32 MatchingPartsCount = 0;

public:
	void SetText(const FText& text);
	FText GetText() const;

	void SetName(FName name);
	FName GetName() const;

	void SetMatchingPartsCount(int32 count);
	int32 GetMatchingPartsCount() const;
};

typedef TArray<TObjectPtr<UVMShipPartFilterEntry>> TUVMShipPartFilterEntryArray;