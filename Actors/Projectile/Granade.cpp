// Fill out your copyright notice in the Description page of Project Settings.


#include "Granade.h"
#include "Kismet/GameplayStatics.h"
#include "FightingProject/Interface_Damagable.h"
#include "Interface_Player.h"
#include "StaticFunctions.h"




void AGranade::BeginPlay()
{
	Super::BeginPlay();
}

void AGranade::Launch(FVector TargetLocation)
{
	AActor* AttackTarget = IInterface_Player::Execute_GetAttackTarget(GetOwner());

	if (AttackTarget)
	{
		LaunchDirection = (AttackTarget->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	}
	else
	{
		LaunchDirection = (TargetLocation - GetActorLocation()).GetSafeNormal();
	}

	FVector LaunchVelocity = LaunchDirection * Speed;
	StaticMeshRoot->AddImpulse(LaunchVelocity);
}

void AGranade::Tick(float DeltaTime)
{
	AActor* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	TArray<AActor*> ActorsToIgnore = TArray<AActor*>();
	ActorsToIgnore.Add(Player);

	CustomTimeDilation = Player->GetActorTimeDilation();

	FHitResult HitResult = StaticFunctions::SphereCollisionTraceChannel(GetWorld(), GetActorLocation(), GetActorLocation(), ETraceTypeQuery::TraceTypeQuery4, ActorsToIgnore, DamageRadius);

	if (HitResult.bBlockingHit)
	{
		AActor* HitActor = HitResult.GetActor();

		bool bIsDamagable = HitActor->GetClass()->ImplementsInterface(UInterface_Damagable::StaticClass());

		if (bIsDamagable)
		{
			Activate();
		}
	}
}

void AGranade::Activate()
{
	if (bActivated == true)
		return;


	bActivated = true;

	AActor* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	TArray<AActor*> ActorsToIgnore = TArray<AActor*>();
	ActorsToIgnore.Add(Player);
	TArray<FHitResult> HitResults = StaticFunctions::SphereCollisionMultiTraceByChannel(GetWorld(), GetActorLocation(), GetActorLocation(), ETraceTypeQuery::TraceTypeQuery4, ActorsToIgnore, ExplosionRadius);

	if (HitResults.Num() > 0)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("HAS HIT")));

		for (const FHitResult& Hit : HitResults)
		{
			AActor* HitActor = Hit.GetActor();

			if (HitActor && HitActor->GetClass()->ImplementsInterface(UInterface_Damagable::StaticClass()))
			{
				//OverlappingTargets.AddUnique(HitActor);
				if (!DamagedActors.Contains(HitActor))
				{
					//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("OBJECT DAMAGE: %s"), *HitActor->GetName()));
					IInterface_Damagable::Execute_TakeDamage(HitActor, DamageSettings, this);
					DamagedActors.AddUnique(HitActor);
					
				}
			}
		}
	}

	DestroyActor();
}
