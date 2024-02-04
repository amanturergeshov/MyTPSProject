// MyTPSProject

#include "Components/TBSWeaponComponent.h"
#include "Weapons/TPSPistolWeapon.h"
#include "GameFramework/Character.h"
#include "Animations/TPSReloadAnimNotify.h"
#include "Net/UnrealNetwork.h"
UTBSWeaponComponent::UTBSWeaponComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

}

void UTBSWeaponComponent::BeginPlay()
{
    Super::BeginPlay();
    CurrentWeaponIndex = 0;
    InitAnimations();
    SpawnWeapons();
    EquipWeapon(CurrentWeaponIndex);
    // ...
}

void UTBSWeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    CurrentWeapon = nullptr;
    for (auto Weapon : Weapons)
    {
        Weapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
        Weapon->Destroy();
    }
    Weapons.Empty();
    Super::EndPlay(EndPlayReason);
}

//_________________________________________________WEAPON_______________________________________________________
void UTBSWeaponComponent::SpawnWeapons()
{

    ACharacter* Character = Cast<ACharacter>(GetOwner());

    if (!Character || !GetWorld())
        return;

    for (auto OneWeaponData : WeaponData)
    {
        auto Weapon = GetWorld()->SpawnActor<ATPSPistolWeapon>(OneWeaponData.WeaponClass);
        if (!Weapon)
            continue;
        Weapon->OnClipEmpty.AddUObject(this, &UTBSWeaponComponent::ChangeClip);

        Weapon->SetOwner(Character);
        Weapons.Add(Weapon);
        if (Weapon->GetWeaponHeavy())
        {
            AttachWeaponToSocket(Weapon, Character->GetMesh(), HeavyWeaponArmorySocketName);
            PlayAnimMontage(HeavyEquipAnimMontage);
        }
        else
        {
            AttachWeaponToSocket(Weapon, Character->GetMesh(), WeaponArmorySocketName);
            PlayAnimMontage(EquipAnimMontage);
        }
    }
}

void UTBSWeaponComponent::AttachWeaponToSocket(
    ATPSPistolWeapon* Weapon, USceneComponent* SceneComponent, const FName& SocketName)
{
    if (!Weapon || !SceneComponent)
        return;
    FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, false);
    Weapon->AttachToComponent(SceneComponent, AttachmentRules, SocketName);
}

void UTBSWeaponComponent::NextWeapon()
{
    CurrentWeaponIndex = (CurrentWeaponIndex + 1) % Weapons.Num();
    EquipWeapon(CurrentWeaponIndex);
}

void UTBSWeaponComponent::EquipWeapon(int32 WeaponIndex)
{
    ReloadAnimInProgress = false;
    if (WeaponIndex < 0 || WeaponIndex >= Weapons.Num())
    {
        return;
    }
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if (!Character)
        return;

    if (CurrentWeapon)
    {
        CurrentWeapon->StopFire();
        if (CurrentWeapon->GetWeaponHeavy())
        {
            AttachWeaponToSocket(CurrentWeapon, Character->GetMesh(), HeavyWeaponArmorySocketName);
            PlayAnimMontage(HeavyEquipAnimMontage);
        }
        else
        {
            AttachWeaponToSocket(CurrentWeapon, Character->GetMesh(), WeaponArmorySocketName);
            PlayAnimMontage(EquipAnimMontage);
        }
    }

    CurrentWeapon = Weapons[WeaponIndex];
    // CurrentWeaponAnimMontage = WeaponData[WeaponIndex].ReloadAnimMontage;
    const auto CurrentWeaponData = WeaponData.FindByPredicate([&](const FWeaponData& Data) { //
        return Data.WeaponClass == CurrentWeapon->GetClass();                                //
    });

    CurrenReloadAnimMontage = CurrentWeaponData ? CurrentWeaponData->ReloadAnimMontage : nullptr;

    if (CurrentWeapon->GetWeaponHeavy())
    {
        AttachWeaponToSocket(CurrentWeapon, Character->GetMesh(), HeavyWeaponArmorySocketName);
        PlayAnimMontage(HeavyEquipAnimMontage);
    }
    else
    {
        AttachWeaponToSocket(CurrentWeapon, Character->GetMesh(), WeaponArmorySocketName);
        PlayAnimMontage(EquipAnimMontage);
    }
}


