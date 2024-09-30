// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "Interface_Player.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshRoot = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Root"));

	//SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Root"));
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));

	StaticMeshRoot->SetupAttachment(GetRootComponent());
	StaticMesh->SetupAttachment(StaticMeshRoot);

	//ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	//ProjectileMovementComponent->UpdatedComponent = StaticMeshRoot;
}

void AProjectile::Launch(FVector TargetLocation)
{
}

void AProjectile::DestroyActor_Implementation()
{

}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	FVector TargetLocation = GetActorLocation() + GetActorForwardVector() * 1000.0f;

	TargetActor = IInterface_Player::Execute_GetAttackTarget(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (TargetActor != nullptr)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, ("ATTACK TARGET VALID"));
		TargetLocation = TargetActor->GetActorLocation();
	}

	Launch(TargetLocation);
}

void AProjectile::AdjustVelocity()
{
	
	float WorldTimeDilation = GetWorld()->GetWorldSettings()->GetEffectiveTimeDilation();

	// Adjust the velocity to counteract the time dilation
	FVector AdjustedVelocity = LaunchDirection * Speed / WorldTimeDilation;

	//StaticMeshRoot->Velocity = AdjustedVelocity;
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	AdjustVelocity();
}

