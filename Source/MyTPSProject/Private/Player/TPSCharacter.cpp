// MyTPSProject

#include "Player/TPSCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/TPSCharacterMovementComponent.h"
#include "Components/TPSHealthComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/TBSWeaponComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Controller.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ATPSCharacter::ATPSCharacter(const FObjectInitializer& ObjInit)
    : Super(
          ObjInit.SetDefaultSubobjectClass<UTPSCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need
    // it.
    PrimaryActorTick.bCanEverTick = true;
    SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
    CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
    SpringArmComponent->SetupAttachment(GetRootComponent());
    SpringArmComponent->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
    SpringArmComponent->SetRelativeLocation(FVector(0.0f, 65.0f, 60.0f));
    SpringArmComponent->TargetArmLength = 240.0f;
    CameraComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 30.0f));
    SpringArmComponent->bEnableCameraLag = true;
    SpringArmComponent->CameraLagSpeed = 15.0f;
    CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
    HealthComponent = CreateDefaultSubobject<UTPSHealthComponent>("HealthComponent");
    HealthTextComponent = CreateDefaultSubobject<UTextRenderComponent>("HealthTextComponent");
    HealthTextComponent->SetupAttachment(GetRootComponent());
    WeaponComponent = CreateDefaultSubobject<UTBSWeaponComponent>("WeaponComponent");
    SetReplicates(true);
    bReplicates = true;
    SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void ATPSCharacter::BeginPlay()
{
    Super::BeginPlay();

    check(HealthComponent);
    check(HealthTextComponent);

    OnHealthChanged(HealthComponent->GetHealth());
    HealthComponent->OnDeath.AddUObject(this, &ATPSCharacter::OnDeath);
    HealthComponent->OnHealthChanged.AddUObject(this, &ATPSCharacter::OnHealthChanged);
}

// Called every frame
void ATPSCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ATPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis("MoveForward", this, &ATPSCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &ATPSCharacter::MoveRight);
    // PlayerInputComponent->BindAxis("LookUp", this, &ATPSCharacter::AddControllerPitchInput);
    // PlayerInputComponent->BindAxis("TurnAround", this, &ATPSCharacter::AddControllerYawInput);
    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ATPSCharacter::Jump);
    PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ATPSCharacter::OnStartSprint);
    PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ATPSCharacter::OnStopSprint);
    // PlayerInputComponent->BindAction("Shoot", IE_Pressed, WeaponComponent, &UTBSWeaponComponent::StartFire);
    // PlayerInputComponent->BindAction("Shoot", IE_Released, WeaponComponent, &UTBSWeaponComponent::StopFire);
    PlayerInputComponent->BindAction("Shoot", IE_Pressed, this, &ATPSCharacter::StartCombat);
    PlayerInputComponent->BindAction("Shoot", IE_Released, this, &ATPSCharacter::StopCombat);
    PlayerInputComponent->BindAction("NextWeapon", IE_Pressed, WeaponComponent, &UTBSWeaponComponent::NextWeapon);
    PlayerInputComponent->BindAction("Reload", IE_Pressed, WeaponComponent, &UTBSWeaponComponent::Reload);
}

// bool ATPSCharacter::IsSprinting() const
//{
//     if (!HeavyWeapon())
//     {
//         return WantsToRun && isMovingForward && !GetVelocity().IsZero();
//     }
//     else
//     {
//         return false;
//     }
//
// }

bool ATPSCharacter::InCombat() const
{
    return IsFighting;
}

bool ATPSCharacter::HeavyWeapon() const
{
    return WeaponComponent->GetEquipType();
}

void ATPSCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ATPSCharacter, IsFighting);
    DOREPLIFETIME(ATPSCharacter, WantsToRun);
}

void ATPSCharacter::MoveForward(float Amount)
{
    isMovingForward = Amount > 0.0f;
    AddMovementInput(GetActorForwardVector(), Amount);
}

