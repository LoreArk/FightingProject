// Fill out your copyright notice in the Description page of Project Settings.

#include "AIC_EnemyBase.h"

#include "EnemyBase.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Components/BoxComponent.h"
#include "Perception/AIPerceptionSystem.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "CombatManager.h"



AAIC_EnemyBase::AAIC_EnemyBase(FObjectInitializer const& ObjectInitializer)
	:Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{

	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
	SetPerceptionComponent(*AIPerceptionComponent);

	SetupPerceptionSystem();

}

void AAIC_EnemyBase::BeginPlay()
{
	Super::BeginPlay();

	if (EnemyBaseRef)
	{
		StartPosition = EnemyBaseRef->GetActorLocation();
	}
}

void AAIC_EnemyBase::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	PlayerRef = UGameplayStatics::GetActorOfClass(GetWorld(), AFightingProjectCharacter::StaticClass());
	CombatManager = Cast<ACombatManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ACombatManager::StaticClass()));

	UCrowdFollowingComponent* CrowdFollowingComponent = FindComponentByClass<UCrowdFollowingComponent>();

	if (CrowdFollowingComponent)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Black, ("CROWD COMPONENT FOUND"));
		CrowdFollowingComponent->SetCrowdSeparation(true);
		CrowdFollowingComponent->SetCrowdSeparationWeight(50.f);
		CrowdFollowingComponent->SetCrowdAvoidanceRangeMultiplier(1.1f);
		CrowdFollowingComponent->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::High, true);
	}

	
	if (AEnemyBase* const enemy = Cast<AEnemyBase>(InPawn)) 
	{
		EnemyBaseRef = enemy;
		if (UBehaviorTree* tree = enemy->GetBehviorTree()) 
		{
			UBlackboardComponent* b;
			UseBlackboard(tree->BlackboardAsset, b);
			Blackboard = b;
			RunBehaviorTree(tree);

			//SetStateAsPassive();
			
		}
	}

	if (UBlackboardComponent* BlackBoardComponent = GetBlackboardComponent())
	{
		BlackBoardComponent->SetValueAsVector("StartPosition", EnemyBaseRef->GetActorLocation());
	}
}

void AAIC_EnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (AiState == EAIStates::Attack)
	{
		GetBlackboardComponent()->SetValueAsBool("HasAvailableAttack", EnemyBaseRef->ReadyAttacks.Num() > 0);
		GetBlackboardComponent()->SetValueAsBool("Cooldown", EnemyBaseRef->bCooling);
		GetBlackboardComponent()->SetValueAsBool("TokenAvailable", EnemyBaseRef->CombatManager->CanAttack(GetOwner()));
		GetBlackboardComponent()->SetValueAsBool("IsFrozen", EnemyBaseRef->CurrentHitReaction != nullptr);
		GetBlackboardComponent()->SetValueAsBool("RangedCombat", EnemyBaseRef->bRangedCombat);
		GetBlackboardComponent()->SetValueAsBool("TargetInsideActionArea", IsTargetWithinAreaOfAction(AttackTarget));
	}
	if (AiState == EAIStates::Frozen || AiState == EAIStates::Attack)
	{
		GetBlackboardComponent()->SetValueAsBool("IsBlocking", EnemyBaseRef->bIsBlocking);
	}
	if(AiState == EAIStates::Passive)
	{
		GetBlackboardComponent()->SetValueAsBool("ReachPlayer", bReachPlayer);
	}
	
}

void AAIC_EnemyBase::SetupPerceptionSystem()
{
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	if (SightConfig)
	{
		SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component")));
		SightConfig->SightRadius = 1000.f;
		SightConfig->LoseSightRadius = SightConfig->SightRadius + 1000.f;
		SightConfig->PeripheralVisionAngleDegrees = 180.f;
		SightConfig->SetMaxAge(100.f);
		SightConfig->AutoSuccessRangeFromLastSeenLocation = 520.f;
		SightConfig->DetectionByAffiliation.bDetectEnemies = true;
		SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
		SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

		GetPerceptionComponent()->SetDominantSense(*SightConfig->GetSenseImplementation());
		GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &AAIC_EnemyBase::OnTargetDetected);
		GetPerceptionComponent()->ConfigureSense(*SightConfig);
	}
}

