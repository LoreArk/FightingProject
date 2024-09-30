// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_ClearFocus.h"
#include "AIC_EnemyBase.h"


UBTTask_ClearFocus::UBTTask_ClearFocus(FObjectInitializer const& ObjectInitializer)
{
	NodeName = TEXT("Clear Focus");

}

EBTNodeResult::Type UBTTask_ClearFocus::ExecuteTask(UBehaviorTreeComponent& OwnerComponent, uint8* NodeMemory)
{

	
	OwnerComponent.GetAIOwner()->ClearFocus(EAIFocusPriority::Default);

	

	return EBTNodeResult::Succeeded;
}
