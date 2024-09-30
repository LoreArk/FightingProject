// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_EndStun.h"
#include "AIC_EnemyBase.h"
#include "EnemyBase.h"

UBTTask_EndStun::UBTTask_EndStun(FObjectInitializer const& ObjectInitializer)
{
	NodeName = "End Stun State";
}

EBTNodeResult::Type UBTTask_EndStun::ExecuteTask(UBehaviorTreeComponent& OwnerComponent, uint8* NodeMemory)
{
	

	if (AAIC_EnemyBase* const contr = Cast<AAIC_EnemyBase>(OwnerComponent.GetAIOwner()))
	{
		AEnemyBase* EnemyBase = contr->EnemyBaseRef;

		if (EnemyBase)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, ("ENEMY BASE FOUND"));
			EnemyBase->EndStunState();
		}
		else
		{

			GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, ("ENEMY BASE NOT FOUND"));
		}
	}
	

	return EBTNodeResult::Succeeded;
}
