// Fill out your copyright notice in the Description page of Project Settings.


#include "ANS_BlockMovementAndRotation.h"
#include "AIController.h"
#include "EnemyBase.h"
#include "GameFramework/Actor.h"
#include "Interface_Player.h"
#include "Interface_Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

void UANS_BlockMovementAndRotation::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
    AActor* Owner = MeshComp->GetOwner();
    if (Owner)
    {
        if (Owner->GetClass()->ImplementsInterface(UInterface_Enemy::StaticClass()))
        {
            Enemy = IInterface_Enemy::Execute_GetOwnerClass(Owner);
            //Enemy->AllowRotation(false);
            //RotationToKeep = Enemy->GetActorRotation();
        }
        else if (Owner->GetClass()->ImplementsInterface(UInterface_Player::StaticClass()))
        {
            Character = IInterface_Player::Execute_GetPlayerCharacter(Owner);
            //Character->GetCharacterMovement()->SetMovementMode(MOVE_None);
        }
    }
    
    
}

void UANS_BlockMovementAndRotation::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);

    if (Enemy)
    {
        //Enemy->BlockMovementAndRotation();
        //Enemy->SetActorRotation(RotationToKeep);
    }
    /*if (Character)
    {
        Character->GetCharacterMovement()->Velocity = FVector::ZeroVector;
    }*/
}

void UANS_BlockMovementAndRotation::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    
}
