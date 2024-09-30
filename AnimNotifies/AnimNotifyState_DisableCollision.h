// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_DisableCollision.generated.h"

/**
 * 
 */
UCLASS()
class FIGHTINGPROJECT_API UAnimNotifyState_DisableCollision : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

private:
    struct FCapsuleCollisionSettings
    {
        TEnumAsByte<ECollisionEnabled::Type> CollisionEnabled;
        FCollisionResponseContainer ResponseToChannels;
    };

    FCapsuleCollisionSettings OriginalSettings;
};
