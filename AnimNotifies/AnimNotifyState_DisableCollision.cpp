// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifyState_DisableCollision.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h" 
#include "Engine/CollisionProfile.h"


#include "GameFramework/CharacterMovementComponent.h"

void UAnimNotifyState_DisableCollision::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

    AActor* OwnerActor = MeshComp->GetOwner();

    if (OwnerActor)
    {
        ACharacter* Character = Cast<ACharacter>(OwnerActor);

        if (Character)
        {
            UCapsuleComponent* CapsuleComponent = Character->GetCapsuleComponent();

            if (CapsuleComponent)
            {
                OriginalSettings.CollisionEnabled = CapsuleComponent->GetCollisionEnabled();
                OriginalSettings.ResponseToChannels = CapsuleComponent->GetCollisionResponseToChannels();


                CapsuleComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
                CapsuleComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel6, ECollisionResponse::ECR_Ignore);
                CapsuleComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
                //MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            }
        }
    }
}

void UAnimNotifyState_DisableCollision::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);
}

void UAnimNotifyState_DisableCollision::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);

    AActor* OwnerActor = MeshComp->GetOwner();

    if (OwnerActor)
    {
        ACharacter* Character = Cast<ACharacter>(OwnerActor);

        if (Character)
        {
            UCapsuleComponent* CapsuleComponent = Character->GetCapsuleComponent();

            if (CapsuleComponent)
            {
                CapsuleComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
                CapsuleComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel6, ECollisionResponse::ECR_Block);
                CapsuleComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Block);


                //CapsuleComponent->SetCollisionEnabled(OriginalSettings.CollisionEnabled);
                //CapsuleComponent->SetCollisionResponseToChannels(OriginalSettings.ResponseToChannels);
                //MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

            }
        }
    }
}
