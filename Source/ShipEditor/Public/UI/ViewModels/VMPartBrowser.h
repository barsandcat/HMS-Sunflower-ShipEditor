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
	UPROPERTY(BlueprintReadOnly, FieldNotify)
	TArray<TObjectPtr<UVMShipPartCategory>> CategoryList;

	UPROPERTY(BlueprintReadOnly, FieldNotify)
	TArray<TObjectPtr<UVMShipPart>> PartList;

public:
	void SetCategoryList(TUVMShipPartCategoryArray& categoryies);
	const TUVMShipPartCategoryArray& GetCategoryList() const;

	void SetPartList(TUVMShipPartArray& NewPartList);
};
