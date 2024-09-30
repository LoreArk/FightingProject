// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyBase.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Component_Damagable.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "FightingProject/PickableActor.h"
#include "Weapon.h"
#include "Pistol.h"
#include "F_AIAttacksData.h"
#include "FighterGameMode.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "StaticFunctions.h"
#include "StatsHUD.h"
#include "TimerManager.h"

// Sets default values
AEnemyBase::AEnemyBase()
{
	PrimaryActorTick.bCanEverTick = true;

	ResourcesComponent = CreateDefaultSubobject<UComponent_ResourcesDrop>(TEXT("Resources Drop Component"));

	StatsWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Stats Widget Component"));
	StatsWidgetComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	FinishersCameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	FinishersCameraBoom->SetupAttachment(GetMesh(), "spine_04");
	FinishersCameraBoom->TargetArmLength = 400.0f;
	FinishersCameraBoom->bUsePawnControlRotation = false;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	Camera->SetupAttachment(FinishersCameraBoom, USpringArmComponent::SocketName); 
	Camera->bUsePawnControlRotation = true;
}

UBehaviorTree* AEnemyBase::GetBehviorTree()
{
	return Tree;
}


void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	
	if (DefaultAnimClassLayer) 
	{
		GetMesh()->GetAnimInstance()->LinkAnimClassLayers(DefaultAnimClassLayer);
	}

	if (AttackUnarmedMovesetData)
	{
		F_AIAttacksData* UnarmedAttackData = AttackUnarmedMovesetData->FindRow<F_AIAttacksData>("Attacks", TEXT(""));
		if (UnarmedAttackData)
		{
			Attacks = UnarmedAttackData->AttackMapAction;
		}
		F_AIAttacksData* CounterAttackData = AttackUnarmedMovesetData->FindRow<F_AIAttacksData>("CounterAttack", TEXT(""));
		if (UnarmedAttackData)
		{
			CounterAttacks = CounterAttackData->AttackMapAction;
		}
	}
	if (WeaponToEquip)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();

			FName SocketName = TEXT("WeaponSocket");

			FTransform SocketTransform = GetMesh()->GetSocketTransform("Club");

			APickableActor* SpawnedWeapon = World->SpawnActor<APickableActor>(WeaponToEquip, SocketTransform, SpawnParams);

			SpawnedWeapon->OnPickUpHandler();

			if (SpawnedWeapon)
			{
				SpawnedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, SpawnedWeapon->CarrySocketName);
			}
			CarriedActor = SpawnedWeapon;

			if (AWeapon* WeaponClass = Cast<AWeapon>(CarriedActor))
			{
				CarriedActor->OnPickUpHandler();
				TMap<UAnimMontage*, FAIAttacks> AttackMap;
				F_AIAttacksData* AttackData = WeaponClass->AI_AttackDataTable->FindRow<F_AIAttacksData>("Attacks", TEXT(""));
				if (AttackData)
				{
					AttackMap = AttackData->AttackMapAction;
					WeaponAttacks = AttackMap;
					AvailableAttacks = AttackMap;
				}
				if (APistol* PistolClass = Cast<APistol>(CarriedActor))
				{
					bRangedCombat = true;
				}
				if (TSubclassOf<UAnimInstance> AnimInstance = WeaponClass->AnimInstanceClass)
				{
					GetMesh()->GetAnimInstance()->LinkAnimClassLayers(AnimInstance);
				}
			}
		}
	}
	else 
	{
		AvailableAttacks = Attacks;
	}

	SpawnDefaultController();

	AIC = Cast<AAIC_EnemyBase>(GetController());

	StatsHud = Cast<UStatsHUD>(StatsWidgetComponent->GetUserWidgetObject());
	if (StatsHud)
	{

		StatsHud->SetDamagableComponent(DamagableComponent);
		StatsHud->ToggleBarsVisibility(false);
	}

	DamagableComponent->OnDeath.AddDynamic(this, &AEnemyBase::OnDeathEvent);
	DamagableComponent->OnStun.AddDynamic(this, &AEnemyBase::OnStunEvent);

	AIC->OnCombatEnter.AddDynamic(this, &AEnemyBase::StartAttacksUpdate);
	AIC->OnCombatExit.AddDynamic(this, &AEnemyBase::StopAttacksUpdate);

	AFighterGameMode* GameMode = Cast<AFighterGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode != nullptr)
	{
		CombatManager = GameMode->CombatManager;
	}
}

void AEnemyBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{

	if (GEngine)
	{
		AActor* OwnerActor = GetOwner();
		FString OwnerName = OwnerActor ? *OwnerActor->GetName() : TEXT("No Owner");

		//GEngine->AddOnScreenDebugMessage(-1, 0.05f, FColor::Orange, FString::Printf(TEXT("INVALIDATE TIMERS, Owner Actor: %s"), *OwnerName));
	}

	InvalidateAllTimers();

	Super::EndPlay(EndPlayReason);
}

