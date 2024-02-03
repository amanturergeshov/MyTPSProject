// MyTPSProject

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TPSHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeathForScore, AActor*, DamageCauser);
DECLARE_MULTICAST_DELEGATE(FOnDeath);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHealthChanged, float);


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MYTPSPROJECT_API UTPSHealthComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    // Sets default values for this component's properties
    UTPSHealthComponent();

    UFUNCTION(BlueprintCallable)
    bool IsDead() const { return Health <= 0.0f;  }

    UFUNCTION(BlueprintCallable)
    float GetHealth() const { return Health; }

    UPROPERTY(BlueprintAssignable)
    FOnDeathForScore OnDeathForScore;

    FOnDeath OnDeath;

    FOnHealthChanged OnHealthChanged;

    UPROPERTY(Replicated)
    float Health = 0.0f;

    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Health", meta = (ClampMin = "0.0"))
    float MaxHealth = 100.0f;

    // Called when the game starts
    virtual void BeginPlay() override;

private:

    UFUNCTION()
    void OnTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType,
        class AController* InstigatedBy, AActor* DamageCauser);
};
