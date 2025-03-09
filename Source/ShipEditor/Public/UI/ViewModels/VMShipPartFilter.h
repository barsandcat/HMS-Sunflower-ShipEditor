// Copyright (c) 2025, sillygilly. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"

#include "VMShipPartFilter.generated.h"

UCLASS()
class SHIPEDITOR_API UVMShipPartFilter : public UMVVMViewModelBase
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter)
	int32 FilterId = 0;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter)
	TArray<FName> Options;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter)
	FText Selected;

public:
	void SetFilterId(int32 filter_id);
	int32 GetFilterId() const;

	void SetOptions(const TArray<FName>& options);
	const TArray<FName>& GetOptions() const;

	void SetSelected(FText selected);
	FText GetSelected() const;
};

typedef TArray<TObjectPtr<UVMShipPartFilter>> TUVMShipPartFilterArray;