// MyTPSProject

#include "Components/TPSCharacterMovementComponent.h"
#include "Player/TPSCharacter.h"
#include "Net/UnrealNetwork.h"

float UTPSCharacterMovementComponent::GetMaxSpeed() const
{
    const float MaxSpeed = Super::GetMaxSpeed();
    const ATPSCharacter* Player = Cast<ATPSCharacter>(GetPawnOwner());

    return Player && Player->IsSprinting ? MaxSpeed + Modifier : MaxSpeed;
}
void UTPSCharacterMovementComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UTPSCharacterMovementComponent, Modifier);
}