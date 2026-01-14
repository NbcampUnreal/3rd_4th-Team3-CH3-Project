#include "CHGameMode.h"
#include "CHCharacter.h"
#include "CHPlayerController.h"

ACHGameMode::ACHGameMode()
{
    DefaultPawnClass = ACHCharacter::StaticClass();
    PlayerControllerClass = ACHPlayerController::StaticClass();
}
