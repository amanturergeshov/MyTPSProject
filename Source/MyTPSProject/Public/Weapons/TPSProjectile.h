// MyTPSProject

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TPSProjectile.generated.h"


class USphereComponent;

class UProjectileMovementComponent;

UCLASS()
class MYTPSPROJECT_API ATPSProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATPSProjectile();

	void SetShotDirection(const FVector& Direction) { ShotDirection = Direction; }

protected:
    UPROPERTY(VisibleDefaultsOnly, Category = "Weapon")
    USphereComponent* CollisionComponent;


    UPROPERTY(VisibleDefaultsOnly, Category = "Weapon")
    UProjectileMovementComponent* MovementComponent;

	
    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    float DamageRadius = 200.0f;

	
    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    float DamageAmount = 100.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    bool DoFullDamage = false;

    
    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    float LifeSeconds = 5.0f;



	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	private:
    FVector ShotDirection;

	UFUNCTION()
    void OnProjectileHit(UPrimitiveComponent* HitComponent,
        AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
    AController* GetController() const;
};
