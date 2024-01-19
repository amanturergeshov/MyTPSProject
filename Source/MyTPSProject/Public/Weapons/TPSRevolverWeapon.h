// MyTPSProject

#pragma once

#include "CoreMinimal.h"
#include "Weapons/TPSPistolWeapon.h"
#include "TPSRevolverWeapon.generated.h"

/**
 * 
 */
UCLASS()
class MYTPSPROJECT_API ATPSRevolverWeapon : public ATPSPistolWeapon
{
	GENERATED_BODY()
public:

    virtual void StartFire() override;
    virtual void StopFire() override;
    virtual bool GetTraceData(FVector& TraceStart, FVector& TraceEnd) const override;

protected:
    UPROPERTY(EditAnyWhere, BlueprintReadWrite)
    float TimerBetweenShot = 0.5f;

    UPROPERTY(EditAnyWhere, BlueprintReadWrite)
    float BulletSpread = 1.0f;

    bool HeavyWeapon = true;

    virtual void MakeShot() override;

private:
    FTimerHandle ShotTimerHandle;
};