float AAIC_EnemyBase::SetMovementSpeed_Implementation(EMovementSpeed Speed)
{
	return IInterface_Enemy::Execute_SetMovementSpeed(GetPawn(), Speed);
}

AActor* AAIC_EnemyBase::GetAttackTarget_Implementation()
{
	return AttackTarget;
}

void AAIC_EnemyBase::Attack_Implementation()
{
	IInterface_Enemy::Execute_Attack(EnemyBaseRef);
}

void AAIC_EnemyBase::OnTargetDetected(AActor* Actor, FAIStimulus const Stimulus)
{
	if (auto* const ch = Cast<AFightingProjectCharacter>(Actor))
	{
		if (EnemyBaseRef)
		{
			if (auto EnemyHud = EnemyBaseRef->StatsHud)
			{
				EnemyHud->ToggleBarsVisibility(true);
			}
		}

		AttackTarget = Actor;
		GetBlackboardComponent()->SetValueAsObject("AttackTarget", AttackTarget);
		GetBlackboardComponent()->SetValueAsEnum("CanSeePlayer", Stimulus.WasSuccessfullySensed());
		
		SetStateAsAttacking();
	}
}

void AAIC_EnemyBase::OnDamageReceived(AActor* DamageDealer)
{
	if (AttackTarget == nullptr) 
	{
		AttackTarget = DamageDealer;
		GetBlackboardComponent()->SetValueAsObject("AttackTarget", AttackTarget);
		SetStateAsAttacking();
	}
}

void AAIC_EnemyBase::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{

}

void AAIC_EnemyBase::SetStateAsAttacking()
{
	if (AiState == EAIStates::Dead)
		return;
	OnCombatEnter.Broadcast();
	AiState = EAIStates::Attack;
	GetBlackboardComponent()->SetValueAsEnum("AiState", 1);
}

void AAIC_EnemyBase::SetStateAsFrozen()
{
	if (AiState == EAIStates::Attack)
	{
		OnCombatExit.Broadcast();
	}
	AiState = EAIStates::Frozen;
	GetBlackboardComponent()->SetValueAsEnum("AiState", 3);
}

void AAIC_EnemyBase::SetStateAsPassive()
{
	if (AiState == EAIStates::Dead)
		return;
	if (AiState == EAIStates::Attack)
	{
		OnCombatExit.Broadcast();
	}
	AiState = EAIStates::Passive;
	GetBlackboardComponent()->SetValueAsEnum("AiState", 0);
}

void AAIC_EnemyBase::SetStateAsFinisher()
{
	if (AiState == EAIStates::Dead)
		return;

	AiState = EAIStates::Finisher;
	GetBlackboardComponent()->SetValueAsEnum("AiState", 4);
}

void AAIC_EnemyBase::SetStateAsDead()
{
	if (AiState == EAIStates::Attack)
	{
		OnCombatExit.Broadcast();
	}
	AiState = EAIStates::Dead;
	GetBlackboardComponent()->SetValueAsEnum("AiState", 5);
}

void AAIC_EnemyBase::SetStateAsStun()
{
	if (AiState == EAIStates::Dead)
		return;
	if (AiState == EAIStates::Attack)
	{
		OnCombatExit.Broadcast();
	}
	AiState = EAIStates::Stun;
	GetBlackboardComponent()->SetValueAsEnum("AiState", 6);
}

void AAIC_EnemyBase::SetStateAsSurrender()
{
	if (AiState == EAIStates::Dead)
		return;
	if (AiState == EAIStates::Attack)
	{
		OnCombatExit.Broadcast();
	}
	AiState = EAIStates::Surrender;
	GetBlackboardComponent()->SetValueAsEnum("AiState", 7);
}


bool AAIC_EnemyBase::IsTargetWithinAreaOfAction(AActor* Target) const
{
	if (!Target) return false;
	if (!EnemyBaseRef->AreaOfAction) return true;

	return EnemyBaseRef->AreaOfAction->IsPointInsideArea(Target->GetActorLocation());
}