//_________________________________________________RELOAD_______________________________________________________
void UTBSWeaponComponent::OnReloadFinished(USkeletalMeshComponent* MeshComp)
{
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if (!Character || MeshComp != Character->GetMesh())
        return;
    ReloadAnimInProgress = false;
}


void UTBSWeaponComponent::ChangeClip()
{
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if (!CanReload())
        return;
    StopFire();
    CurrentWeapon->ChangeClip();
    ReloadAnimInProgress = true;
    PlayAnimMontage(CurrenReloadAnimMontage);
    if (Character->HasAuthority())
    {
        MulticastChangeClip();
    }
    else
    {
        ServerChangeClip();
    }
}
//_________________________________________________ANIMATION_______________________________________________________
void UTBSWeaponComponent::PlayAnimMontage(UAnimMontage* Animation)
{
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if (!Character)
        return;
    Character->PlayAnimMontage(Animation);
    if (Character->HasAuthority())
    {
        MulticastPlayAnimation(Animation);
    }
    else
    {
        ServerPlayAnimation(Animation);
    }
}
void UTBSWeaponComponent::InitAnimations()
{

    for (auto OneWeaponData : WeaponData)
    {
        auto ReloadFinishedNotify = FindNotifyByClass<UTPSReloadAnimNotify>(OneWeaponData.ReloadAnimMontage);
        if (!ReloadFinishedNotify)
            continue;

        ReloadFinishedNotify->OnNotified.AddUObject(this, &UTBSWeaponComponent::OnReloadFinished);
    }
}


//_________________________________________________COMBAT_______________________________________________________
void UTBSWeaponComponent::StartFire()
{
    if (!CanFire())
        return;
    CurrentWeapon->StartFire();
}

void UTBSWeaponComponent::StopFire()
{
    if (!CurrentWeapon)
        return;
    CurrentWeapon->StopFire();
}

//_________________________________________________WEAPON INFO_______________________________________________________
bool UTBSWeaponComponent::CanFire() const
{
    return CurrentWeapon && !ReloadAnimInProgress;
}

bool UTBSWeaponComponent::CanReload() const
{
    return CurrentWeapon && !ReloadAnimInProgress && CurrentWeapon->CanReload();
}
bool UTBSWeaponComponent::GetEquipType()
{
    if (!CurrentWeapon)
        return false;
    return CurrentWeapon->GetWeaponHeavy();
}

int32 UTBSWeaponComponent::GetCurrentBullets()
{
    if (!CurrentWeapon)
        return 0;
    return CurrentWeapon->GetBullets();
}

int32 UTBSWeaponComponent::GetCurrentClips()
{
    if (!CurrentWeapon)
        return 0;
    return CurrentWeapon->GetClips();
}

//_____________________________________REPLICATION!__________________________________


void UTBSWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const 
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UTBSWeaponComponent, CurrenReloadAnimMontage);
}


//_____________________________________RPCs__________________________________
//Reload

void UTBSWeaponComponent::ServerChangeClip_Implementation()
{
    MulticastChangeClip();
}

void UTBSWeaponComponent::MulticastChangeClip_Implementation() {
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if (Character->IsLocallyControlled())
    {
        return;
    }
    else
    {
        if (!CanReload())
            return;
        StopFire();
        CurrentWeapon->ChangeClip();
        ReloadAnimInProgress = true;
        PlayAnimMontage(CurrenReloadAnimMontage);
    }
    
}

//__________________________________________________________________________________________________________________

void UTBSWeaponComponent::ServerPlayAnimation_Implementation(UAnimMontage* Animation)
{
    MulticastPlayAnimation(Animation);
}

void UTBSWeaponComponent::MulticastPlayAnimation_Implementation(UAnimMontage* Animation)
{
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if (Character->IsLocallyControlled())
    {
        return;
    }
    else
    {
        if (!Character)
            return;
        Character->PlayAnimMontage(Animation);
    }
}
