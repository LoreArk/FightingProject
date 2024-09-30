// Fill out your copyright notice in the Description page of Project Settings.


#include "Pooled_Bullet.h"
#include "StaticFunctions.h"
#include "Kismet/KismetMathLibrary.h"
#include "Interface_Damagable.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/PrimitiveComponent.h"
#include "Kismet/GameplayStatics.h"

APooled_Bullet::APooled_Bullet()
{
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Bullet Mesh"));

	SceneRoot->SetupAttachment(GetRootComponent());
	BulletMesh->SetupAttachment(SceneRoot);

}

void APooled_Bullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsActive)
		return;
	
	FVector LerpedVector = FMath::Lerp(GetActorLocation(), TargetLocation, DeltaTime * Speed);
	
	SetActorLocation(LerpedVector);

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	AActor* HitActor = nullptr;
	UPrimitiveComponent* HitComponent = nullptr;
	//FHitResult HitResult = StaticFunctions::SphereCollisionTraceChannel(GetWorld(), GetActorLocation(), GetActorLocation(), ETraceTypeQuery::TraceTypeQuery4, ActorsToIgnore, BulletTraceRadius);
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel2));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));
	
	FHitResult HitResult = StaticFunctions::SphereCollisionTraceObject(GetWorld(), GetActorLocation(), GetActorLocation(), ObjectTypes, ActorsToIgnore, BulletTraceRadius);

	if (HitResult.bBlockingHit)
	{
		//SetInUse(false);

		HitActor = HitResult.GetActor();
		HitComponent = HitResult.GetComponent();

		if (HitActor != nullptr)
		{
			if (DamagedActors.Contains(HitActor))
			{
				return;
			}
			else
			{
				bIsActive = false;

				DamagedActors.Add(HitActor);

				if (HitResult.GetActor()->GetClass()->ImplementsInterface(UInterface_Damagable::StaticClass()))
				{
					IInterface_Damagable::Execute_TakeDamage(HitActor, DamageSettings, this);
				}

				FXHandler(HitResult);
				return;
			}
		}
		if (HitComponent != nullptr)
		{
			bIsActive = false;
			FXHandler(HitResult);
		}
	}
}

void APooled_Bullet::BeginPlay()
{
	Super::BeginPlay();

}

void APooled_Bullet::FXHandler_Implementation(FHitResult HitResult)
{

}

void APooled_Bullet::SetInUse_Implementation(bool InUse)
{
	Super::SetInUse_Implementation(InUse);

	BulletMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (InUse)
	{
		bIsActive = true;
		FVector ForwardVector = GetActorForwardVector() * 10000.f;

		DamagedActors.Empty();
		TargetLocation = ForwardVector + GetActorLocation();
	}
	else
	{
		bIsActive = false;
		FVector StartLocation;
		SetActorLocation(StartLocation);
	}
}
