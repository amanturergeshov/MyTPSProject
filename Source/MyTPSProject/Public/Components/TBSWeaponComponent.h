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
    //******************************FUNCTIONS*****************************
    UTBSWeaponComponent();
    //_________________________________COMBAT_________________________
    void StartFire();
    void StopFire();
    //_________________________________CHANGE_WEAPON_________________________
    void NextWeapon();
    //_________________________________CHANGE_CLIP_________________________
    void ChangeClip();
    //_________________________________GET INFO_________________________
    bool GetEquipType();
    UFUNCTION(BlueprintCallable)
    int32 GetCurrentBullets();
    UFUNCTION(BlueprintCallable)
    int32 GetCurrentClips();

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    //********************************REPLICATION********************************
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
    void SpawnWeapons();

    void InitAnimations();

    void AttachWeaponToSocket(ATPSPistolWeapon* Weapon, USceneComponent* SceneComponent, const FName& SocketName);

    void EquipWeapon(int32 WeaponIndex);

    void PlayAnimMontage(UAnimMontage* Animation);

    void OnReloadFinished(USkeletalMeshComponent* MeshComp);

    //_________________________________GET INFO_________________________
    bool ReloadAnimInProgress = false;
    bool CanFire() const;
    bool CanReload() const;


protected:
    //*******************************PROPERTIES********************************
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
    //********************************PROPERTIES**********************************
    UPROPERTY()
    ATPSPistolWeapon* CurrentWeapon = nullptr;

    UPROPERTY()
    TArray<ATPSPistolWeapon*> Weapons;

    UPROPERTY(Replicated)
    UAnimMontage* CurrenReloadAnimMontage = nullptr;

    int32 CurrentWeaponIndex = 0;


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