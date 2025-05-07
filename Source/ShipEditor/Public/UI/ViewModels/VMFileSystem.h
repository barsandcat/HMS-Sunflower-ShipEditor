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
	UVMFileSystem(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	UFUNCTION(BlueprintPure, FieldNotify)
	TArray<UVMFileName*> GetFiles() const;

	UFUNCTION(BlueprintCallable)
	void SetCWD(const FString& new_cwd);

	FString GetCWD() const;
};