void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (DamagableComponent->bIsStaggered)
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.05f, FColor::Orange, "STAGGERED");

	}
	
}

FRotator AEnemyBase::GetBaseAimRotation() const
{
	AController* CharController = GetController();
	if (CharController)
	{
		FRotator ControlRotation = CharController->GetControlRotation();

		if (AIC)
		{
			if (AIC->AttackTarget)
			{
				FVector PawnLocation = GetActorLocation();
				FVector TargetLocation = AIC->AttackTarget->GetActorLocation();
				FRotator LookAtRotation = (TargetLocation - PawnLocation).Rotation();

				// Update the pitch of the control rotation to look at the target
				ControlRotation.Pitch = LookAtRotation.Pitch;
			}
		}
		

		return ControlRotation;
	}

	return Super::GetBaseAimRotation();
}

// Called to bind functionality to input
void AEnemyBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemyBase::TakeDamage_Implementation(FDamageSettings DamageSettings, AActor* InstigatorActor)
{
	//FString DistanceString = FString::Printf(TEXT("Distance from Dealer: %.2f"), distanceFromDealer);
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, DistanceString);

	if (DamagableComponent->Health <= 0)
	{
		return;
	}

	ReceivedDamageSettings = DamageSettings;
	
	if (AIC->AiState == EAIStates::Finisher) //If receiving damage during finisher, should not execute any further logic
	{
		DamagableComponent->TakeFinisherDamage(DamageSettings);
		return;
	}

	bool bWasDamaged = DamagableComponent->TakeDamage(DamageSettings, InstigatorActor);
	
	if (bWasDamaged) 
	{
		if (DamagableComponent->CurrentFrozenState == EFrozenState::None)
		{
			HitTakenCount++;
		}

		AggroCooldownTimer();

		DamagableComponent->ProcessDamageForHitReaction(ReceivedDamageSettings, InstigatorActor);
	}
	else if (bIsBlocking)
	{
		if (DamageSettings.DamageResponse == EDamageResponse::Stun)
		{
			bIsBlocking = false;
			DamagableComponent->CurrentFrozenState = EFrozenState::None;

			if (DamagableComponent->bIsStunned)
			{
				HighPriorityStunMontage = DamagableComponent->StunMontage;
				IInterface_CombatCharacter::Execute_PlayHitReaction(this, DamagableComponent->StunMontage, InstigatorActor);
				return;
			}
			
			TMap<UAnimMontage*, FHitReactionSettings>::TIterator It = DamagableComponent->StunHitReactions.CreateIterator();
			UAnimMontage* Key = It.Key();
			IInterface_CombatCharacter::Execute_PlayHitReaction(this, Key, InstigatorActor);

			return;
		}

		BlockedHitCount++;

		StartBlockTimer(BlockedHitCount);

		BlockedHitsForCounter = UKismetMathLibrary::RandomIntegerInRange(MinHitForCounter, MaxHitForCounter);
		if (BlockedHitCount >= BlockedHitsForCounter)
		{
			DamagableComponent->bIsInvincible = true;

			SetIsTargeted_Implementation(false);

			TMap<UAnimMontage*, FAIAttacks>::TIterator It = CounterAttacks.CreateIterator();
			UAnimMontage* Key = It.Key();
			FAIAttacks Value = It.Value();
			CurrentDamageSettings = Value.DamageSettings;
			HighPriorityAttackMontage = Key;

			CounterAttack();
		}
		else
		{
			FVector PushDirection = GetActorLocation() - InstigatorActor->GetActorLocation();
			MovementComponentPush_Implementation(PushDirection, 10.f);
			BlockAttackFX(DamageSettings);
		}
	}

	AIC->OnDamageReceived(InstigatorActor);
}

