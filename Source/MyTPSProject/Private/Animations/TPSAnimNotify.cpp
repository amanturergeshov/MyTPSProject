// MyTPSProject


#include "Animations/TPSAnimNotify.h"

void UTPSAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    OnNotified.Broadcast(MeshComp);
    Super::Notify(MeshComp, Animation);

}
