// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FightingProjectCharacter.h"
#include "Sound/SoundCue.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_HoldAttack.generated.h"

/**
 * 
 */
UCLASS()
class FIGHTINGPROJECT_API UAnimNotifyState_HoldAttack : public UAnimNotifyState
{
	GENERATED_BODY()

private:

	UPROPERTY()
	AFightingProjectCharacter* CachedPlayerCharacter;

	float AnimationTotalDuration;
	float HoldStartTime;
	float HoldEndTime;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundCue* ChargeAttackSoundCue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundCue* ChargingSoundCue;

	UPROPERTY()
	bool HoldingInput;

	UPROPERTY()
	float DefaultAnimPlayRate;

	UPROPERTY()
	float HeldAnimPlayRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Settings")
	float AnimScaleDivider = 3;

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

};
