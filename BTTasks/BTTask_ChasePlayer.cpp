// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_ChasePlayer.h"
#include "AIC_EnemyBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"


UBTTask_ChasePlayer::UBTTask_ChasePlayer(FObjectInitializer const& ObjectInitializer)
{
	NodeName = TEXT("Chase Player");
}

EBTNodeResult::Type UBTTask_ChasePlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComponent, uint8* NodeMemory)
{
	if (auto* const cont = Cast<AAIC_EnemyBase>(OwnerComponent.GetAIOwner()))
	{
		auto const PlayerLocation = OwnerComponent.GetBlackboardComponent()->GetValueAsVector(GetSelectedBlackboardKey());
	
		//UAIBlueprintHelperLibrary::SimpleMoveToLocation(cont, PlayerLocation);

		cont->MoveToLocation(PlayerLocation, AcceptanceRadius, false, true, true, true);

		FinishLatentTask(OwnerComponent, EBTNodeResult::Succeeded);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
