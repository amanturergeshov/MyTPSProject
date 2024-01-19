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
    // Sets default values for this actor's properties
    ATPSPistolWeapon();
    UFUNCTION(BlueprintCallable, Category = "Movement")
    virtual void StartFire();
    virtual void StopFire();
    bool GetWeaponHeavy();
    FOnClipEmptySignature OnClipEmpty;
    UPROPERTY(Replicated,EditDefaultsOnly, Category = "Weapon")
    bool HeavyWeapon=true;

    
    void ChangeClip();

    bool CanReload() const;

    int32 GetBullets();
    int32 GetClips();
    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

protected:
    /*  UPROPERTY(VisibleAnyWhere, BlueprintReadWrite, Category = "Components")
      USkeletalMeshComponent* WeaponMesh;*/

    UPROPERTY(VisibleAnyWhere, BlueprintReadWrite, Category = "Components")
    UStaticMeshComponent* WeaponMesh;

    UPROPERTY(VisibleAnyWhere, BlueprintReadWrite)
    FName MuzzleSocketName = "MuzzleSocket";

    UPROPERTY(VisibleAnyWhere, BlueprintReadWrite)
    float TraceMaxDistance = 15000.0f;

    UPROPERTY(VisibleAnyWhere, BlueprintReadWrite)
    float DamageAmount = 10.0f;

    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    FAmmoData DefaultAmmo{6, 6, false};

    
    UPROPERTY(VisibleAnyWhere, BlueprintReadWrite)
    FAmmoData CurrentAmmo;

    virtual void BeginPlay() override;


    virtual void MakeShot();

    APlayerController* GetPlayerController() const;

    bool GetPlayerViewPoint(FVector& ViewLocation, FRotator& ViewRotation) const;

    FVector GetMuzzleWorldLocation() const;

    virtual bool GetTraceData(FVector& TraceStart, FVector& TraceEnd) const;

    void MakeHit(FHitResult& HitResult, const FVector& TraceStart, const FVector& TraceEnd);
    void MakeDamage(const FHitResult& HitResult);

    void DecreaseAmmo();
    bool IsAmmoEmpty() const;
    bool IsClipEmpty() const;
    void LogAmmo();

};