void ATPSCharacter::MoveRight(float Amount)
{
    AddMovementInput(GetActorRightVector(), Amount);
}

void ATPSCharacter::OnStartSprint()
{
    StopCombat();
    if (!HeavyWeapon())
    {
        // WantsToRun = isMovingForward && !GetVelocity().IsZero();
        WantsToRun = true;
    }
    else
    {
        WantsToRun = false;
    }
    if (HasAuthority())
    {
        MulticastOnStartSprint();
    }
    else
    {
        ServerOnStartSprint();
    }
}

void ATPSCharacter::OnStopSprint()
{
    WantsToRun = false;
    if (HasAuthority())
    {
        MulticastOnStopSprint();
    }
    else
    {
        ServerOnStopSprint();
    }
}
void ATPSCharacter::OnDeath()
{
    UE_LOG(LogTemp, Warning, TEXT("Plater %s is dead"), *GetName());

    // PlayAnimMontage(DeathAnimMontage);

    GetCharacterMovement()->DisableMovement();

    SetLifeSpan(5.0f);

    if (Controller)
    {
        Controller->ChangeState(NAME_Spectating);
    }
    GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    WeaponComponent->StopFire();

    GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    GetMesh()->SetSimulatePhysics(true);
}

void ATPSCharacter::OnHealthChanged(float Health)
{
    HealthTextComponent->SetText(FText::FromString(FString::Printf(TEXT("%.0f"), Health)));
}

void ATPSCharacter::StartCombat()
{
    // if (!IsSprinting())

    //if (!WantsToRun)
    //{
    //    IsFighting = true;
    //    WeaponComponent->StartFire();
    //}
    if (HasAuthority())
    {
        if (!WantsToRun)
        {
            IsFighting = true;
            WeaponComponent->StartFire();
        }
        MulticastOnStartCombat();
    }
    else
    {

        ServerOnStartCombat();
    }
}

void ATPSCharacter::StopCombat()
{
    //IsFighting = false;
    //WeaponComponent->StopFire();
    if (HasAuthority())
    {
        IsFighting = false;
        WeaponComponent->StopFire();
        MulticastOnStopCombat();
    }
    else
    {

        ServerOnStopCombat();
    }
}

//______________________________ÎÑÒÎÐÎÆÍÎ! ÄÀËÜØÅ ÐÅÏËÈÊÀÖÈß!____________________________
// StartSprint
void ATPSCharacter::ServerOnStartSprint_Implementation()
{
    MulticastOnStartSprint();
}
void ATPSCharacter::MulticastOnStartSprint_Implementation()
{
    if (!IsLocallyControlled())
    {
        StopCombat();
        if (!HeavyWeapon())
        {
            // WantsToRun = isMovingForward && !GetVelocity().IsZero();
            WantsToRun = true;
        }
        else
        {
            WantsToRun = false;
        }
    }
}
//_______________________________________________________
// StopSprint
void ATPSCharacter::ServerOnStopSprint_Implementation()
{
    MulticastOnStopSprint();
}
void ATPSCharacter::MulticastOnStopSprint_Implementation()
{
    if (!IsLocallyControlled())
    {
        WantsToRun = false;
    }
}
//_________________________________________________
// StartCombat
void ATPSCharacter::ServerOnStartCombat_Implementation()
{
    MulticastOnStartCombat();
}
void ATPSCharacter::MulticastOnStartCombat_Implementation()
{
    if (!IsLocallyControlled())
    {
        if (!WantsToRun)
        {
            IsFighting = true;
            WeaponComponent->StartFire();
        }
    }
}
//_______________________________________________________
// StopCombat
void ATPSCharacter::ServerOnStopCombat_Implementation()
{
    MulticastOnStopCombat();
}
void ATPSCharacter::MulticastOnStopCombat_Implementation()
{
    if (!IsLocallyControlled())
    {
        UE_LOG(LogTemp, Warning, TEXT("BUM!"))
        IsFighting = false;
        WeaponComponent->StopFire();
    }
}
//_______________________________________________________