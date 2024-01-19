// MyTPSProject

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TPSCharacterMovementComponent.generated.h"

/**
 *
 */
UCLASS()
class MYTPSPROJECT_API UTPSCharacterMovementComponent : public UCharacterMovementComponent
{
    GENERATED_BODY()

public:
    virtual float GetMaxSpeed() const override;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
    float Modifier = 400;
};
