// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "EnemyBase.h"
#include "FightingProjectCharacter.h"
#include "ANS_BlockMovementAndRotation.generated.h"

/**
 * 
 */
UCLASS()
class FIGHTINGPROJECT_API UANS_BlockMovementAndRotation : public UAnimNotifyState
{
	GENERATED_BODY()

	UPROPERTY()
	AEnemyBase* Enemy;

	UPROPERTY()
	AFightingProjectCharacter* Character;

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	FRotator RotationToKeep;
};
