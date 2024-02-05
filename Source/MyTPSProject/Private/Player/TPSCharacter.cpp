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
    //HealthTextComponent = CreateDefaultSubobject<UTextRenderComponent>("HealthTextComponent");
    //HealthTextComponent->SetupAttachment(GetRootComponent());
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
    HealthComponent->OnDeath.AddUObject(this, &ATPSCharacter::OnDeath);


    //check(HealthTextComponent);
    //OnHealthChanged(HealthComponent->GetHealth());
    //HealthComponent->OnHealthChanged.AddUObject(this, &ATPSCharacter::OnHealthChanged);
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
        EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &ATPSCharacter::OnStartSprint);
        EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ATPSCharacter::OnStopSprint);
        EnhancedInputComponent->BindAction(
            ChangeWeaponAction, ETriggerEvent::Started, this, &ATPSCharacter::NextWeapon);
        EnhancedInputComponent->BindAction(
            ReloadAction, ETriggerEvent::Started, WeaponComponent, &UTBSWeaponComponent::ChangeClip);
        EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Started, this, &ATPSCharacter::StartCombat);
        EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Completed, this, &ATPSCharacter::StopCombat);
    }
    //_________________________OLD INPUT SYSTEM_____________________________________________
    // PlayerInputComponent->BindAxis("MoveForward", this, &ATPSCharacter::MoveForward);
    // PlayerInputComponent->BindAxis("MoveRight", this, &ATPSCharacter::MoveRight);
    // PlayerInputComponent->BindAxis("LookUp", this, &ATPSCharacter::AddControllerPitchInput);
    // PlayerInputComponent->BindAxis("TurnAround", this, &ATPSCharacter::AddControllerYawInput);
    // PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ATPSCharacter::Jump);
    // PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ATPSCharacter::OnStartSprint);
    // PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ATPSCharacter::OnStopSprint);
    // PlayerInputComponent->BindAction("Shoot", IE_Pressed, WeaponComponent, &UTBSWeaponComponent::StartFire);
    // PlayerInputComponent->BindAction("Shoot", IE_Released, WeaponComponent, &UTBSWeaponComponent::StopFire);
    // PlayerInputComponent->BindAction("Shoot", IE_Pressed, this, &ATPSCharacter::StartCombat);
    // PlayerInputComponent->BindAction("Shoot", IE_Released, this, &ATPSCharacter::StopCombat);
    // PlayerInputComponent->BindAction("NextWeapon", IE_Pressed, WeaponComponent, &UTBSWeaponComponent::NextWeapon);
    // PlayerInputComponent->BindAction("Reload", IE_Pressed, WeaponComponent, &UTBSWeaponComponent::Reload);
}

//-------------------------------------------------MOVEMENT-----------------------------------------------

void ATPSCharacter::Move(const FInputActionValue& Value)
{
    const FVector2D MovementVector = Value.Get<FVector2D>();
    MoveForward(MovementVector.Y);
    MoveRight(MovementVector.X);
}

bool ATPSCharacter::HeavyWeapon() const
{
    return WeaponComponent->GetEquipType();
}

void ATPSCharacter::MoveForward(float Amount)
{
    AddMovementInput(GetActorForwardVector(), Amount);
    ServerMoveForward(Amount);
}

void ATPSCharacter::MoveRight(float Amount)
{
    AddMovementInput(GetActorRightVector(), Amount);
}

//-------------------------------------------------HEALTH-----------------------------------------------
void ATPSCharacter::OnDeath()
{
    UE_LOG(LogTemp, Warning, TEXT("Plater %s is dead"), *GetName());

    // PlayAnimMontage(DeathAnimMontage);

    GetCharacterMovement()->DisableMovement();

    SetLifeSpan(5.0f);

    /*if (Controller)
    {
        Controller->ChangeState(NAME_Spectating);
    }*/
    GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    WeaponComponent->StopFire();

    GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    GetMesh()->SetSimulatePhysics(true);

    OnDeathForRespawn.Broadcast();
}
//
//void ATPSCharacter::OnHealthChanged(float Health)
//{
//    HealthTextComponent->SetText(FText::FromString(FString::Printf(TEXT("%.0f"), Health)));
//}

//-------------------------------------------------COMBAT-----------------------------------------------

void ATPSCharacter::StartCombat_Implementation()
{
    if (!WantsToRun)
    {
        if (HeavyWeapon() || IsFighting)
        {
            ServerOnStartCombat();
        }
        else
        {
            IsFighting = true;
            GetWorldTimerManager().SetTimer(
                AttackTimerHandle, this, &ATPSCharacter::ServerOnStartCombat, TimerBetweenShot, false);
        }
    }
}

void ATPSCharacter::StopCombat_Implementation()
{
    if (InZoom==false)
    {
        if (!HeavyWeapon())
        {
            GetWorldTimerManager().SetTimer(
                StopAttackTimerHandle, this, &ATPSCharacter::ServerOnStopCombat, TimerAfterShot, false);
        }
        else
        {
            IsFighting = false;
        }
    }
}
//_________________________________NEXT WEAPON____________________________________

void ATPSCharacter::NextWeapon()
{
    if (WeaponComponent)
    {
        WeaponComponent->NextWeapon();
    }

    if (HasAuthority())
    {
        MulticastNextWeapon();
    }
    else
    {
        ServerNextWeapon();
    }
}

//______________________________OSTOROJNO! DALSHE RPCs!!____________________________

void ATPSCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ATPSCharacter, IsFighting);
    DOREPLIFETIME(ATPSCharacter, WantsToRun);
    DOREPLIFETIME(ATPSCharacter, isMovingForward);
}
//________________________________________________
// Sprint
void ATPSCharacter::OnStartSprint_Implementation()
{
    IsFighting = false;
    if (!HeavyWeapon())
    {
        WantsToRun = isMovingForward && !GetVelocity().IsZero();
    }
}

void ATPSCharacter::OnStopSprint_Implementation()
{
    WantsToRun = false;
}
//_________________________________________________
// StartCombat
void ATPSCharacter::ServerOnStartCombat_Implementation()
{
    WeaponComponent->StartFire();
    MulticastOnStartCombat();
}
void ATPSCharacter::MulticastOnStartCombat_Implementation()
{
    WeaponComponent->DecreaseAmmo();
}
//_______________________________________________________
// StopCombat
void ATPSCharacter::ServerOnStopCombat_Implementation()
{
    IsFighting = false;
    WeaponComponent->StopFire();
}

void ATPSCharacter::ServerMoveForward_Implementation(float Amount)
{
    isMovingForward = Amount > 0.0f;
}

//__________________________________________________________
// NextWeapon

void ATPSCharacter::ServerNextWeapon_Implementation()
{
    MulticastNextWeapon();
}

void ATPSCharacter::MulticastNextWeapon_Implementation()
{
    if (!IsLocallyControlled())
    {
        if (WeaponComponent)
        {
            WeaponComponent->NextWeapon();
        }
    }
}