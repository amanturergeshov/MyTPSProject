// MyTPSProject

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "TPSCharacter.generated.h"


class UTPSHealthComponent;
class USpringArmComponent;
class UCameraComponent;
class UTextRenderComponent;
class UTBSWeaponComponent;
class UInputMappingContext;
class UInputAction;

UCLASS()
class MYTPSPROJECT_API ATPSCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ATPSCharacter(const FObjectInitializer& ObjInit);

protected:
    //-------------------------Components--------------------------
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

    //-------------------------INPUTS--------------------------
    UPROPERTY(EditAnywhere, Category = Input)
    UInputMappingContext* SlashContext;

    UPROPERTY(EditAnywhere, Category = Input)
    UInputAction* MovementAction;

    UPROPERTY(EditAnywhere, Category = Input)
    UInputAction* JumpAction;

    UPROPERTY(EditAnywhere, Category = Input)
    UInputAction* SprintAction;

    UPROPERTY(EditAnywhere, Category = Input)
    UInputAction* ChangeWeaponAction;

    UPROPERTY(EditAnywhere, Category = Input)
    UInputAction* ReloadAction;
    
    UPROPERTY(EditAnywhere, Category = Input)
    UInputAction* ShootAction;
    //________________________________________________________

    virtual void BeginPlay() override;

public:

    //___________________________REPLICATION______________________
    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    // StartSprint
    UFUNCTION(Server, Unreliable)
    void ServerOnStartSprint();
    UFUNCTION(NetMulticast, Unreliable)
    void MulticastOnStartSprint();

    //______________________________________________________________
    // StopSprint
    UFUNCTION(Server, Unreliable)
    void ServerOnStopSprint();
    UFUNCTION(NetMulticast, Unreliable)
    void MulticastOnStopSprint();
    //_____________________________________________________________
    // StartCombat
    UFUNCTION(Server, Reliable)
    void ServerOnStartCombat();
    UFUNCTION(NetMulticast, Reliable)
    void MulticastOnStartCombat();

    //______________________________________________________________
    // StopCombat
    UFUNCTION(Server, Reliable)
    void ServerOnStopCombat();
    UFUNCTION(NetMulticast, Reliable)
    void MulticastOnStopCombat();

    //______________________________________________________________
    // MoveForward
    UFUNCTION(Server, Unreliable)
    void ServerMoveForward(float Amount);
    UFUNCTION(NetMulticast, Unreliable)
    void MulticastMoveForward(float Amount);

    //______________________________________________________________
    // MoveForward
    UFUNCTION(Server, Unreliable)
    void ServerSetSprinting();
    UFUNCTION(NetMulticast, Unreliable)
    void MulticastSetSprinting();

    //______________________________________________________________

    UFUNCTION(BlueprintCallable, Category = "Movement")
    bool HeavyWeapon() const;
    UFUNCTION(BlueprintCallable, Category = "Movement")
    void SetSprinting();
    void Move(const FInputActionValue& Value);
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
private:
    //_____________________
    void MoveForward(float Amount);
    void MoveRight(float Amount);
    //_____________________
    void OnStartSprint();
    void OnStopSprint();
    //_____________________
    void OnDeath();
    void OnHealthChanged(float);
    //_____________________
    void StartCombat();
    void StopCombat();
    //_______________________
    void Reload();

public:
    //_______________________PROPERTIES___________________
    UPROPERTY(Replicated, BlueprintReadWrite)
    bool IsFighting = false;

    UPROPERTY(Replicated, BlueprintReadWrite)
    bool WantsToRun = false;

    UPROPERTY(Replicated, BlueprintReadWrite)
    bool IsSprinting = false;

    UPROPERTY(Replicated, BlueprintReadWrite)
    bool isMovingForward = false;
};
