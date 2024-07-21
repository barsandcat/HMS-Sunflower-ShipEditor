#include "MyGameMode.h"

#include "EditState/SelectEditState.h"

void AMyGameMode::StartPlay()
{
	EditState = NewObject<USelectEditState>();
}
