#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "EditStateInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(BlueprintType, Blueprintable)
class UEditStateInterface : public UInterface
{
	GENERATED_BODY()
};

class SHIPEDITOR_API IEditStateInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	TScriptInterface<IEditStateInterface> SelectPart(int partId);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	TScriptInterface<IEditStateInterface> Cancel();
};
