// MyTPSProject

#include "Weapons/TPSPistolWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Character.h"
#include "GameFramework/Controller.h"
#include "Net/UnrealNetwork.h"

ATPSPistolWeapon::ATPSPistolWeapon()
{

    PrimaryActorTick.bCanEverTick = false;

    // WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh");
    // SetRootComponent(WeaponMesh);

    WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>("WeaponMesh");
    SetRootComponent(WeaponMesh);
    SetReplicates(true);
}

void ATPSPistolWeapon::BeginPlay()
{
    Super::BeginPlay();
    check(WeaponMesh);
    CurrentAmmo = DefaultAmmo;
}

void ATPSPistolWeapon::StartFire() {}

void ATPSPistolWeapon::StopFire() {}

bool ATPSPistolWeapon::GetWeaponHeavy()
{
    return HeavyWeapon;
}

void ATPSPistolWeapon::MakeShot() {}

APlayerController* ATPSPistolWeapon::GetPlayerController() const
{
    const auto Player = Cast<ACharacter>(GetOwner());
    if (!Player)
        return nullptr;

    return Player->GetController<APlayerController>();
}

bool ATPSPistolWeapon::GetPlayerViewPoint(FVector& ViewLocation, FRotator& ViewRotation) const
{
    const auto Controller = GetPlayerController();
    if (!Controller)
        return false;

    Controller->GetPlayerViewPoint(ViewLocation, ViewRotation);
    return true;
}

bool ATPSPistolWeapon::GetTraceData(FVector& TraceStart, FVector& TraceEnd) const
{
    FVector ViewLocation;
    FRotator ViewRotation;
    if (!GetPlayerViewPoint(ViewLocation, ViewRotation))
        return false;
    TraceStart = ViewLocation;
    const FVector ShootDirection = ViewRotation.Vector();
    TraceEnd = TraceStart + ShootDirection * TraceMaxDistance;
    return true;
}

FVector ATPSPistolWeapon::GetMuzzleWorldLocation() const
{
    return WeaponMesh->GetSocketLocation(MuzzleSocketName);
}

void ATPSPistolWeapon::MakeHit(FHitResult& HitResult, const FVector& TraceStart, const FVector& TraceEnd)
{
    if (!GetWorld())
        return;
    FCollisionQueryParams CollisionParams;
    CollisionParams.AddIgnoredActor(GetOwner());
    GetWorld()->LineTraceSingleByChannel(
        HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility, CollisionParams);
}

void ATPSPistolWeapon::MakeDamage(const FHitResult& HitResult)
{
    const auto DamagedActor = HitResult.GetActor();
    if (!DamagedActor)
        return;
    DamagedActor->TakeDamage(DamageAmount, FDamageEvent(), GetPlayerController(), this);
}

void ATPSPistolWeapon::DecreaseAmmo()
{

    CurrentAmmo.Bullets--;
    LogAmmo();
    if (IsClipEmpty() && !IsAmmoEmpty())
    {
        OnClipEmpty.Broadcast();
    }
}

bool ATPSPistolWeapon::IsAmmoEmpty() const
{
    return !CurrentAmmo.Infinite && CurrentAmmo.Clips == 0 && IsClipEmpty();
}

bool ATPSPistolWeapon::IsClipEmpty() const
{
    return CurrentAmmo.Bullets == 0;
}

void ATPSPistolWeapon::ChangeClip()
{
    if (!CurrentAmmo.Infinite)
    {
        if (CurrentAmmo.Clips == 0)
            return;
        CurrentAmmo.Clips--;
    }
    CurrentAmmo.Bullets = DefaultAmmo.Bullets;
    UE_LOG(LogTemp, Error, TEXT("RELOAD!"));
}

bool ATPSPistolWeapon::CanReload() const
{
    return CurrentAmmo.Bullets < DefaultAmmo.Bullets && CurrentAmmo.Clips > 0;
}

int32 ATPSPistolWeapon::GetBullets()
{
    return CurrentAmmo.Bullets;
}

int32 ATPSPistolWeapon::GetClips()
{
    return CurrentAmmo.Clips;
}

void ATPSPistolWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ATPSPistolWeapon, HeavyWeapon)
}

void ATPSPistolWeapon::LogAmmo()
{
    FString AmmoInfo = "Ammo: " + FString::FromInt(CurrentAmmo.Bullets) + "/";
    AmmoInfo += FString::FromInt(CurrentAmmo.Clips);
    UE_LOG(LogTemp, Warning, TEXT("%s"), *AmmoInfo);
}
