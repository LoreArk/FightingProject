// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "F_DamageSettings.h"
#include "F_HitTraceSettings.h"
#include "AttackTraceNotifyState.generated.h"

/**
 * 
 */
UCLASS()
class FIGHTINGPROJECT_API UAttackTraceNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDamageSettings DamageSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	F_HitTraceSettings HitTraceSettings;
};