void AEnemyBase::SetIsTargeted_Implementation(bool bIsTargeted)
{
	if (DamagableComponent->bIsDead)
		return;

	if (!bIsTargeted && bIsBlocking)
	{
		BlockedHitCount = 0;
		bIsBlocking = false;
		EndAttack();
		return;
	}

	if (DamagableComponent->bIsStunned)
		return;
	if (DamagableComponent->bIsStaggered)
		return;
	
	if (bShouldTryDodge && !bIsBlocking && !bIsAttacking && GetCanPlayAttackMontage())
	{
		int DodgeChance = UKismetMathLibrary::RandomIntegerInRange(0, 3);
		if (DodgeChance > 1)
		{
			EndAttack();
			int DodgeIndex = GetDodgeMontageIndex();
			CurrentDodgeMontage = DodgeMontages[DodgeIndex];
			GetMesh()->GetAnimInstance()->Montage_Play(CurrentDodgeMontage, 1.2f);
			SetIsTargeted_Implementation(false);
			return;
		}
	}
	else
	{
		if (DamagableComponent->CurrentFrozenState == EFrozenState::Juggling && GetMesh()->GetAnimInstance()->Montage_IsPlaying(DodgeMontages[3]) == false)
		{
			int JuggleAvoidChance = UKismetMathLibrary::RandomIntegerInRange(0, 3);
			if (JuggleAvoidChance == 3)
			{
				DamagableComponent->bIsInvincible = true;
				CurrentDodgeMontage = DodgeMontages[3];
				GetMesh()->GetAnimInstance()->Montage_Play(CurrentDodgeMontage, 1.f);
				return;
			}
		}
	}

	HitForBlocking = UKismetMathLibrary::RandomIntegerInRange(MinHitForBlocking, MaxHitForBlocking);

	if (HitTakenCount >= HitForBlocking  && !bIsAttacking && DamagableComponent->CurrentFrozenState == EFrozenState::None)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("SET IS TARGETED ON"));
		EndAttack();
		AIC->SetStateAsAttacking();
		bIsBlocking = true;
		HitTakenCount = 0;
		BlockStance();
		return;
	}
}

float AEnemyBase::SetMovementSpeed_Implementation(EMovementSpeed Speed)
{
	float TargetSpeed = 0.f;
	switch (Speed)
	{
	case EMovementSpeed::Idle:
		TargetSpeed = 0.f;
		break;
	case EMovementSpeed::Walk:
		TargetSpeed = 220.f;
		break;
	case EMovementSpeed::Jog:
		TargetSpeed = 330.f;
		break;
	case EMovementSpeed::Sprint:
		TargetSpeed = 430.f;
		break;
	default:
		break;
	}

	GetCharacterMovement()->MaxWalkSpeed = TargetSpeed;
	return TargetSpeed;
}

AEnemyBase* AEnemyBase::GetOwnerClass_Implementation()
{
	return this;
}

bool AEnemyBase::GetIsBlocking_Implementation()
{
	return bIsBlocking;
}

bool AEnemyBase::GetIsStunned_Implementation()
{
	if (DamagableComponent)
	{
		return DamagableComponent->bIsStunned;
	}
	else 
	{
		return false;
	}
}


void AEnemyBase::PlayHitReaction_Implementation(UAnimMontage* TargetMontage, AActor* DamageInstigator)
{
	if (ShouldStopMontageForHitReaction())
	{
		float StopBlendTime = 0.0f;
		if (DamagableComponent->CurrentFrozenState == EFrozenState::Juggling)
		{
			StopBlendTime = 0.1f;
		}
		GetMesh()->GetAnimInstance()->StopAllMontages(StopBlendTime);
	}

	Super::PlayHitReaction_Implementation(TargetMontage, DamageInstigator);

	EndAttack();
	CurrentHitReaction = TargetMontage;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;


	GetMesh()->GetAnimInstance()->OnMontageBlendingOut.RemoveAll(this);

	FOnMontageBlendingOutStarted BlendingOutDelegate;
	BlendingOutDelegate.BindLambda([this](UAnimMontage* Montage, bool bInterrupted)
		{
			EndHitReaction(Montage);
		});
	

	GetMesh()->GetAnimInstance()->Montage_Play(TargetMontage, 1.2f, EMontagePlayReturnType::Duration, 0.0f, ShouldStopMontageForHitReaction());
	
	GetMesh()->GetAnimInstance()->Montage_SetBlendingOutDelegate(BlendingOutDelegate, TargetMontage);

}

void AEnemyBase::PlayGetUpMontage()
{
	if (DamagableComponent->bIsDead)
		return;

	GetMesh()->GetAnimInstance()->OnMontageBlendingOut.RemoveAll(this);

	FOnMontageBlendingOutStarted BlendingOutDelegate;
	BlendingOutDelegate.BindLambda([this](UAnimMontage* Montage, bool bInterrupted)
		{
			EndHitReaction(Montage);
		});

	GetMesh()->GetAnimInstance()->Montage_Play(GetUpMontage, 1.2f, EMontagePlayReturnType::Duration, 0.0f, ShouldStopMontageForHitReaction());
	GetMesh()->GetAnimInstance()->Montage_SetBlendingOutDelegate(BlendingOutDelegate, GetUpMontage);
}

