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
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter)
	FText Name;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter)
	int32 CategoryId = 0;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter)
	bool Selected = false;

public:
	void SetName(const FText& name);
	const FText& GetName() const;

	void SetCategoryId(int32 category_id);
	int32 GetCategoryId() const;

	void SetSelected(bool selected);
	bool GetSelected() const;
};

typedef TArray<TObjectPtr<UVMShipPartCategory>> TUVMShipPartCategoryArray;
