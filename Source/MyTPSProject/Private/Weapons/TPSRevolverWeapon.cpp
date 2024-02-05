// MyTPSProject

#include "Weapons/TPSRevolverWeapon.h"

void ATPSRevolverWeapon::StartFire()
{
     MakeShot();
    //GetWorldTimerManager().SetTimer(ShotTimerHandle, this, &ATPSRevolverWeapon::MakeShot, TimerBetweenShot, true);
}

void ATPSRevolverWeapon::StopFire()
{
    //GetWorldTimerManager().ClearTimer(ShotTimerHandle);
}

void ATPSRevolverWeapon::MakeShot()
{

    if (!GetWorld() || IsAmmoEmpty())
    {
        StopFire();
        return;
    }
    FVector TraceStart, TraceEnd;
    if (!GetTraceData(TraceStart, TraceEnd))
        return;
    FHitResult HitResult;

    MakeHit(HitResult, TraceStart, TraceEnd);

    if (HitResult.bBlockingHit)
    {
        MakeDamage(HitResult);
        DrawDebugLine(GetWorld(), GetMuzzleWorldLocation(), HitResult.ImpactPoint, FColor::Red, false, 3.0f, 0, 3.0f);
        DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 10.0f, 24, FColor::Red, false, 5.0f);
    }
    else
    {
        DrawDebugLine(GetWorld(), GetMuzzleWorldLocation(), TraceEnd, FColor::Red, false, 3.0f, 0, 3.0f);
    }

    //DecreaseAmmo();
}
