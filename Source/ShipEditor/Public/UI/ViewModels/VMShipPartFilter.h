// Copyright (c) 2025, sillygilly. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "VMShipPartFilterEntry.h"

#include "VMShipPartFilter.generated.h"

UCLASS()
class SHIPEDITOR_API UVMShipPartFilter : public UMVVMViewModelBase
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter)
	int32 FilterId = 0;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter)
	FName Selected;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter)
	TArray<TObjectPtr<UVMShipPartFilterEntry>> VMEntries;

public:
	UFUNCTION(BlueprintPure, FieldNotify)
	TArray<FName> GetOptions() const;

	void SetFilterId(int32 filter_id);
	int32 GetFilterId() const;

	void SetSelected(FName selected);
	FName GetSelected() const;

	void SetVMEntries(const TUVMShipPartFilterEntryArray& entries);
	const TArray<TObjectPtr<UVMShipPartFilterEntry>>& GetVMEntries() const;

	UFUNCTION(BlueprintCallable)
	UVMShipPartFilterEntry* GetEntryByName(FName name) const
	{
		for (const TObjectPtr<UVMShipPartFilterEntry>& entry : VMEntries)
		{
			if (entry->GetName() == name)
			{
				return entry;
			}
		}
		return nullptr;
	}
};

typedef TArray<TObjectPtr<UVMShipPartFilter>> TUVMShipPartFilterArray;