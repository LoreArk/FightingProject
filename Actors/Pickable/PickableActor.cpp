// Fill out your copyright notice in the Description page of Project Settings.


#include "PickableActor.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/PrimitiveComponent.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/Controller.h"
#include "Kismet/GameplayStatics.h"
#include "StaticFunctions.h"
#include "FighterGameMode.h"
#include "GameFramework/Character.h"
#include "GeometryCollection/GeometryCollectionComponent.h"

// Sets default values
APickableActor::APickableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	GeometryCollectionComponent = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollectionComponent"));
	
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	SpawnObjectReference = CreateDefaultSubobject<UArrowComponent>(TEXT("RightHandReference"));
	LeftHandReference = CreateDefaultSubobject<UArrowComponent>(TEXT("LeftHandReference"));


	StaticMesh->SetupAttachment(GetRootComponent());
	GeometryCollectionComponent->SetupAttachment(StaticMesh);
	SpawnObjectReference->SetupAttachment(StaticMesh);
	LeftHandReference->SetupAttachment(StaticMesh);

	SpawnedPoolComponent = CreateDefaultSubobject<UComponent_ObjectPool>(TEXT("FX Actor Pool"));

	//Scene->SetupAttachment(GetRootComponent());
	//GeometryCollectionComponent->SetupAttachment(GetRootComponent());
	bShouldSpawnItem = true;
	
}

// Called when the game starts or when spawned
void APickableActor::BeginPlay()
{
	Super::BeginPlay();
	
	OnThrow.AddDynamic(this, &APickableActor::OnThrowHandler);
	OnPickUp.AddDynamic(this, &APickableActor::OnPickUpHandler);
	OnObjectSpawn.AddDynamic(this, &APickableActor::OnObjectSpawnHandler);

	StaticMesh->OnComponentHit.AddDynamic(this, &APickableActor::OnComponentHit);
	StaticMesh->SetSimulatePhysics(false);
}

void APickableActor::HitTrace()
{
	
	//TArray<AActor*> OverlappingTargets;
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("OBJECT HIT TRACE")));

	// Get the player position
	FVector TraceOrigin = GeometryCollectionComponent->GetComponentLocation();

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_PhysicsBody));

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	AActor* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);// Ignore self
	ActorsToIgnore.Add(Player);

	// Sphere trace to find all overlapping actors
	TArray<FHitResult> OutHits;
	bool bHasHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
		GetWorld(),
		TraceOrigin,
		TraceOrigin,
		DamageRadius,
		ObjectTypes,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		OutHits,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		5.0f
	);

	if (bHasHit)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("HAS HIT")));

		for (const FHitResult& Hit : OutHits)
		{
			AActor* HitActor = Hit.GetActor();

			if (HitActor && HitActor->GetClass()->ImplementsInterface(UInterface_Damagable::StaticClass()))
			{
				//OverlappingTargets.AddUnique(HitActor);
				if (!DamagedActor.Contains(HitActor)) 
				{
					//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("OBJECT DAMAGE: %s"), *HitActor->GetName()));
					IInterface_Damagable::Execute_TakeDamage(HitActor, DamageSettings, this);
					DamagedActor.AddUnique(HitActor);
				}
			}
		}
	}
}

// Called every frame
void APickableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APickableActor::Interact_Implementation()
{
	//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, "Interacting");
}

APickableActor* APickableActor::GetCarriable_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, "GET CARRIABLE");
	return this;
}

void APickableActor::TakeDamage_Implementation(FDamageSettings Settings, AActor* InstigatorActor)
{
	//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, " RECEIVE DAMAGE");

	if(!bDontDestroyBeforePickUp)
	Break();
}

void APickableActor::OnPickUpHandler()
{
	//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, "Collision set up");
	
	if (StaticMesh == nullptr) 
	{
		UE_LOG(LogTemp, Warning, TEXT("StaticMesh is null in OnPickUpHandler"));

		return;
	}


	StaticMesh->SetSimulatePhysics(false);
	StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StaticMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	StaticMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Block);
}

