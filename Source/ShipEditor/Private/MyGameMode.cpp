#include "MyGameMode.h"

#include "EditState/SelectEditState.h"

void AMyGameMode::StartPlay()
{
	Super::StartPlay();
	EditState = NewObject<USelectEditState>();
}