int AEnemyBase::GetDodgeMontageIndex()
{
	FVector LeftDirection = -GetActorRightVector();
	FVector RightDirection = GetActorRightVector();
	FVector BackDirection = -GetActorForwardVector();

	bool bLeftClear = !PerformSphereTraceWithDirection(LeftDirection);
	bool bRightClear = !PerformSphereTraceWithDirection(RightDirection);
	bool bBackClear = !PerformSphereTraceWithDirection(BackDirection);

	AActor* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	float Angle = StaticFunctions::GetAngleFromActorWithSideVector(Player, this);
	//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, FString::Printf(TEXT("Angle: %.2f"), Angle));
	// Prioritize directions with consideration to the attacker
	if (bLeftClear && Angle < 0)
	{
		return 0; // Index for left montage
	}
	else if (bRightClear && Angle > 0)
	{
		return 1; // Index for right montage
	}
	else if (Angle == 0)
	{
		if (bLeftClear)
		{
			return 0;
		}
		if (bRightClear)
		{
			return 1;
		}
		if (bBackClear)
		{
			return 2;
		}
	}

	return 2; // Index for back montage
}

bool AEnemyBase::PerformSphereTraceWithDirection(FVector Direction)
{
	FVector Start = GetActorLocation();
	FVector End = Start + (Direction * 300.0f); 
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
	TArray<AActor*> ActorToIgnore = TArray<AActor*>();
	if(CarriedActor)
	ActorToIgnore.Add(CarriedActor);
	ActorToIgnore.Add(this);

	FHitResult HitResult = StaticFunctions::SphereCollisionTraceObject(GetWorld(), Start, End, ObjectTypes, ActorToIgnore, 50.f);

	if (HitResult.bBlockingHit)
	{
		FString HitActorName = HitResult.GetActor() ? HitResult.GetActor()->GetName() : TEXT("None");

		FString HitDetails = FString::Printf(
			TEXT("Hit Actor: %s"),
			*HitActorName
		);

		//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, HitDetails);
	}
	else
	{
		// Log when no hit occurs
		//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, TEXT("No hit detected"));
	}

	return HitResult.bBlockingHit;
}

void AEnemyBase::StartAttacksUpdate()
{
	GetWorld()->GetTimerManager().SetTimer(UpdateAttackTimerHandle, this, &AEnemyBase::UpdateReadyAttacks, 0.2f, true);
}

void AEnemyBase::StopAttacksUpdate()
{
	GetWorldTimerManager().ClearTimer(UpdateAttackTimerHandle);
}

void AEnemyBase::PlayDeathMontage_Implementation(UAnimMontage* TargetMontage)
{
	DeathMontage = TargetMontage;
	UAnimMontage* MontageToStop = GetMesh()->GetAnimInstance()->GetCurrentActiveMontage();
	GetMesh()->GetAnimInstance()->Montage_Stop(0.0f, MontageToStop);
}

void AEnemyBase::SpawnWeaponSpawnableActor_Implementation()
{
	if (CarriedActor)
	{
		CarriedActor->OnObjectSpawn.Broadcast(this);
	}
}

void AEnemyBase::Attack_Implementation()
{
	DefaultAttack_Implementation(AIC->AttackTarget);
}

AActor* AEnemyBase::GetAttackTarget_Implementation()
{
	if (AIC)
	{
		return AIC->AttackTarget;
	}
	return nullptr;
}

void AEnemyBase::ReceiveFinisher_Implementation(AActor* ActorToFace, UAnimMontage* Montage)
{
	FRotator TargetRotation = StaticFunctions::GetRotationForFacingDirection(this, ActorToFace);

	SetActorRotation(TargetRotation);
	
	EndAttack();

	EndStunState();
	FinishersComponent->EndVulnerability();
	AIC->SetStateAsFinisher();
	DamagableComponent->OnStunEnd.Broadcast();
}

UCameraComponent* AEnemyBase::GetFinisherCamera_Implementation()
{
	return Camera;
}

void AEnemyBase::MovementComponentPush_Implementation(FVector PushDirection, float Strenght)
{
	GetCharacterMovement()->AddImpulse(PushDirection * Strenght, true);
}

void AEnemyBase::Interact_Implementation()
{
}

bool AEnemyBase::CanBeGrabbed_Implementation()
{
	if (DamagableComponent->PoiseValue >= 1.f && !DamagableComponent->bIsStunned && DamagableComponent->PoiseValue >= 1.f && !DamagableComponent->bIsStaggered || DamagableComponent->bIsInvincible || bIsBlocking || bIsAttacking)
	{
		return false;
	}
	return true;
}

void AEnemyBase::ReceiveGrabAction_Implementation(UAnimMontage* Montage, AActor* ActorToFace)
{
	FRotator TargetRotation = StaticFunctions::GetRotationForFacingDirection(this, ActorToFace);

	SetActorRotation(TargetRotation);

	EndAttack();
	AIC->SetStateAsFinisher();
	CurrentGrabPose = Montage;
	GetMesh()->GetAnimInstance()->Montage_Play(Montage, 1.f, EMontagePlayReturnType::MontageLength, 0.0f, true);
}

