// MyTPSProject

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TBSWeaponComponent.generated.h"

class ATPSPistolWeapon;

USTRUCT(BlueprintType)
struct FWeaponData
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    TSubclassOf<ATPSPistolWeapon> WeaponClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    UAnimMontage* ReloadAnimMontage;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MYTPSPROJECT_API UTBSWeaponComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    // Sets default values for this component's properties
    UTBSWeaponComponent();

    void StartFire();
    void StopFire();
    void NextWeapon();
    bool GetEquipType();
    void Reload();
    UFUNCTION(BlueprintCallable)
    int32 GetCurrentBullets();
    UFUNCTION(BlueprintCallable)
    int32 GetCurrentClips();
    

protected:
    // Called when the game starts
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    TArray<FWeaponData> WeaponData;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    FName WeaponEquipSocketName = "BazookaSocket";

    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    FName HeavyWeaponArmorySocketName = "BazookaSocket";

    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    FName WeaponArmorySocketName = "pistolSocket";

    UPROPERTY(EditDefaultsOnly, Category = "Animation")
    UAnimMontage* EquipAnimMontage;

    UPROPERTY(EditDefaultsOnly, Category = "Animation")
    UAnimMontage* HeavyEquipAnimMontage;

    //___________________________________–≈œÀ» ¿÷»ﬂ_____________________
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    //_________________________________
    //Reload
    UFUNCTION(Server,Reliable)
    void ServerChangeClip();
    UFUNCTION(NetMulticast, Reliable)
    void MulticastChangeClip();
    
    //__________________________________________
    //ReloadAnimation
    UFUNCTION(Server, Reliable)
    void ServerPlayAnimation(UAnimMontage* Animation);
    UFUNCTION(NetMulticast, Reliable)
    void MulticastPlayAnimation(UAnimMontage* Animation);
    //__________________________________________



private:
    UPROPERTY()
    ATPSPistolWeapon* CurrentWeapon = nullptr;

    UPROPERTY()
    TArray<ATPSPistolWeapon*> Weapons;

    UPROPERTY(Replicated)
    UAnimMontage* CurrenReloadAnimMontage = nullptr;

    int32 CurrentWeaponIndex = 0;

    void SpawnWeapons();

    void InitAnimations();

    void AttachWeaponToSocket(ATPSPistolWeapon* Weapon, USceneComponent* SceneComponent, const FName& SocketName);

    void EquipWeapon(int32 WeaponIndex);

    void PlayAnimMontage(UAnimMontage* Animation);

    bool ReloadAnimInProgress = false;
    void OnReloadFinished(USkeletalMeshComponent* MeshComp);

    bool CanFire() const;
    bool CanReload() const;

    void OnEmptyClip();
    void ChangeClip();

    template <typename T> T* FindNotifyByClass(UAnimSequenceBase* Animation)
    {
        if (!Animation)
            return nullptr;
        const auto NotifyEvents = Animation->Notifies;
        for (auto NotifyEvent : NotifyEvents)
        {
            auto AnimNotify = Cast<T>(NotifyEvent.Notify);
            if (AnimNotify)
            {
                return AnimNotify;
            }
        }
        return nullptr;
    }
};