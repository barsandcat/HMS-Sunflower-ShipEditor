// Copyright (c) 2024, sillygilly. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "VMShipPart.h"
#include "VMShipPartCategory.h"

#include "VMPartBrowser.generated.h"

UCLASS()
class SHIPEDITOR_API UVMPartBrowser : public UMVVMViewModelBase
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter)
	int32 PartId = 0;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter)
	int32 CategoryId = 0;

	UPROPERTY(BlueprintReadOnly, FieldNotify)
	TArray<TObjectPtr<UVMShipPartCategory>> CategoryList;

	UPROPERTY(BlueprintReadOnly, FieldNotify)
	TArray<TObjectPtr<UVMShipPart>> PartList;

public:
	void SetPartId(int32 Id);
	int32 GetPartId() const;

	void SetCategoryId(int32 id);
	int32 GetCategoryId() const;

	void SetCategoryList(TUVMShipPartCategoryArray& categoryies);

	void SetPartList(TUVMShipPartArray& NewPartList);
};
