// Fill out your copyright notice in the Description page of Project Settings.


#include "Component_Finishers.h"
#include "Kismet/KismetMathLibrary.h"
#include "StaticFunctions.h"
#include "FightingProject/FightingProjectCharacter.h"
#

// Sets default values for this component's properties
UComponent_Finishers::UComponent_Finishers()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UComponent_Finishers::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UComponent_Finishers::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bFinishable)
	{
		AActor* PossibleAttacker = FinisherVulnerabilityTrace();
		if (PossibleAttacker == nullptr && Attacker != nullptr)
		{
			IInterface_Finishers::Execute_RemoveFinishableActor(Attacker, GetOwner());
		}
		Attacker = PossibleAttacker;
	}
}

AActor* UComponent_Finishers::FinisherVulnerabilityTrace()
{
	AActor* AttackerCandidate = nullptr;
	TArray<AActor*> ActorsToIgnore = TArray<AActor*>();
	ActorsToIgnore.Add(GetOwner());

	FVector StartTrace = GetOwner()->GetActorLocation();

	FHitResult HitResult = StaticFunctions::SphereCollisionTraceChannel(GetWorld(), StartTrace, StartTrace, ETraceTypeQuery::TraceTypeQuery6, ActorsToIgnore, 150.f);

	if (HitResult.bBlockingHit)
	{
		bool bIsPlayer = HitResult.GetActor()->GetClass()->ImplementsInterface(UInterface_Player::StaticClass());

		if (bIsPlayer)
		{
			AttackerCandidate = HitResult.GetActor();
			IInterface_Finishers::Execute_SetFinishableActor(HitResult.GetActor(), GetOwner());
		}
		else
		{
			AttackerCandidate = nullptr;
		}
	}
	
	return AttackerCandidate;

}

void UComponent_Finishers::HandleFinisherRequest(AActor* Target)
{
	AActor* ActorToFinish = nullptr;

	if (Finishers.Num() <= 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, ("WARNING: NO FINISHER TO EXECUTE"));

		return;
	}

	if (Target != nullptr)
	{
		ActorToFinish = Target;
	}
	else
	{
		if (FinishableActors.Num() == 1)
		{
			ActorToFinish = FinishableActors[0];
		}
		else
		{
			ActorToFinish = GetBestFinisherActor();
		}
	}

	if (ActorToFinish != nullptr)
	{
		Victim = ActorToFinish;
		FVector TargetLocation = StaticFunctions::GetLocationAtDistanceXFromActorB(GetOwner(), Victim, Finishers[0].Distance);
		FRotator TargetRotation = StaticFunctions::GetRotationForFacingDirection(GetOwner(), Victim);
		//DrawDebugSphere(GetWorld(), Victim->GetActorLocation(), 30.f, 12, FColor::Red, false, 2.0f);
		//DrawDebugSphere(GetWorld(), TargetLocation, 30.f, 12, FColor::Green, false, 2.0f);

		IInterface_Finishers::Execute_ExecuteFinisher(GetOwner(), TargetLocation, TargetRotation, Finishers[0].FinisherAttack);
		IInterface_Finishers::Execute_ReceiveFinisher(Victim, GetOwner(), Finishers[0].FinisherVictim);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, ("EXECUTE FINISHER REQUEST BUT CANDIDATE ACTOR IS NULL"));
	}
}

AActor* UComponent_Finishers::GetBestFinisherActor()
{
	float MinAngle = FLT_MAX;

	AActor* BestCandidate = nullptr;

	for (AActor* Actor : FinishableActors)
	{
		if (Actor)
		{
			FVector ActorLocation = Actor->GetActorLocation();

			float Angle = StaticFunctions::GetAngleFromActorAToB(GetOwner(), Actor);
			if (Angle < MinAngle)
			{
				MinAngle = Angle;
				BestCandidate = Actor;
			}
		}
	}

	return BestCandidate;
}

void UComponent_Finishers::EndVulnerability()
{
	if (Attacker != nullptr)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, ("REMOVE FINISHABLE ACTOR"));

		IInterface_Finishers::Execute_RemoveFinishableActor(Attacker, GetOwner());
	}
}

