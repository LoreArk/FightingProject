// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"
#include "Kismet/GameplayStatics.h"
#include "FightingProject/Interface_Damagable.h"
#include "Interface_Player.h"
#include "StaticFunctions.h"

void ABullet::BeginPlay()
{
	Super::BeginPlay();
}

void ABullet::Launch(FVector TargetLocation)
{
	AActor* AttackTarget = IInterface_Player::Execute_GetAttackTarget(GetOwner());

	if (AttackTarget)
	{
		FVector AttackTargetLocation = AttackTarget->GetActorLocation();
		AttackTargetLocation.Z += 60;
		LaunchDirection = (AttackTargetLocation - GetActorLocation()).GetSafeNormal();
	}
	else
	{
		LaunchDirection = (TargetLocation - GetActorLocation()).GetSafeNormal();
	}

	FVector LaunchVelocity = LaunchDirection * Speed;
	StaticMeshRoot->AddImpulse(LaunchVelocity);
}

void ABullet::Tick(float DeltaTime)
{
	if (bActivated)
		return;

	Super::Tick(DeltaTime);

	AActor* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	TArray<AActor*> ActorsToIgnore = TArray<AActor*>();
	ActorsToIgnore.Add(Player);

	CustomTimeDilation = Player->GetActorTimeDilation();

	FHitResult HitResult = StaticFunctions::SphereCollisionTraceChannel(GetWorld(), GetActorLocation(), GetActorLocation(), ETraceTypeQuery::TraceTypeQuery4, ActorsToIgnore, DamageRadius);

	if (HitResult.bBlockingHit)
	{
		AActor* HitActor = HitResult.GetActor();
		if (DamagedActors.Contains(HitActor))
			return;

		bool bIsDamagable = HitActor->GetClass()->ImplementsInterface(UInterface_Damagable::StaticClass());

		if (bIsDamagable)
		{
			DamagedActors.Add(HitActor);
			IInterface_Damagable::Execute_TakeDamage(HitActor, DamageSettings, this);
			Destroy();
			bActivated = true;
		}
	}
}