void AEnemyBase::ReceiveGrabAttack_Implementation(UAnimMontage* Montage)
{
	
}


void AEnemyBase::HitWall_Implementation(FHitResult HitResult)
{
	FVector ImpactNormal = HitResult.ImpactNormal;
	FRotator ImpactRotation = ImpactNormal.Rotation();

	MotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation("Wall", HitResult.ImpactPoint, ImpactRotation);
}

void AEnemyBase::ExitGrab()
{
	GetMesh()->GetAnimInstance()->StopAllMontages(0.2f);
	if (DamagableComponent->Health <= 0) //send death event only after grab montage
	{
		CurrentGrabPose = nullptr;
		DamagableComponent->bIsDead = true;
		DamagableComponent->OnDeath.Broadcast(this);
		ShutDownAfterDeathMontage();
		return;
	}
	if (DamagableComponent->bIsStunned)
	{
		AIC->SetStateAsStun();
	}
	else
	{
		AIC->SetStateAsAttacking();
	}

	CurrentGrabPose = nullptr;
	//EndAttack();

}

EFrozenState AEnemyBase::CanReceiveFrozenAttack_Implementation()
{
	if (DamagableComponent->CurrentFrozenState == EFrozenState::Fell)
	{
		return EFrozenState::Fell;
	}

	if (bJuggleApexFlag)
	{
		return EFrozenState::Juggling;
	}

	return EFrozenState::None;
}

void AEnemyBase::UpdateReadyAttacks()
{
	//GEngine->AddOnScreenDebugMessage(-1, 0.05f, FColor::Green, FString::Printf(TEXT("Update ready attack, Owner Actor: %s"), *GetOwner()->GetName()));

	if (!AIC)
	{
		return;
	}
	if (!AIC->AttackTarget)
	{
		return;
	}
	if (bIsBlocking == true)
	{
		return;
	}
	if (CurrentGrabPose != nullptr)
	{
		ReadyAttacks.Empty();
		return;
	}

	FVector TraceStart = GetActorLocation();
	TraceStart.Z += 80.f;
	FVector TraceEnd = AIC->AttackTarget->GetActorLocation();
	TraceEnd.Z += 80.f;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
	
	TArray<AActor*> ActorToIgnore = TArray<AActor*>();
	if (CarriedActor)
	{
		ActorToIgnore.Add(CarriedActor);
	}
	ActorToIgnore.Add(this);

	FHitResult HitResult = StaticFunctions::SphereCollisionTraceObject(GetWorld(), TraceStart, TraceEnd, ObjectTypes, ActorToIgnore, 1.f);
	StatsHud->ToggleBarsVisibility(!HitResult.bBlockingHit);

	if (HitResult.bBlockingHit)
	{
		ReadyAttacks.Empty();
		return;
	}

	float DistanceFromTarget = UKismetMathLibrary::Vector_Distance(GetActorLocation(), AIC->AttackTarget->GetActorLocation());

	ReadyAttacks.Empty();

	TArray<TPair<UAnimMontage*, FAIAttacks>> ValidAttacks;

	// Loop through available attacks
	for (TPair<UAnimMontage*, FAIAttacks>& Kvp : AvailableAttacks)
	{
		if (Kvp.Value.MinDistance < DistanceFromTarget)
		{
			continue;
		}
		if (Kvp.Value.MaxDistance > DistanceFromTarget)
		{
			continue;
		}
		ValidAttacks.Add(Kvp);
	}

	// If there's one or no valid attack, use them directly
	if (ValidAttacks.Num() <= 1)
	{
		for (const TPair<UAnimMontage*, FAIAttacks>& Kvp : ValidAttacks)
		{
			ReadyAttacks.Add(Kvp.Key, Kvp.Value); 
		}
	}
	else
	{
		TArray<TPair<UAnimMontage*, FAIAttacks>> PreferredAttacks;

		// Calculate the minimum distance to choose based on the criteria
		for (const TPair<UAnimMontage*, FAIAttacks>& Kvp : ValidAttacks)
		{
			float DistanceToMin = FMath::Abs(Kvp.Value.MinDistance - DistanceFromTarget);
			if (DistanceToMin <= 50.0f)
			{
				PreferredAttacks.Add(Kvp);
			}
		}

		// If we have preferred attacks, use those
		if (PreferredAttacks.Num() > 0)
		{
			for (const TPair<UAnimMontage*, FAIAttacks>& Kvp : PreferredAttacks)
			{
				ReadyAttacks.Add(Kvp.Key, Kvp.Value);
			}
		}
		else
		{
			// No preferred attacks found, but we still add the valid ones
			for (const TPair<UAnimMontage*, FAIAttacks>& Kvp : ValidAttacks)
			{
				ReadyAttacks.Add(Kvp.Key, Kvp.Value);
			}
		}
	}
}

