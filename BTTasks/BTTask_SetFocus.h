// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_SetFocus.generated.h"

/**
 * 
 */
UCLASS()
class FIGHTINGPROJECT_API UBTTask_SetFocus : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	explicit UBTTask_SetFocus(FObjectInitializer const& ObjectInitializer);

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComponent, uint8* NodeMemory) override;

};
