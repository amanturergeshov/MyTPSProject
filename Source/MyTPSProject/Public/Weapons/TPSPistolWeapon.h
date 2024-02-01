// MyTPSProject

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TPSPistolWeapon.generated.h"

class USkeletalMeshComponent;

class UStaticMeshComponent;

DECLARE_MULTICAST_DELEGATE(FOnClipEmptySignature);

USTRUCT(BlueprintType)
struct FAmmoData
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    int32 Bullets;
      
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon", meta = (EditCondition = "!infinite"))
    int32 Clips;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    bool Infinite;
};

UCLASS()
class MYTPSPROJECT_API ATPSPistolWeapon : public AActor
{
    GENERATED_BODY()

public:
    //________________________________FUNCTIONS______________________
    ATPSPistolWeapon();
    //_______________COMBAT____________
    virtual void StartFire();
    virtual void StopFire();
    //_______________RELOAD____________
    void ChangeClip();
    bool CanReload() const;
    //_______________GETINFO____________
    bool GetWeaponHeavy();
    int32 GetBullets();
    int32 GetClips();
    //_______________REPLICATION_______
    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

    

protected:
    /*  UPROPERTY(VisibleAnyWhere, BlueprintReadWrite, Category = "Components")
      USkeletalMeshComponent* WeaponMesh;*/

    virtual void BeginPlay() override;
    //_______________AMMO______________
    void DecreaseAmmo();
    bool IsAmmoEmpty() const;
    bool IsClipEmpty() const;
    //_______________SHOT______________
    virtual void MakeShot();
    void MakeDamage(const FHitResult& HitResult);
    void MakeHit(FHitResult& HitResult, const FVector& TraceStart, const FVector& TraceEnd);
    //_________GET INFO FOR SHOT_______
    APlayerController* GetPlayerController() const;
    bool GetPlayerViewPoint(FVector& ViewLocation, FRotator& ViewRotation) const;
    FVector GetMuzzleWorldLocation() const;
    bool GetTraceData(FVector& TraceStart, FVector& TraceEnd) const;

public:
    //_______________________________________PROPERTIES________________________________
    FOnClipEmptySignature OnClipEmpty;

    UPROPERTY(Replicated, EditDefaultsOnly, Category = "Weapon")
    bool HeavyWeapon = true;

protected:
    UPROPERTY(VisibleAnyWhere, BlueprintReadWrite, Category = "Components")
    UStaticMeshComponent* WeaponMesh;

    UPROPERTY(VisibleAnyWhere, BlueprintReadWrite, Category = "Weapon")
    FName MuzzleSocketName = "MuzzleSocket";

    UPROPERTY(VisibleAnyWhere, BlueprintReadWrite, Category = "Weapon")
    float TraceMaxDistance = 15000.0f;

    UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Weapon")
    float BulletSpread = 0.0f;

    UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Weapon")
    float DamageAmount = 10.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    FAmmoData DefaultAmmo{6, 6, false};

    UPROPERTY(VisibleAnyWhere, BlueprintReadWrite, Category = "Weapon")
    FAmmoData CurrentAmmo;
};