void AEnemyBase::OnReceiveFinisherEnd(UAnimMontage* Montage, bool bInterrupted)
{
	if (DamagableComponent->Health <= 0) //trigger death event only after the finisher animation
	{
		DamagableComponent->bIsDead = true;
		DamagableComponent->OnDeath.Broadcast(this);
		OnDeathEvent(this);
		ShutDownAfterDeathMontage();
		FinishersComponent->EndVulnerability();
		return;
	}
	CurrentGrabPose = nullptr;
	EndAttack();

	AvailableAttacks = Attacks;
	//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, "RECEIVED FINISHER END");

	AIC->SetStateAsAttacking();
}

void AEnemyBase::DefaultAttack_Implementation(AActor* AttackTarget)
{
	if (DamagableComponent->bIsDead)
		return;
	
	if (ReadyAttacks.Num() > 0 && !bAttackSelectionTriggerFlag && CurrentHitReaction == nullptr)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, "FIND ATTACK Weighted Attack");

		//From the available attacks map, get only the first attack of combo or single attacks for weighting
		float TotalWeight = 0.0f;
		
		bool bTokenAvailable = false;

		TArray<FString> ComboIds;
		TMap<UAnimMontage*, FAIAttacks> SelectedForWeighting;
		for (TPair<UAnimMontage*, FAIAttacks>& Kvp : ReadyAttacks)
		{
			if (Kvp.Value.SortString == "" || !ComboIds.Contains(Kvp.Value.SortString))
			{
				ComboIds.AddUnique(Kvp.Value.SortString);
				SelectedForWeighting.Add(Kvp.Key, Kvp.Value);
				TotalWeight += Kvp.Value.Weight;
			}
		}

		float RandomValue = FMath::FRandRange(0.0f, TotalWeight);

		float CumulativeWeight = 0.0f;
		TMap<UAnimMontage*, FAIAttacks> Weighted;
		FString ComboID = "";
		bool bIsCombo = false;
		for (TPair<UAnimMontage*, FAIAttacks>& Kvp : SelectedForWeighting)
		{
			CumulativeWeight += Kvp.Value.Weight;
			if (RandomValue <= CumulativeWeight)
			{
				if (Kvp.Value.SortString != "")
				{
					bIsCombo = true;
					ComboID = Kvp.Value.SortString;
				}
				Weighted.Add(Kvp.Key, Kvp.Value);

				break;
			}
		}

		//If exists, get the entire combo for the weighted single attack
		TMap<UAnimMontage*, FAIAttacks> AttacksToPlay = TMap<UAnimMontage*, FAIAttacks>();
		if (bIsCombo)
		{
			for (TPair<UAnimMontage*, FAIAttacks>& Kvp : ReadyAttacks)
			{
				if (Weighted.Num() > 0 && Kvp.Value.SortString == ComboID)
				{
					AttacksToPlay.Add(Kvp.Key, Kvp.Value);
				}
			}
		}
		else
		{
			AttacksToPlay = Weighted;
		}
		
		if (AttacksToPlay.Num() > 0)
		{
			bAttackSelectionTriggerFlag = true;
			AttackMontagePlay_Implementation(AttacksToPlay, ComboCount);

			if (CombatManager)
			{
				CombatManager->AddAttackingActor(this);
			}
		}
		else
		{
			EndAttack();
		}
	}
	else
	{

		EndAttack();
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Magenta, "COULDNT EXECUTE DEFAULT ATTACK");
	}
}

void AEnemyBase::CounterAttack_Implementation()
{
	HighPriorityAttackMontage = nullptr;
	DamagableComponent->bIsInvincible = false;
	EndAttack();
	AIC->SetStateAsAttacking();
	SetIsTargeted_Implementation(false);
}

void AEnemyBase::AttackMontagePlay_Implementation(const TMap<UAnimMontage*, FAIAttacks>& ComboAttacks, int32 AttacksCount)
{
	if (GetCanPlayAttackMontage() == false)
		return;
	int Num = ComboAttacks.Num();
	
	/*
	if (AttacksCount == 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, "Move to");
		AIC->MoveToActor(AIC->AttackTarget, 50.f);
	}*/
	
	if (AttacksCount < Num)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, FString::Printf(TEXT("%d"), AttacksCount));
		TMap<UAnimMontage*, FAIAttacks> AttackMontages = TMap<UAnimMontage*, FAIAttacks>();

		int32 CurrentIndex = 0;
		UAnimMontage* CurrentAttackKey = nullptr;
		for (const TPair<UAnimMontage*, FAIAttacks>& Pair : ComboAttacks)
		{
			if (CurrentIndex == AttacksCount)
			{
				CurrentAttack.Empty();
				CurrentAttack.Add(Pair);
				CurrentAttackKey = Pair.Key;
			}
			CurrentIndex++;

			AttackMontages.Add(Pair);
		}
		
		if (CurrentAttackKey != nullptr)
		{
			StartCooldownTimer(CurrentAttackKey);
			AvailableAttacks.Remove(CurrentAttackKey);
		}

		AttackMontagePlay(ComboAttacks, AttacksCount);
	}
	else
	{
		EndAttack();
	}
}

