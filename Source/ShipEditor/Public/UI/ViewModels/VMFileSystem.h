// Copyright (c) 2025, sillygilly. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "VMFileName.h"
#include "VMFileSystem.generated.h"

UCLASS()
class SHIPEDITOR_API UVMFileSystem : public UMVVMViewModelBase
{
	GENERATED_BODY()
protected:

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter)
	FString CWD;

public:
	UFUNCTION(BlueprintPure, FieldNotify)
	TArray<UVMFileName*> GetFiles() const;

	void SetCWD(const FString& cwd);
	FString GetCWD() const;	
};
