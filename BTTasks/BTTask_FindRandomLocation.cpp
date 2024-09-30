// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_FindRandomLocation.h"
#include "NavigationSystem.h"
#include "AIC_EnemyBase.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_FindRandomLocation::UBTTask_FindRandomLocation(FObjectInitializer const& ObjectInitializer)
{
	NodeName = "Find Random Location In NavMesh";
}

EBTNodeResult::Type UBTTask_FindRandomLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComponent, uint8* NodeMemory)
{
	if (AAIC_EnemyBase* const contr = Cast<AAIC_EnemyBase>(OwnerComponent.GetAIOwner()))
	{
		if (APawn* const enemy = contr->GetPawn())
		{
			FVector const Origin = enemy->GetActorLocation();

			if (auto* const NavSys = UNavigationSystemV1::GetCurrent(GetWorld()))
			{
				FNavLocation Location;
				if (NavSys->GetRandomPointInNavigableRadius(Origin, SearchRadius, Location))
				{
					OwnerComponent.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), Location.Location);

					FinishLatentTask(OwnerComponent, EBTNodeResult::Succeeded);
					return EBTNodeResult::Succeeded;
				}
			}
		}
	}

	return EBTNodeResult::Failed;
}

