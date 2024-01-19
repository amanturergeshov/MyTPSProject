// MyTPSProject

#include "Weapons/TPSBazookaWeapon.h"
#include "Weapons/TPSProjectile.h"
#include "Kismet/GameplayStatics.h"

void ATPSBazookaWeapon::StartFire()
{
    MakeShot();
}

void ATPSBazookaWeapon::MakeShot()
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

    const FVector EndPoint = HitResult.bBlockingHit ? HitResult.ImpactPoint : TraceEnd;
    const FVector Direction = (EndPoint - GetMuzzleWorldLocation()).GetSafeNormal();

    const FTransform SpawnTransform(FRotator::ZeroRotator, GetMuzzleWorldLocation());
    ATPSProjectile* Projectile = GetWorld()->SpawnActorDeferred<ATPSProjectile>(ProjectileClass, SpawnTransform);
    if (Projectile)
    {
        Projectile->SetShotDirection(Direction);
        Projectile->SetOwner(GetOwner());
        Projectile->FinishSpawning(SpawnTransform);
    }

    DecreaseAmmo();

    // UGameplayStatics::FinishSpawningActor(Projectile, SpawnTransform);
}
