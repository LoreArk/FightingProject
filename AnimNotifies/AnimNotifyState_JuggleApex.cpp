// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifyState_JuggleApex.h"
#include "Interface_Enemy.h"
#include "EnemyBase.h"

void UAnimNotifyState_JuggleApex::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
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
                EnemyChar->bJuggleApexFlag = true;
            }
        }
    }
}

void UAnimNotifyState_JuggleApex::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);

    if (EnemyChar)
    {
        EnemyChar->bJuggleApexFlag = false;
    }
}
