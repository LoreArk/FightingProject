// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "F_DamageSettings.h"
#include "AnimNotify_FinisherDamage.generated.h"

/**
 * 
 */
UCLASS()
class FIGHTINGPROJECT_API UAnimNotify_FinisherDamage : public UAnimNotify
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDamageSettings DamageSettings;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;


};
