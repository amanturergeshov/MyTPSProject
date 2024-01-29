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
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

ATPSCharacter::ATPSCharacter(const FObjectInitializer& ObjInit)
    : Super(
          ObjInit.SetDefaultSubobjectClass<UTPSCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{

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

    PrimaryActorTick.bCanEverTick = true;
    SetReplicates(true);
    bReplicates = true;
    SetReplicateMovement(true);
}

void ATPSCharacter::BeginPlay()
{
    Super::BeginPlay();

    //---------------------New Input System-------------------------------------------------
    if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
                ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(SlashContext, 0);
        }
    }
    //______________________________________________________________________________________

    check(HealthComponent);
    check(HealthTextComponent);

    OnHealthChanged(HealthComponent->GetHealth());
    HealthComponent->OnDeath.AddUObject(this, &ATPSCharacter::OnDeath);
    HealthComponent->OnHealthChanged.AddUObject(this, &ATPSCharacter::OnHealthChanged);
}


void ATPSCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}


void ATPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    //_________________________NEW INPUT SYSTEM_____________________________________________

    if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Triggered, this, &ATPSCharacter::Move);
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ATPSCharacter::Jump);
        EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &ATPSCharacter::OnStartSprint);
        EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ATPSCharacter::OnStopSprint);
        EnhancedInputComponent->BindAction(ChangeWeaponAction, ETriggerEvent::Started, WeaponComponent, &UTBSWeaponComponent::NextWeapon);
        EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, WeaponComponent, &UTBSWeaponComponent::Reload);
        EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Started, this, &ATPSCharacter::StartCombat);
        EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Completed, this, &ATPSCharacter::StopCombat);
    }
    //_________________________OLD INPUT SYSTEM_____________________________________________
    //PlayerInputComponent->BindAxis("MoveForward", this, &ATPSCharacter::MoveForward);
    //PlayerInputComponent->BindAxis("MoveRight", this, &ATPSCharacter::MoveRight);
    //PlayerInputComponent->BindAxis("LookUp", this, &ATPSCharacter::AddControllerPitchInput);
    //PlayerInputComponent->BindAxis("TurnAround", this, &ATPSCharacter::AddControllerYawInput);
    //PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ATPSCharacter::Jump);
    //PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ATPSCharacter::OnStartSprint);
    //PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ATPSCharacter::OnStopSprint);
    //PlayerInputComponent->BindAction("Shoot", IE_Pressed, WeaponComponent, &UTBSWeaponComponent::StartFire);
    //PlayerInputComponent->BindAction("Shoot", IE_Released, WeaponComponent, &UTBSWeaponComponent::StopFire);
    //PlayerInputComponent->BindAction("Shoot", IE_Pressed, this, &ATPSCharacter::StartCombat);
    //PlayerInputComponent->BindAction("Shoot", IE_Released, this, &ATPSCharacter::StopCombat);
    //PlayerInputComponent->BindAction("NextWeapon", IE_Pressed, WeaponComponent, &UTBSWeaponComponent::NextWeapon);
    //PlayerInputComponent->BindAction("Reload", IE_Pressed, WeaponComponent, &UTBSWeaponComponent::Reload);
}

//-------------------------------------------------MOVEMENT-----------------------------------------------

void ATPSCharacter::Move(const FInputActionValue& Value)
{
    const FVector2D MovementVector = Value.Get<FVector2D>();
    MoveForward(MovementVector.Y);
    MoveRight(MovementVector.X);
}

void ATPSCharacter::SetSprinting()
{
    if (!HeavyWeapon())
    {
        IsSprinting = WantsToRun && isMovingForward && !GetVelocity().IsZero();
    }
    else
    {
        IsSprinting = false;
    }
    if (HasAuthority())
    {
        MulticastSetSprinting();
    }
    else
    {
        ServerSetSprinting();
    }
}

bool ATPSCharacter::HeavyWeapon() const
{
    return WeaponComponent->GetEquipType();
}

void ATPSCharacter::MoveForward(float Amount)
{
    isMovingForward = Amount > 0.0f;
    AddMovementInput(GetActorForwardVector(), Amount);
    if (HasAuthority())
    {
        MulticastMoveForward(Amount);
    }
    else
    {
        ServerMoveForward(Amount);
    }
}

void ATPSCharacter::MoveRight(float Amount)
{
    SetSprinting();
    AddMovementInput(GetActorRightVector(), Amount);
}

void ATPSCharacter::OnStartSprint()
{
    StopCombat();
    WantsToRun = true;
    SetSprinting();
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
    SetSprinting();
    if (HasAuthority())
    {
        MulticastOnStopSprint();
    }
    else
    {
        ServerOnStopSprint();
    }
}


//-------------------------------------------------HEALTH-----------------------------------------------
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


//-------------------------------------------------COMBAT-----------------------------------------------

void ATPSCharacter::StartCombat()
{
    if (!WantsToRun)
    {
        IsFighting = true;
        WeaponComponent->StartFire();
    }
    if (HasAuthority())
    {
        MulticastOnStartCombat();
    }
    else
    {

        ServerOnStartCombat();
    }
}

void ATPSCharacter::StopCombat()
{
    IsFighting = false;
    WeaponComponent->StopFire();
    if (HasAuthority())
    {
        MulticastOnStopCombat();
    }
    else
    {

        ServerOnStopCombat();
    }
}

//______________________________OSTOROJNO! DALSHE REPLICATION!!____________________________

void ATPSCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ATPSCharacter, IsFighting);
    DOREPLIFETIME(ATPSCharacter, WantsToRun);
    DOREPLIFETIME(ATPSCharacter, isMovingForward);
    DOREPLIFETIME(ATPSCharacter, IsSprinting);
}
//________________________________________________
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
        WantsToRun = true;
        SetSprinting();
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
        SetSprinting();
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
        if (!IsSprinting)
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
        IsFighting = false;
        WeaponComponent->StopFire();
    }
}
//_______________________________________________________
// StopCombat
void ATPSCharacter::ServerMoveForward_Implementation(float Amount)
{
    MulticastMoveForward(Amount);
}
void ATPSCharacter::MulticastMoveForward_Implementation(float Amount)
{
    if (!IsLocallyControlled())
    {
        isMovingForward = Amount > 0.0f;
        AddMovementInput(GetActorForwardVector(), Amount);
    }
}
//_______________________________________________________

void ATPSCharacter::ServerSetSprinting_Implementation()
{
    MulticastSetSprinting();
}

void ATPSCharacter::MulticastSetSprinting_Implementation()
{
    if (!IsLocallyControlled())
    {
        if (!HeavyWeapon())
        {
            IsSprinting = WantsToRun && isMovingForward && !GetVelocity().IsZero();
        }
        else
        {
            IsSprinting = false;
            SetSprinting();
        }
    }
}