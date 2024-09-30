// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Attack.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Interface_Enemy.h"
#include "AIC_EnemyBase.h"
#include "EnemyBase.h"


UBTTask_Attack::UBTTask_Attack(FObjectInitializer const& ObjectInitializer)
{
	NodeName = TEXT("Attack");
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComponent, uint8* NodeMemory)
{

	if (IInterface_Enemy* const interface = Cast<IInterface_Enemy>(OwnerComponent.GetAIOwner()))
	{
		IInterface_Enemy::Execute_Attack(OwnerComponent.GetAIOwner());

	}

	AAIC_EnemyBase* AIC = Cast<AAIC_EnemyBase>(OwnerComponent.GetAIOwner());
	if (AIC)
	{
		AIC->OnAttackFinishedDelegate.RemoveDynamic(this, &UBTTask_Attack::OnAttackFinished); // Ensure not double-bound
		AIC->OnAttackFinishedDelegate.AddDynamic(this, &UBTTask_Attack::OnAttackFinished);
	}

	//AIC->OnAttackFinishedDelegate.AddDynamic(this, &UBTTask_Attack::OnAttackFinished);
	//AIC->OnAttackFinishedDelegate.Ad

	return EBTNodeResult::InProgress;
}

void UBTTask_Attack::OnAttackFinished(AAIC_EnemyBase* AIC)
{
	UBehaviorTreeComponent* OwnerComp = Cast<UBehaviorTreeComponent>(GetOuter());
	if (AIC)
	{
		if (OwnerComp)
		{
			AIC->OnAttackFinishedDelegate.RemoveDynamic(this, &UBTTask_Attack::OnAttackFinished);

			FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Magenta, "FINISH TASK FAILED: owner component not found");

		}
	}

	
}
