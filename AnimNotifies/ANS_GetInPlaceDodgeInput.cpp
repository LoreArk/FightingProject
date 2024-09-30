// Fill out your copyright notice in the Description page of Project Settings.


#include "ANS_GetInPlaceDodgeInput.h"

void UANS_GetInPlaceDodgeInput::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

    AActor* OwnerActor = MeshComp->GetOwner();
    if (OwnerActor && OwnerActor->GetClass()->ImplementsInterface(UInterface_Player::StaticClass()))
    {
        CachedPlayerCharacter = IInterface_Player::Execute_GetPlayerCharacter(OwnerActor);
    }
    else
    {
        CachedPlayerCharacter = nullptr;
    }

    if (CachedPlayerCharacter)
    {
        CachedPlayerCharacter->bLowAttackInputFlag = true;
    }
}

void UANS_GetInPlaceDodgeInput::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);
}

void UANS_GetInPlaceDodgeInput::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);

    if (CachedPlayerCharacter)
    {
        CachedPlayerCharacter->bLowAttackInputFlag = false;
    }
}