void AEnemyBase::PlayMontage_Implementation(UAnimMontage* MontageToPlay)
{
}

void AEnemyBase::EndAttack()
{
	if (CombatManager)
	{
		CombatManager->RemoveAttackingActor(this);
	}
	AIC->OnAttackFinishedDelegate.Broadcast(AIC);
	StartCooldownTimer(nullptr);

	bAttackSelectionTriggerFlag = false;
	ComboCount = 0;
}

void AEnemyBase::BlockStance_Implementation()
{
	AIC->SetStateAsAttacking();
}

void AEnemyBase::OnStunEvent()
{
	FinishersComponent->bFinishable = true;
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, " STUN");
	DropWeapon();
	bRangedCombat = false;

	if (GetMesh()->GetAnimInstance()->GetClass() != DefaultAnimClassLayer)
	{
		GetMesh()->GetAnimInstance()->LinkAnimClassLayers(DefaultAnimClassLayer);
	}

	CurrentHitReaction = DamagableComponent->StunMontage;
	
	AIC->SetStateAsStun();
}

void AEnemyBase::EndStunState()
{
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, "END STUN");
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	FinishersComponent->bFinishable = false;
	CurrentHitReaction = nullptr;
	DamagableComponent->ResetStun();
	AIC->SetStateAsAttacking();
}


void AEnemyBase::ShutDownAfterDeathMontage()
{
	//GEngine->AddOnScreenDebugMessage(-1, 4, FColor::Red, ("SHUT DOWN AFTER DEATH MONTAGE"));

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetMesh()->GetAnimInstance()->SetRootMotionMode(ERootMotionMode::IgnoreRootMotion);
	GetMesh()->SetCollisionObjectType(ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	if (UCharacterMovementComponent* MovementComponent = GetCharacterMovement())
	{
		MovementComponent->DisableMovement();
		MovementComponent->StopMovementImmediately();
	}

	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->WakeAllRigidBodies();
	GetMesh()->bBlendPhysics = true;

}

bool AEnemyBase::GetTokenForAvailableAttack(int32 TokenNeeded)
{
	if (CombatManager)
	{
		if (CombatManager->AvailableAttackToken > TokenNeeded)
		{
			return true;
		}
		else
		{
			return false;
		}
	}


	//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, "COMBAT MANAGER IS NULL");

	return true;
}

void AEnemyBase::InvalidateAllTimers()
{
	UE_LOG(LogTemp, Error, TEXT("INVALIDATE ENEMY TIMERS"));
	AActor* OwnerActor = GetOwner();
	FString OwnerName = OwnerActor ? *OwnerActor->GetName() : TEXT("No Owner");
	//GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Orange, FString::Printf(TEXT("INVALIDATE TIMERS, Owner Actor: %s"), *OwnerName));

	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);

	BlockTimerHandle.Invalidate();
	AttackTimerHandle.Invalidate();
	CooldownTimerHandle.Invalidate();
	UpdateAttackTimerHandle.Invalidate();
}

void AEnemyBase::DropWeapon()
{
	if (CarriedActor == nullptr)
	{
		return;
	}

	CarriedActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	if (AWeapon* WeaponClass = Cast<AWeapon>(CarriedActor))
	{
		WeaponClass->Drop();
	}
	CarriedActor = nullptr;
	AvailableAttacks = Attacks;
}

bool AEnemyBase::ShouldStopMontageForHitReaction()
{
	if (DeathMontage == nullptr && GetMesh()->GetAnimInstance()->Montage_IsPlaying(DamagableComponent->StunMontage) == false) 
	{
		return true;
	}
	return false;
}

void AEnemyBase::BlockAttackFX_Implementation(FDamageSettings DamageSettings)
{
}



void AEnemyBase::OnDeathEvent(AActor* Actor)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "ON DEATH EVENT");

	AvailableAttacks.Empty();
	Attacks.Empty();
	EndAttack();
	EndStunState();
	InvalidateAllTimers();
	GetMesh()->GetAnimInstance()->StopAllMontages(0.0f);
	DropWeapon();

	FinishersComponent->EndVulnerability();
	SetIsTargeted_Implementation(false);
	AIC->SetStateAsDead();
	AIC->BrainComponent->StopLogic("Dead");
	AIC->UnPossess();
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);

}

