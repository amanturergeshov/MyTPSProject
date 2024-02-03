// MyTPSProject

#include "Components/TPSHealthComponent.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY_STATIC(LogHealthComponent, All, All)

UTPSHealthComponent::UTPSHealthComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    // ...
}

void UTPSHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UTPSHealthComponent, Health)
}

// Called when the game starts
void UTPSHealthComponent::BeginPlay()
{
    Super::BeginPlay();

    Health = MaxHealth;

    OnHealthChanged.Broadcast(Health);

    AActor* ComponentOwner = GetOwner();
    if (ComponentOwner)
    {
        ComponentOwner->OnTakeAnyDamage.AddDynamic(this, &UTPSHealthComponent::OnTakeAnyDamage);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("ERROR:!!!"))
    }
}

void UTPSHealthComponent::OnTakeAnyDamage(
    AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
    if (Damage <= 0.0f || IsDead())
        return;

    Health = FMath::Clamp(Health - Damage, 0.0f, MaxHealth);
    OnHealthChanged.Broadcast(Health);
    if (IsDead())
    {
        OnDeathForScore.Broadcast(DamageCauser->GetOwner());
        OnDeath.Broadcast();
    }
}
