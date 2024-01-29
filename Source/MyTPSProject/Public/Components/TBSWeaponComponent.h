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
    UTBSWeaponComponent();
    //******************************************FUNCTIONS*********************************
    void StartFire();
    void StopFire();
    void NextWeapon();
    void ChangeClip();
    //__________________________________________GETINFO____________________________________
    UFUNCTION(BlueprintCallable)
    int32 GetCurrentBullets();
    UFUNCTION(BlueprintCallable)
    int32 GetCurrentClips();
    bool GetEquipType();

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    //___________________________________REPLICATION_____________________
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    //_________________________________
    // Reload
    UFUNCTION(Server, Reliable)
    void ServerChangeClip();
    UFUNCTION(NetMulticast, Reliable)
    void MulticastChangeClip();

    //__________________________________________
    // ReloadAnimation
    UFUNCTION(Server, Reliable)
    void ServerPlayAnimation(UAnimMontage* Animation);
    UFUNCTION(NetMulticast, Reliable)
    void MulticastPlayAnimation(UAnimMontage* Animation);
    //__________________________________________
private:
    void SpawnWeapons();
    void InitAnimations();
    void AttachWeaponToSocket(ATPSPistolWeapon* Weapon, USceneComponent* SceneComponent, const FName& SocketName);
    void EquipWeapon(int32 WeaponIndex);
    void PlayAnimMontage(UAnimMontage* Animation);
    void OnReloadFinished(USkeletalMeshComponent* MeshComp);
    //__________________________________________GETINFO____________________________________
    bool CanFire() const;
    bool CanReload() const;

protected:
    //******************************************PROPERTIES***********************************
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


private:
    UPROPERTY()
    ATPSPistolWeapon* CurrentWeapon = nullptr;

    UPROPERTY()
    TArray<ATPSPistolWeapon*> Weapons;

    UPROPERTY(Replicated)
    UAnimMontage* CurrenReloadAnimMontage = nullptr;

    int32 CurrentWeaponIndex = 0;

    bool ReloadAnimInProgress = false;

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