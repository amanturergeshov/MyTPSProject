// MyTPSProject

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TPSCharacter.generated.h"

class UTPSHealthComponent;

class USpringArmComponent;

class UCameraComponent;

class UTextRenderComponent;

class UTBSWeaponComponent;

UCLASS()
class MYTPSPROJECT_API ATPSCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    // Sets default values for this character's properties
    ATPSCharacter(const FObjectInitializer& ObjInit);

protected:
    // Called when the game starts or when spawned
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
    UCameraComponent* CameraComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
    USpringArmComponent* SpringArmComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
    UTPSHealthComponent* HealthComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
    UTextRenderComponent* HealthTextComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
    UTBSWeaponComponent* WeaponComponent;

    UPROPERTY(EditDefaultsOnly, Category = "Animation")
    UAnimMontage* DeathAnimMontage;

    //UPROPERTY(Replicated,VisibleAnywhere, BlueprintReadWrite)
    //float Health = HealthComponent->GetHealth();


    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    UFUNCTION( BlueprintCallable, Category = "Movement")
    bool IsSprinting() const;

    UFUNCTION(BlueprintCallable, Category = "Movement")
    bool InCombat() const;

    UFUNCTION(BlueprintCallable, Category = "Movement")
    bool HeavyWeapon() const;

    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite)
    bool IsFighting = false;

    UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite)
    bool WantsToRun = false;

private:
    bool isMovingForward = false;
    void MoveForward(float Amount);
    void MoveRight(float Amount);

    void OnStartSprint();
    void OnStopSprint();

    void OnDeath();
    
    void OnHealthChanged(float);

    UFUNCTION(BlueprintCallable)
    void StartCombat();

    UFUNCTION(BlueprintCallable)
    void StopCombat();

    

};
