// Fill out your copyright notice in the Description page of Project Settings.


#include "Component_ObjectPool.h"

UComponent_ObjectPool::UComponent_ObjectPool()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void UComponent_ObjectPool::BeginPlay()
{
	Super::BeginPlay();

	InitializePools();
}

void UComponent_ObjectPool::InitializePools()
{
	for (int i = 0; i < AttackHitPoolSize; i++)
	{
		FActorSpawnParameters SpawnParams;
		FVector Location;
		FRotator Rotation;
		APooledActor* SpawnedActor = GetOwner()->GetWorld()->SpawnActor<APooledActor>(AttackHitActor, Location, Rotation, SpawnParams);
		AttackHitPool.AddUnique(SpawnedActor);
		SpawnedActor->SetInUse(false);
	}
	for (int i = 0; i < AttackHitPoolSize_2; i++)
	{
		FActorSpawnParameters SpawnParams;
		FVector Location;
		FRotator Rotation;
		APooledActor* SpawnedActor = GetOwner()->GetWorld()->SpawnActor<APooledActor>(AttackHitActor_2, Location, Rotation, SpawnParams);
		AttackHitPool_2.AddUnique(SpawnedActor);
		SpawnedActor->SetInUse(false);
	}
	for (int i = 0; i < BulletPoolSize; i++)
	{
		FActorSpawnParameters SpawnParams;
		FVector Location;
		FRotator Rotation;
		APooledActor* SpawnedActor = GetOwner()->GetWorld()->SpawnActor<APooledActor>(BulletActor, Location, Rotation, SpawnParams);
		BulletPool.AddUnique(SpawnedActor);
		SpawnedActor->SetInUse(false);
	}
	for (int i = 0; i < GranadePoolSize; i++)
	{
		FActorSpawnParameters SpawnParams;
		FVector Location;
		FRotator Rotation;
		APooledActor* SpawnedActor = GetOwner()->GetWorld()->SpawnActor<APooledActor>(GranadeActor, Location, Rotation, SpawnParams);
		
		GranadePool.AddUnique(SpawnedActor);
		SpawnedActor->SetInUse(false);
	}
}

APooledActor* UComponent_ObjectPool::FindFirstAvailableActorInPool(TArray<APooledActor*> ActorPool)
{
	for (int i = 0; i < ActorPool.Num(); i++)
	{
		if (ActorPool[i]->bInUse == false)
		{
			return ActorPool[i];
		}
	}
	return nullptr;
}


APooledActor* UComponent_ObjectPool::SpawnActorFromPool(FTransform SpawnTransform, TArray<APooledActor*> Pool)
{
	APooledActor* ActorFromPool = FindFirstAvailableActorInPool(Pool);

	if (ActorFromPool)
	{
		ActorFromPool->SetActorLocation(SpawnTransform.GetLocation());
		ActorFromPool->SetActorRotation(SpawnTransform.GetRotation());
		ActorFromPool->SetInUse(true);
		return ActorFromPool;
	}
	else 
	{
		//
	}

	return nullptr;
}


void UComponent_ObjectPool::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

