// Copyright (c) 2025, sillygilly. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"

#include "VMShipPartCategory.generated.h"

UCLASS(BlueprintType)
class SHIPEDITOR_API UVMShipPartCategory : public UMVVMViewModelBase
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, FieldNotify)
	FText Name;

	UPROPERTY(BlueprintReadOnly, FieldNotify)
	int32 CategoryId = 0;

public:
	void SetName(const FText& name);
	void SetCategoryId(int32 category_id);
};

typedef TArray<TObjectPtr<UVMShipPartCategory>> TUVMShipPartCategoryArray;