void APickableActor::OnObjectSpawnHandler(ACharacter* OwnerChar)
{
	if (!OwnerChar)
	{
		UE_LOG(LogTemp, Warning, TEXT("Parent actor is not a character."));
		return;
	}

	// Get the base aim rotation
	FRotator BaseAimRotation = OwnerChar->GetBaseAimRotation();

	if (OwnerChar->GetClass()->ImplementsInterface(UInterface_Player::StaticClass()))
	{
		AActor* PlayerTarget = IInterface_Player::Execute_GetAttackTarget(OwnerChar);
		if (PlayerTarget)
		{
			FVector DirectionToTarget = PlayerTarget->GetActorLocation() - SpawnObjectReference->GetComponentLocation();
			DirectionToTarget.Z = 0;

			// Calculate the new yaw rotation to face the target
			FRotator LookAtRotation = FRotationMatrix::MakeFromX(DirectionToTarget).Rotator();

			// Update the BaseAimRotation to include the new yaw
			BaseAimRotation.Yaw = LookAtRotation.Yaw;
		}
	}

	// Get the current transform of the SpawnObjectReference
	FTransform SpawnTransform = SpawnObjectReference->GetComponentTransform();

	// Adjust the rotation of SpawnTransform to match the BaseAimRotation
	SpawnTransform.SetRotation(BaseAimRotation.Quaternion());

	// Spawn the actor from the pool with the adjusted transform
	SpawnedPoolComponent->SpawnActorFromPool(SpawnTransform, SpawnedPoolComponent->BulletPool);
}


void APickableActor::OnThrowHandler()
{
	//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, " Throw Collision set up");
	if (thrown || StaticMesh == nullptr)
		return;

	StaticMesh->SetSimulatePhysics(true);
	StaticMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	
	thrown = true;
}

void APickableActor::OnBreakEvent_Implementation()
{
	if (ObjectType == EPickableType::Box && bShouldSpawnItem) 
	{
		AFighterGameMode* GameManager = Cast<AFighterGameMode>(UGameplayStatics::GetActorOfClass(GetWorld(), AFighterGameMode::StaticClass()));
		if (GameManager)
		{
			GameManager->TrySpawnItem(GetActorLocation());
		}
	}


}

void APickableActor::Break()
{
	bBroken = true;
	GeometryCollectionComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	GeometryCollectionComponent->SetSimulatePhysics(true);
	GeometryCollectionComponent->SetHiddenInGame(false);
	

	OnBreakEvent();

	if (StaticMesh != nullptr) 
	{
		StaticMesh->DestroyComponent();
	}

}

void APickableActor::OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!thrown)
		return;
	if (!Hit.bBlockingHit)
		return;
	if (bHitTraceFlag == false && OtherActor != ThrowrActor)
	{
		bHitTraceFlag = true;

		HitTrace();
	}

	if (OtherActor != ThrowrActor) 
	{

		Break();

		FTransform ImpactTransform;
		ImpactTransform.SetLocation(Hit.ImpactPoint);
		ImpactTransform.SetRotation(Hit.ImpactNormal.ToOrientationQuat());
		ImpactTransform.SetScale3D(FVector(1.0f, 1.0f, 1.0f));

		SpawnImpactVFX(ImpactTransform, Hit);
	}

	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("COMPONENT HIT!"));
}


void APickableActor::SpawnImpactVFX_Implementation(FTransform SpawnTransform, FHitResult HitResult)
{
	SpawnedPoolComponent->SpawnActorFromPool(SpawnTransform, SpawnedPoolComponent->AttackHitPool);
}

bool APickableActor::CanThrow_Implementation()
{
	if (!bBroken && StaticMesh != nullptr && !thrown)
	{
		return true;
	}
	return false;
}



