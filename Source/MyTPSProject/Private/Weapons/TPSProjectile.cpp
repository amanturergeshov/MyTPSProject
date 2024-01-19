// MyTPSProject

#include "Weapons/TPSProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ATPSProjectile::ATPSProjectile()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;

    CollisionComponent = CreateDefaultSubobject<USphereComponent>("SphereComponent");
    CollisionComponent->InitSphereRadius(5.0f);
    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    CollisionComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
    SetRootComponent(CollisionComponent);

    MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovementComponent");
    MovementComponent->InitialSpeed = 2000.0f;
    MovementComponent->ProjectileGravityScale = 0.0f;
}

// Called when the game starts or when spawned
void ATPSProjectile::BeginPlay()
{
    Super::BeginPlay();

    check(MovementComponent);

    MovementComponent->Velocity = ShotDirection * MovementComponent->InitialSpeed;

    CollisionComponent->IgnoreActorWhenMoving(GetOwner(), true);

    CollisionComponent->OnComponentHit.AddDynamic(this, &ATPSProjectile ::OnProjectileHit);

    SetLifeSpan(LifeSeconds);
}

void ATPSProjectile::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (!GetWorld())
        return;
    MovementComponent->StopMovementImmediately();

    // makedamage

    UGameplayStatics::ApplyRadialDamage(GetWorld(), //
        DamageAmount,                               //
        GetActorLocation(),                         //
        DamageRadius,                               //
        nullptr,                                    //
        {GetOwner()},                               //
        this,                                       //
        GetController(),                            //
        DoFullDamage);

    DrawDebugSphere(GetWorld(), GetActorLocation(), DamageRadius, 24, FColor::Red, false, 5.0f);

    Destroy();
}

AController* ATPSProjectile::GetController() const
{
    const auto Pawn = Cast<APawn>(GetOwner());
    return Pawn ? Pawn->GetController() : nullptr;
}
