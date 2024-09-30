
#include "AnimNotifyStateSaveAttack.h"
#include "FightingProject/FightingProjectCharacter.h"

//This anim notify state handles the transition between the current attack montage and an attack montage saved by the input buffering system, 
//the frames of this notify state are the frames in which the saved attack is blended with the current attack


void UAnimNotifyStateSaveAttack::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
    Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);

    if (MeshComp->GetOwner() == nullptr)
        return;

    if (AFightingProjectCharacter* Character = Cast<AFightingProjectCharacter>(MeshComp->GetOwner()))
    {
        Character->HandleAttackCombo(FrameDeltaTime);
    }
}
