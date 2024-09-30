// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"

#include "Interface_Enemy.h"
#include "AIC_EnemyBase.h"
#include "EnemyBase.h"
#include "BTTask_SetMovementSpeed.generated.h"

/**
 * 
 */
UCLASS()
class FIGHTINGPROJECT_API UBTTask_SetMovementSpeed : public UBTTask_BlackboardBase
{
	GENERATED_BODY()


public:
	explicit UBTTask_SetMovementSpeed(FObjectInitializer const& ObjectInitializer);

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComponent, uint8* NodeMemory) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Search", meta = (AllowPrivateAccess = "true"))
	EMovementSpeed MovementSpeed;
};
