// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_SetFocus.h"
#include "AIC_EnemyBase.h"

UBTTask_SetFocus::UBTTask_SetFocus(FObjectInitializer const& ObjectInitializer)
{
	NodeName = TEXT("Set Focus");
}

EBTNodeResult::Type UBTTask_SetFocus::ExecuteTask(UBehaviorTreeComponent& OwnerComponent, uint8* NodeMemory)
{

	AActor* AttackTarget = IInterface_Enemy::Execute_GetAttackTarget(OwnerComponent.GetAIOwner());
	if (AttackTarget != nullptr)
	{
		OwnerComponent.GetAIOwner()->SetFocus(AttackTarget);
		//OwnerComponent.GetAIOwner()->ClearFocus(EAIFocusPriority::Default);

	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, TEXT("ATTACK TARGET NULL"));
	}

	return EBTNodeResult::Succeeded;
}
