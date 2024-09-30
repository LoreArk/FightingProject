// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_SetMovementSpeed.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Interface_Enemy.h"
#include "AIC_EnemyBase.h"
#include "EnemyBase.h"

UBTTask_SetMovementSpeed::UBTTask_SetMovementSpeed(FObjectInitializer const& ObjectInitializer)
{
	NodeName = TEXT("Set Movement Speed");
}

EBTNodeResult::Type UBTTask_SetMovementSpeed::ExecuteTask(UBehaviorTreeComponent& OwnerComponent, uint8* NodeMemory)
{

	//MovementSpeed = OwnerComponent.GetBlackboardComponent()->GetValueAsEnum(GetSelectedBlackboardKey());

	if (IInterface_Enemy* const interface = Cast<IInterface_Enemy>(OwnerComponent.GetAIOwner()))
	{
		IInterface_Enemy::Execute_SetMovementSpeed(OwnerComponent.GetAIOwner(), MovementSpeed);
	}

	return EBTNodeResult::Succeeded;
}
