// Fill out your copyright notice in the Description page of Project Settings.


#include "ANS_SetControlRotation.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "FightingProjectCharacter.h"

void UANS_SetControlRotation::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

    AActor* OwnerActor = MeshComp->GetOwner();

    if (OwnerActor)
    {
        // Attempt to cast the owner to a character (if the owner is of character type)
        ACharacter* OwningCharacter = Cast<ACharacter>(OwnerActor);
        if (OwningCharacter)
        {
            OwningCharacter->GetCharacterMovement()->bOrientRotationToMovement = true;
            OwningCharacter->bUseControllerRotationYaw = false;
        }
    }
}

void UANS_SetControlRotation::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);

}

void UANS_SetControlRotation::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);

    AActor* OwnerActor = MeshComp->GetOwner();

    if (OwnerActor)
    {
        // Attempt to cast the owner to a character (if the owner is of character type)
        ACharacter* OwningCharacter = Cast<ACharacter>(OwnerActor);
        if (OwningCharacter)
        {
           // OwningCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
           // OwningCharacter->bUseControllerRotationYaw = true;
        }
        AFightingProjectCharacter* Player = Cast<AFightingProjectCharacter>(OwnerActor);
        if (Player)
        {
            Player->GrabAttackEnd();
        }
    }
}
