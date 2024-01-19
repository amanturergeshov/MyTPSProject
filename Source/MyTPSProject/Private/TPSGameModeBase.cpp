// MyTPSProject


#include "TPSGameModeBase.h"
#include "Player/MyTPSPlayerController.h"
#include "Player/TPSCharacter.h"

ATPSGameModeBase::ATPSGameModeBase() {
    DefaultPawnClass = ATPSCharacter::StaticClass();
    PlayerControllerClass = AMyTPSPlayerController::StaticClass();
}
