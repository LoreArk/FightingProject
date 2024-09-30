// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimNotifyState_FocusTarget.h"
#include "AIController.h"
#include "EnemyBase.h"
#include "Interface_Enemy.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "AIC_EnemyBase.h"
#include "GameFramework/CharacterMovementComponent.h"

void UAnimNotifyState_FocusTarget::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
    Super::NotifyBegin(MeshComp, Animation, TotalDuration);

    AActor* OwnerActor = MeshComp->GetOwner();

    if (OwnerActor)
    {
        if (OwnerActor->GetClass()->ImplementsInterface(UInterface_Enemy::StaticClass()))
        {
            EnemyChar = IInterface_Enemy::Execute_GetOwnerClass(OwnerActor);

            if (EnemyChar)
            {
                AAIC_EnemyBase* AI = EnemyChar->AIC;
                if (EnemyChar->AIC)
                {
                    AI->SetFocus(AI->AttackTarget);
                    EnemyChar->bUseControllerRotationYaw = true;
                }
            }
        }
    }
}

void UAnimNotifyState_FocusTarget::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    Super::NotifyEnd(MeshComp, Animation);
    
    AActor* OwnerActor = MeshComp->GetOwner();

    if (OwnerActor)
    {
        if (EnemyChar)
        {
            if (EnemyChar->AIC)
            {
                EnemyChar->AIC->SetFocus(OwnerActor);
                EnemyChar->bUseControllerRotationYaw = false;
            }
        }
    }
}