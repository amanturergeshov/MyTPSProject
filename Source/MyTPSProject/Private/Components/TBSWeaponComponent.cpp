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
            AttachWeaponToSocket(CurrentWeapon, Character->GetMesh(), HeavyWeaponArmorySocketName);
            PlayAnimMontage(HeavyEquipAnimMontage);
        }
        else
        {
            AttachWeaponToSocket(CurrentWeapon, Character->GetMesh(), WeaponArmorySocketName);
            PlayAnimMontage(EquipAnimMontage);
        }
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

void UTBSWeaponComponent::AttachWeaponToSocket(
    ATPSPistolWeapon* Weapon, USceneComponent* SceneComponent, const FName& SocketName)
{
    if (!Weapon || !SceneComponent)
        return;
    FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, false);
    Weapon->AttachToComponent(SceneComponent, AttachmentRules, SocketName);
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

void UTBSWeaponComponent::PlayAnimMontage(UAnimMontage* Animation)
{
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if (!Character)
        return;
    Character->PlayAnimMontage(Animation);
}

void UTBSWeaponComponent::OnReloadFinished(USkeletalMeshComponent* MeshComp)
{
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if (!Character || MeshComp != Character->GetMesh())
        return;
    ReloadAnimInProgress = false;
}

bool UTBSWeaponComponent::CanFire() const
{
    return CurrentWeapon && !ReloadAnimInProgress;
}

bool UTBSWeaponComponent::CanReload() const
{
    return CurrentWeapon && !ReloadAnimInProgress && CurrentWeapon->CanReload();
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
}

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

void UTBSWeaponComponent::NextWeapon()
{
    CurrentWeaponIndex = (CurrentWeaponIndex + 1) % Weapons.Num();
    EquipWeapon(CurrentWeaponIndex);
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

//_____________________________________����������!__________________________________


void UTBSWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const 
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UTBSWeaponComponent, CurrenReloadAnimMontage);
}

