// MyTPSProject

#pragma once

#include "CoreMinimal.h"
#include "Weapons/TPSPistolWeapon.h"
#include "TPSBazookaWeapon.generated.h"

class ATPSProjectile;
/**
 *
 */
UCLASS()
class MYTPSPROJECT_API ATPSBazookaWeapon : public ATPSPistolWeapon
{
    GENERATED_BODY()

public:

    virtual void StartFire() override;

protected:
    bool HeavyWeapon = true;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    TSubclassOf<ATPSProjectile> ProjectileClass;

    virtual void MakeShot() override;
};