void AEnemyBase::OnFinisherVictimEnd()
{
	
}

bool AEnemyBase::GetCanPlayAttackMontage()
{
	if (DeathMontage || HighPriorityAttackMontage || HighPriorityStunMontage || CurrentHitReaction || GetMesh()->GetAnimInstance()->Montage_IsPlaying(CurrentDodgeMontage) || CurrentGrabPose)
	{
		EndAttack();
		return false;
	}
	return true;
}

void AEnemyBase::EndHitReaction(UAnimMontage* HitReactionMontage)
{
	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("END HIT REACTION"));
	if (HighPriorityAttackMontage != nullptr)
	{
		HighPriorityAttackMontage = nullptr;
	}

	if (CurrentHitReaction == HitReactionMontage)
	{
		CurrentHitReaction = nullptr; 
		if (!DamagableComponent->bIsStunned)
		{
			GetCharacterMovement()->bUseControllerDesiredRotation = true;
		}
	}
	if (HighPriorityStunMontage != nullptr)
	{
		if (DamagableComponent->bIsStunned)
		{
			GetCharacterMovement()->bUseControllerDesiredRotation = false;
		}
		HighPriorityStunMontage = nullptr;
	}

	EndAttack();
}

void AEnemyBase::AttackCooldownTimer(UAnimMontage* Attack)
{
	if (this == nullptr)
	{
		return;
	}
	if (DamagableComponent->bIsDead || Attack == nullptr)
	{
		return;
	}

	FAIAttacks* AttackSettings = nullptr;

	if (Attacks.Contains(Attack))
	{
		AttackSettings = Attacks.Find(Attack);
	}
	else if (WeaponAttacks.Contains(Attack))
	{
		AttackSettings = WeaponAttacks.Find(Attack);
	}

	if (AttackSettings == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("AttackSettings is null for the provided montage!"));
		return;
	}
	if (!AvailableAttacks.Contains(Attack))
	{
		AvailableAttacks.Add(Attack, *AttackSettings);
	}

	FString MontageName = Attack->GetName();
	//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("AnimMontage Name: %s"), *MontageName));
	GetWorldTimerManager().ClearTimer(AttackTimerHandle);

}

void AEnemyBase::AggroCooldownTimer()
{
	if (this == nullptr)
		return;
	bCooling = false;
	GetWorldTimerManager().ClearTimer(CooldownTimerHandle);
	//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("COOLED")));
}

void AEnemyBase::StartCooldownTimer(UAnimMontage* Attack)
{
	if (Attack == nullptr)
	{
		bCooling = true;

		GetWorld()->GetTimerManager().SetTimer(CooldownTimerHandle, this, &AEnemyBase::AggroCooldownTimer, 1.f);
		
		return;
	}

	float CooldownTime = 0.0f;
	if (Attacks.Contains(Attack))
	{
		CooldownTime = Attacks.Find(Attack)->CooldownTime;
	}
	else if (WeaponAttacks.Contains(Attack))
	{
		CooldownTime = WeaponAttacks.Find(Attack)->CooldownTime;
	}

	TWeakObjectPtr<AEnemyBase> WeakThis(this);
	GetWorld()->GetTimerManager().SetTimer(AttackTimerHandle, [WeakThis, Attack]()
		{
			if (WeakThis.IsValid())
			{
				WeakThis->AttackCooldownTimer(Attack);
			}
			else 
			{
				UE_LOG(LogTemp, Error, TEXT("AEnemyBase: TIMER WEAK POINTER NULL"));
			}
		}, CooldownTime, false);
}

void AEnemyBase::StartBlockTimer(int32 CurrentBlockedHits)
{
	TWeakObjectPtr<AEnemyBase> WeakThis(this);

	GetWorld()->GetTimerManager().SetTimer(BlockTimerHandle, [WeakThis, CurrentBlockedHits]()
		{
			if (WeakThis.IsValid())
			{
				WeakThis->BlockTimer(CurrentBlockedHits);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("AEnemyBase: TIMER WEAK POINTER NULL"));
			}
		}, 2, false);
}

void AEnemyBase::BlockTimer(int32 CurrentBlockedHits)
{
	if (this == nullptr)
		return;

	GetWorldTimerManager().ClearTimer(BlockTimerHandle);

	if (DamagableComponent->Health <= 0)
	{
		bIsBlocking = false;
		SetIsTargeted_Implementation(false);
		return;
	}

	if (bIsBlocking)
	{
		if (CurrentBlockedHits == BlockedHitCount)
		{
			SetIsTargeted_Implementation(false);
		}
		else
		{
			//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("DONT EXIT BLOCK")));
		}
	}
}

