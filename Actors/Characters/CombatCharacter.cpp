// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatCharacter.h"
#include "Component_Damagable.h"
#include "Component_Finishers.h"
#include "Component_ObjectPool.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "FighterGameMode.h"
#include "StaticFunctions.h"
#include "PickableActor.h"
#include "Pistol.h"

// Sets default values
ACombatCharacter::ACombatCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DamagableComponent = CreateDefaultSubobject<UComponent_Damagable>(TEXT("Damagable Component"));
	FinishersComponent = CreateDefaultSubobject<UComponent_Finishers>(TEXT("Finishers Component"));
	FXActorPoolComponent = CreateDefaultSubobject<UComponent_ObjectPool>(TEXT("Spawned FX Pool Component"));
	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("Motion Warping Component"));
}

// Called when the game starts or when spawned
void ACombatCharacter::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void ACombatCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ACombatCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

UComponent_Damagable* ACombatCharacter::GetDamagableComponent_Implementation()
{
	return DamagableComponent;
}

void ACombatCharacter::PlayHitReaction_Implementation(UAnimMontage* TargetMontage, AActor* DamageInstigator)
{
	if (DamageInstigator != nullptr)
	{
		FVector PlayerLocation = GetActorLocation();
		FRotator LookAtRotation = (DamageInstigator->GetActorLocation() - PlayerLocation).Rotation();

		FRotator NewRotation = FRotator(0.0f, LookAtRotation.Yaw, 0.0f);

		SetActorRotation(NewRotation);
		AController* Contrl = GetController();
		if (Contrl)
		{
			Contrl->SetControlRotation(NewRotation);
		}
	}
}

void ACombatCharacter::HitTraceStart_Implementation(FDamageSettings DamageSettings, F_HitTraceSettings HitTraceSettings)
{
	CurrentDamageSettings = DamageSettings;

	float DamageMultiplier = FMath::Lerp(1.0f, 3, ChargedAttackPercentage);

	CurrentDamageSettings.DamageAmount *= DamageMultiplier;
	CurrentDamageSettings.ChargedAttackPercentage = ChargedAttackPercentage;

	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("DAMAGE MULTIPLIER: %f"), DamageMultiplier));
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("DAMAGE: %f"), CurrentDamageSettings.DamageAmount));

	CurrentHitTraceRadius = HitTraceSettings.HitTraceRadius;
	TPair<FName, FName> FightTracePair = StaticFunctions::GetSocketNameFromFightTraceEnum(HitTraceSettings.FightTrace);

	if (HitTraceSettings.FightTrace != EFightTrace::Body)
	{
		bShouldDamageDealer = true;
	}
	else
	{
		bShouldDamageDealer = false;
	}

	FightStartTrace = FightTracePair.Key;
	FightEndTrace = FightTracePair.Value;

}

void ACombatCharacter::HitTraceTick_Implementation()
{
	FVector TraceStart = GetMesh()->GetSocketLocation(FightStartTrace);
	FVector TraceEnd = GetMesh()->GetSocketLocation(FightEndTrace);

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	if (CarriedActor)
		ActorsToIgnore.Add(CarriedActor);


	FHitResult HitResult = StaticFunctions::SphereCollisionTraceChannel(GetWorld(), TraceStart, TraceEnd, ETraceTypeQuery::TraceTypeQuery4, ActorsToIgnore, CurrentHitTraceRadius);

	if (HitResult.bBlockingHit)
	{
		AActor* HitActor = HitResult.GetActor();


		if (!bShouldDamageDealer)
		{
			if (DamagableComponent->LastDamageDealer == HitActor)
			{
				return;
			}
		}

		bool bIsDamagable = HitActor->GetClass()->ImplementsInterface(UInterface_Damagable::StaticClass());

		if (LastDamagedActors.Contains(HitActor))
			return;
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Hit actor: %s with profile: %s"), *HitActor->GetName()));

		if (bIsDamagable)
		{

			LastDamagedActors.Add(HitActor);
			IInterface_Damagable::Execute_TakeDamage(HitActor, CurrentDamageSettings, this);

			FTransform ImpactTransform;
			ImpactTransform.SetLocation(HitResult.ImpactPoint);
			ImpactTransform.SetRotation(HitResult.ImpactNormal.ToOrientationQuat());
			ImpactTransform.SetScale3D(FVector(1.0f, 1.0f, 1.0f));

			if (CarriedActor)
			{
				CarriedActor->SpawnImpactVFX(ImpactTransform, HitResult);
				if (HitActor->GetClass()->ImplementsInterface(UInterface_CombatCharacter::StaticClass()))
				{
					ConsumeWeaponUse();
				}
			}
			else
			{
				FXActorPoolComponent->SpawnActorFromPool(ImpactTransform, FXActorPoolComponent->AttackHitPool);
			}
		}
	}

}

void ACombatCharacter::HitTraceEnd_Implementation()
{
	ChargedAttackPercentage = 0.f;
	LastDamagedActors.Empty();
}

bool ACombatCharacter::IsFinishable_Implementation()
{
	return FinishersComponent->bFinishable;
}

void ACombatCharacter::SetFinishableActor_Implementation(AActor* FinishableActor)
{
	FinishersComponent->FinishableActors.AddUnique(FinishableActor);
}

AActor* ACombatCharacter::GetFinisherVictim_Implementation()
{
	return FinishersComponent->Victim;
}

void ACombatCharacter::RemoveFinishableActor_Implementation(AActor* Actor)
{
	if (FinishersComponent->FinishableActors.Contains(Actor))
	{
		FinishersComponent->FinishableActors.Remove(Actor);
	}
}

void ACombatCharacter::ExitGrabPose_Implementation()
{
	ExitGrab();
}

ACombatCharacter* ACombatCharacter::GetCombatCharacter_Implementation()
{
	return this;
}

void ACombatCharacter::ConsumeWeaponUse()
{
	
}

void ACombatCharacter::ExitGrab()
{

}

void ACombatCharacter::CombatPauseGlobalTimeDilation()
{
	float GlobalDilationFactor = 0.1f;
	GetWorld()->GetWorldSettings()->SetTimeDilation(GlobalDilationFactor);
	CustomTimeDilation = 1.0f / GlobalDilationFactor;
}

void ACombatCharacter::CombatResumeGlobalTimeDilation()
{
	GetWorld()->GetWorldSettings()->SetTimeDilation(1.0f);
	CustomTimeDilation = 1.0f;
}

void ACombatCharacter::OnStartAttack(float distance, AActor* AttackTarget)
{
	float DistanceFromTarget = StaticFunctions::GetHorizontalDistanceBetweenActors(this, AttackTarget);

	if (AttackTarget)
	{
		if (DistanceFromTarget <= distance)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("DistanceFromTarget: %f"), DistanceFromTarget));
			FVector WarpingLocation = StaticFunctions::GetLocationAtDistanceXFromActorB(this, AttackTarget, distance);
			FRotator FacingRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), AttackTarget->GetActorLocation());

			MotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation("AttackTarget", WarpingLocation, FacingRotation);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "Remove warping");

			MotionWarpingComponent->RemoveWarpTarget("AttackTarget");
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "Remove warping");

		MotionWarpingComponent->RemoveWarpTarget("AttackTarget");
	}
	
}

