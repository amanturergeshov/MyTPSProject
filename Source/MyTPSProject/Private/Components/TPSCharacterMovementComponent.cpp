// MyTPSProject

#include "Components/TPSCharacterMovementComponent.h"
#include "Player/TPSCharacter.h"

float UTPSCharacterMovementComponent::GetMaxSpeed() const
{
    const float MaxSpeed = Super::GetMaxSpeed();
    const ATPSCharacter* Player = Cast<ATPSCharacter>(GetPawnOwner());

    return Player && Player->IsSprinting() ? MaxSpeed + Modifier : MaxSpeed;
}
