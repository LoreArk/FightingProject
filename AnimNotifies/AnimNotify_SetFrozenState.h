// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "E_DamageEnums.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_SetFrozenState.generated.h"

/**
 * 
 */
UCLASS()
class FIGHTINGPROJECT_API UAnimNotify_SetFrozenState : public UAnimNotify
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EFrozenState FrozenState;


	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;


};
