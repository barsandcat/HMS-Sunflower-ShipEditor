// Copyright (c) 2024, sillygilly. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "VMShipPart.h"
#include "VMShipPartCategory.h"
#include "VMShipPartFilter.h"

#include "VMPartBrowser.generated.h"

UCLASS()
class SHIPEDITOR_API UVMPartBrowser : public UMVVMViewModelBase
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter)
	TArray<TObjectPtr<UVMShipPartCategory>> CategoryList;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter)
	TArray<TObjectPtr<UVMShipPartFilter>> FilterList;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter)
	TArray<TObjectPtr<UVMShipPart>> PartList;

public:
	void SetCategoryList(TUVMShipPartCategoryArray& categoryies);
	const TArray<TObjectPtr<UVMShipPartCategory>>& GetCategoryList() const;

	void SetFilterList(TUVMShipPartFilterArray& filters);
	const TArray<TObjectPtr<UVMShipPartFilter>>& GetFilterList() const;

	void SetPartList(TUVMShipPartArray& part_list);
	const TArray<TObjectPtr<UVMShipPart>>& GetPartList() const;
};
