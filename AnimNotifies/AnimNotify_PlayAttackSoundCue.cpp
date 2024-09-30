// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_PlayAttackSoundCue.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"


void UAnimNotify_PlayAttackSoundCue::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    if (AttackSoundCue && MeshComp)
    {
        UGameplayStatics::PlaySoundAtLocation(MeshComp->GetWorld(), AttackSoundCue, MeshComp->GetComponentLocation());
    }
}
