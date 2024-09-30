// Fill out your copyright notice in the Description page of Project Settings.


#include "Component_Damagable.h"
#include "GameFramework/Actor.h"
#include "StaticFunctions.h"
#include "F_HitReactionsData.h"
#include "Interface_CombatCharacter.h"

// Sets default values for this component's properties
UComponent_Damagable::UComponent_Damagable()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	Health = MaxHealth;
	bIsInvincible = false;
	CurrentFrozenState = EFrozenState::None;

	StunValue = 0.0f;
	MaxStunValue = 100.0f;
	StunIncreaseValue = 1.0f;
	StunDecreaseRate = 5.0f;
	bIsStunned = false;

	StunDuration = 10.f;
}

float UComponent_Damagable::GetMaxHealth()
{
	return MaxHealth;
}

bool UComponent_Damagable::TakeDamage(FDamageSettings DamageSettings, AActor* Instigator)
{
	bool WasDamaged = true;

	if (DamageSettings.bShouldDamageInvincible == false) 
	{
		if (bIsInvincible)
		{
			return false;
		}
		if (IInterface_CombatCharacter::Execute_GetIsBlocking(GetOwner()))
		{
			if (DamageSettings.DamageResponse == EDamageResponse::Stun) 
			{
				StunIncreaseValue = 3;
				ReceiveStun(DamageSettings.StunPower);

				StunIncreaseValue = 1;
			}
			return false;
		}
	}
	
	LastReceivedDamageSettings = DamageSettings;

	Health -= DamageSettings.DamageAmount;
	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("%.2f"), DamageSettings.DamageAmount));
	if (Health <= 0)
	{
		bIsDead = true;

		OnDeath.Broadcast(GetOwner());

		ProcessDeathHitReaction(DamageSettings, Instigator);

		return false;
	}

	ReceiveStun(DamageSettings.StunPower);
	
	LastDamageDealer = Instigator;

	return true;
}

void UComponent_Damagable::TakeFinisherDamage(FDamageSettings DamageSettings)
{
	Health -= DamageSettings.DamageAmount;
}

EHitDirection UComponent_Damagable::GetHitDirection(AActor* Instigator)
{
	AActor* Owner = GetOwner();
	FVector OwnerForward = Owner->GetActorForwardVector();
	FVector InstigatorLocation = Instigator->GetActorLocation();
	FVector OwnerLocation = Owner->GetActorLocation();

	InstigatorLocation.Z = OwnerLocation.Z;

	FVector DirectionToInstigator = InstigatorLocation - OwnerLocation;

	OwnerForward.Normalize();
	DirectionToInstigator.Normalize();

	float DotProduct = FVector::DotProduct(OwnerForward, DirectionToInstigator);

	float Angle = FMath::RadiansToDegrees(FMath::Acos(DotProduct));
	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("%.2f"), Angle));

	FVector CrossProduct = FVector::CrossProduct(OwnerForward, DirectionToInstigator);
	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("%.2f"), CrossProduct.Z));

	EHitDirection HitDirection = EHitDirection::Front;

	if (Angle < 45) 
	{
		HitDirection = EHitDirection::Front;
	}
	else if (Angle > 135) 
	{
		HitDirection = EHitDirection::Back;
	}
	else 
	{
		if (CrossProduct.Z < 0) 
		{
			HitDirection = EHitDirection::Left;
		}
		else if (CrossProduct.Z > 0)
		{
			HitDirection = EHitDirection::Right;
		}
	}

	return HitDirection;

}

void UComponent_Damagable::ProcessDamageForHitReaction(FDamageSettings DamageSettings, AActor* DamageInstigator)
{
	//Check if special hit damage response should be applied, otherwise apply normal hit reaction. 
	// Poise generally blocks special hit reaction, but Poise is ignored by: non phys damage or if already in a special effect state (Current Frozen State)

	if (PoiseValue >= 1.0f && DamageSettings.DamageType != EDamageType::Physical || PoiseValue >= 1.0f && CurrentFrozenState != EFrozenState::None || !bHasPoise)
	{
		if (DamageSettings.DamageResponse == EDamageResponse::Launch)
		{
			int32 CurrentIndex = 0;
			int32 TargetIndex = 0;
			UAnimMontage* Key = nullptr;
			CurrentFrozenState = EFrozenState::Launched;
			
			if (DamageSettings.ChargedAttackPercentage > 0.88f)
			{
				TargetIndex = 1;
			}

			for (TMap<UAnimMontage*, FHitReactionSettings>::TIterator It = LaunchHitReactions.CreateIterator(); It; ++It)
			{
				if (CurrentIndex == TargetIndex)
				{
					Key = It.Key();
					break;
				}
				CurrentIndex++;
			}

			if (Key)
			{
				IInterface_CombatCharacter::Execute_PlayHitReaction(GetOwner(), Key, nullptr);
			}

			return;
		}
		if (CurrentFrozenState == EFrozenState::Juggling)
		{
			TMap<UAnimMontage*, FHitReactionSettings>::TIterator It = JuggleHitReactions.CreateIterator();
			UAnimMontage* Key = It.Key();
			CurrentFrozenState = EFrozenState::Juggling;
			//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("REPEAT JUGGLE"));

			IInterface_CombatCharacter::Execute_PlayHitReaction(GetOwner(), Key, nullptr);
			return;
		}
		if (DamageSettings.DamageResponse == EDamageResponse::Juggle)
		{
			int32 CurrentIndex = 0;
			int32 TargetIndex = 0;
			UAnimMontage* Key = nullptr;
			CurrentFrozenState = EFrozenState::Juggling;
			//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT(" JUGGLE"));

			if (DamageSettings.ChargedAttackPercentage > 0.88f)
			{
				TargetIndex = 1;
			}

			for (TMap<UAnimMontage*, FHitReactionSettings>::TIterator It = JuggleHitReactions.CreateIterator(); It; ++It)
			{
				if (CurrentIndex == TargetIndex)
				{
					Key = It.Key();
					break;
				}
				CurrentIndex++;
			}

			if (Key)
			{
				IInterface_CombatCharacter::Execute_PlayHitReaction(GetOwner(), Key, nullptr);
			}
			return;
		}
		if (DamageSettings.DamageResponse == EDamageResponse::Fall && CurrentFrozenState != EFrozenState::Fell)
		{
			TMap<UAnimMontage*, FHitReactionSettings>::TIterator It = FallHitReactions.CreateIterator();
			UAnimMontage* Key = It.Key();
			CurrentFrozenState = EFrozenState::Fell;

			IInterface_CombatCharacter::Execute_PlayHitReaction(GetOwner(), Key, nullptr);

			return;
		}
		if (CurrentFrozenState == EFrozenState::Fell)
		{
			TMap<UAnimMontage*, FHitReactionSettings>::TIterator It = LayingHitReactions.CreateIterator();
			UAnimMontage* Key = It.Key();
			CurrentFrozenState = EFrozenState::Fell;

			IInterface_CombatCharacter::Execute_PlayHitReaction(GetOwner(), Key, nullptr);

			return;
		}
		
	}

	if (CurrentFrozenState == EFrozenState::Launched)
		return;

	//No special hit reaction applied, apply normal hit reaction

	EHitDirection OtherPosition = GetHitDirection(DamageInstigator);

	UAnimMontage* TargetMontage = FindHitReactionMontage(DamageSettings, DamageSettings.HitType, OtherPosition);

	if (TargetMontage != nullptr)
	{
		//FString MontageName = TargetMontage->GetName();
		//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Emerald, MontageName);
		IInterface_CombatCharacter::Execute_PlayHitReaction(GetOwner(), TargetMontage, nullptr);
	}
}

void UComponent_Damagable::ProcessDeathHitReaction(FDamageSettings DamageSettings, AActor* DamageInstigator)
{
	EHitDirection HitDirection = GetHitDirection(DamageInstigator);

	if (DamageSettings.DamageResponse == EDamageResponse::Launch)
	{
		TMap<UAnimMontage*, FHitReactionSettings>::TIterator It = DeathLaunchMontages.CreateIterator();
		UAnimMontage* Key = It.Key();

		IInterface_CombatCharacter::Execute_PlayDeathMontage(GetOwner(), Key);

		return;
	}
	if (DamageSettings.DamageResponse == EDamageResponse::Juggle || CurrentFrozenState == EFrozenState::Juggling)
	{
		TMap<UAnimMontage*, FHitReactionSettings>::TIterator It = DeathJuggleMontages.CreateIterator();
		UAnimMontage* Key = It.Key();

		IInterface_CombatCharacter::Execute_PlayDeathMontage(GetOwner(), Key);

		return;
	}
	if (CurrentFrozenState == EFrozenState::Fell)
	{
		TMap<UAnimMontage*, FHitReactionSettings>::TIterator It = DeathFallMontages.CreateIterator();
		UAnimMontage* Key = It.Key();
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Magenta, ("DEATH MONTAGE NULL"));

		IInterface_CombatCharacter::Execute_PlayDeathMontage(GetOwner(), Key);

		return;
	}

	UAnimMontage* TargetMontage = FindDeathMontage(DamageSettings, DamageSettings.HitType, HitDirection);

	if (TargetMontage == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Magenta, ("DEATH MONTAGE NULL"));
	}

	if (TargetMontage != nullptr)
	{
		FString MontageName = TargetMontage->GetName();
		//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Magenta, ("FOUND DEATH MONTAGE %d:", MontageName));

		//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Emerald, MontageName);
		IInterface_CombatCharacter::Execute_PlayDeathMontage(GetOwner(), TargetMontage);
	}
}

UAnimMontage* UComponent_Damagable::FindHitReactionMontage(FDamageSettings DamageSettings, EHitType HitType, EHitDirection OtherPosition)
{
	EDamageResponse DamageResponseType = DamageSettings.DamageResponse;
	TMap<UAnimMontage*, FHitReactionSettings> HitDamageResponseMap;

	if (bReceiveStagger && !bIsStaggered)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Magenta, ("STAGGER HIT REACTION"));

		DamageResponseType = EDamageResponse::Stagger;
	}

	//Select the map of corresponding hit type -- if this method is executed, Launch, Stun, Fall, Juggle effects are not applied and they correspond to Medium Hit Reactions
	switch (DamageResponseType)
	{
	case EDamageResponse::None:
		HitDamageResponseMap = WeakHitReactions;
		break;
	case EDamageResponse::Weak:
		HitDamageResponseMap = WeakHitReactions;
		break;
	case EDamageResponse::Med:
		HitDamageResponseMap = MedHitReactions;
		break;
	case EDamageResponse::Stun:
		HitDamageResponseMap = MedHitReactions;
		break;
	case EDamageResponse::Stagger:
		HitDamageResponseMap = StaggerHitReactions;
		break;
	case EDamageResponse::Launch:
		HitDamageResponseMap = MedHitReactions;
		break;
	case EDamageResponse::Fall:
		HitDamageResponseMap = MedHitReactions;
		break;
	case EDamageResponse::Juggle:
		HitDamageResponseMap = MedHitReactions;
		break;
	default:
		break;
	}

	if (WeakHitReactions.Num() > 0)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, ("WEAK HIT REACTIONS ARE HERE"));
	}

	//Select hit reactions based on hit type (high, low, med)
	TMap<UAnimMontage*, FHitReactionSettings> HitTypeReactions;
	if (DamageResponseType == EDamageResponse::Stagger)
	{
		HitType = EHitType::Mid;
	}
	for (const TPair<UAnimMontage*, FHitReactionSettings>& Kvp : HitDamageResponseMap)
	{
		if (Kvp.Value.HitType == HitType)
		{
			HitTypeReactions.Add(Kvp.Key, Kvp.Value);
		}
	}

	//Get hit of correct direction
	UAnimMontage* SelectedMontage = nullptr;
	EHitDirection AttackDirection = DamageSettings.HitDirection; //Direction of the animation attack received

	EHitDirection HitReactionDirection = GetHitReactionDirection(OtherPosition, AttackDirection);

	for (const TPair<UAnimMontage*, FHitReactionSettings>& Kvp : HitTypeReactions)
	{
		if (Kvp.Value.HitDirection == HitReactionDirection)
		{
			SelectedMontage = Kvp.Key;
		}
	}
	if (SelectedMontage == nullptr)
	{
		for (const TPair<UAnimMontage*, FHitReactionSettings>& Kvp : HitTypeReactions)
		{
			if (Kvp.Value.HitDirection == EHitDirection::Front)
			{
				SelectedMontage = Kvp.Key;
			}
		}
	}
	return SelectedMontage;
}

UAnimMontage* UComponent_Damagable::FindDeathMontage(FDamageSettings DamageSettings, EHitType HitType, EHitDirection OtherPosition)
{
	EDamageResponse DamageResponseType = DamageSettings.DamageResponse;

	//Select hit reactions based on hit type (high, low, med)
	TMap<UAnimMontage*, FHitReactionSettings> HitTypeReactions;
	for (const TPair<UAnimMontage*, FHitReactionSettings>& Kvp : DeathMontages)
	{
		if (Kvp.Value.HitType == HitType)
		{
			
			HitTypeReactions.Add(Kvp.Key, Kvp.Value);
		}
	}

	//Get hit of correct direction
	UAnimMontage* SelectedMontage = nullptr;
	EHitDirection AttackDirection = DamageSettings.HitDirection; //Direction of the animation attack received

	EHitDirection HitReactionDirection = GetHitReactionDirection(OtherPosition, AttackDirection);

	for (const TPair<UAnimMontage*, FHitReactionSettings>& Kvp : HitTypeReactions)
	{
		if (Kvp.Value.HitDirection == HitReactionDirection)
		{
			SelectedMontage = Kvp.Key;
		}
	}
	if (SelectedMontage == nullptr)
	{
		for (const TPair<UAnimMontage*, FHitReactionSettings>& Kvp : HitTypeReactions)
		{
			if (Kvp.Value.HitDirection == EHitDirection::Front)
			{
				SelectedMontage = Kvp.Key;
			}
		}
	}
	return SelectedMontage;
}

EHitDirection UComponent_Damagable::GetHitReactionDirection(EHitDirection OtherPosition, EHitDirection AttackDirection)
{

	EHitDirection HitReactionDirection = EHitDirection::Front; //Direction of the hit reaction to find

	if (OtherPosition == EHitDirection::Front)
	{
		if (AttackDirection == EHitDirection::Front)
		{
			HitReactionDirection = EHitDirection::Front;
		}
		else if (AttackDirection == EHitDirection::Right)
		{
			HitReactionDirection = EHitDirection::Right;
		}
		else if (AttackDirection == EHitDirection::Left)
		{
			HitReactionDirection = EHitDirection::Left;
		}
	}
	else if (OtherPosition == EHitDirection::Back)
	{
		if (AttackDirection == EHitDirection::Front)
		{
			HitReactionDirection = EHitDirection::Back;
		}
		else if (AttackDirection == EHitDirection::Right)
		{
			HitReactionDirection = EHitDirection::Right;
		}
		else if (AttackDirection == EHitDirection::Left)
		{
			HitReactionDirection = EHitDirection::Left;
		}
	}
	else if (OtherPosition == EHitDirection::Right)
	{
		HitReactionDirection = EHitDirection::Right;
	}
	else if (OtherPosition == EHitDirection::Left)
	{
		HitReactionDirection = EHitDirection::Left;
	}


	return HitReactionDirection;
}

void UComponent_Damagable::ReceiveStun(float StunPower)
{
	// Increase the stun value based on the hit's stun power and the component's stun increase value

	if (bIsStunned)
		return;

	StunValue += StunPower * StunIncreaseValue;

	if (StunValue >= MaxStunValue)
	{
		SetHasPoise(false);
		bIsStunned = true;
		StunValue = MaxStunValue;

		IInterface_CombatCharacter::Execute_PlayHitReaction(GetOwner(), StunMontage, nullptr);
		OnStun.Broadcast();
	}
}


void UComponent_Damagable::DecreaseStun(float DeltaTime)
{
	if (StunValue > 0.0f)
	{
		StunValue -= StunDecreaseRate * DeltaTime;
		if (StunValue < 0.0f)
		{
			StunValue = 0.0f;
		}
	}
}

void UComponent_Damagable::ResetStun()
{
	StunValue = 0.0f;
	SetHasPoise(true);
	bIsStunned = false;
	StunIncreaseValue = 1.0f;
	StunDecreaseRate = 5.0f;
}

void UComponent_Damagable::SetHasPoise(bool HasPoise)
{
	if (PoiseValue >= 1.f)
	{
		bHasPoise = HasPoise;
	}

	bReceiveStagger = !HasPoise;
}


void UComponent_Damagable::BeginPlay()
{
	Super::BeginPlay();

	if (HitReactionsDataTable)
	{
		F_HitReactionsData* HitsDataWeak = HitReactionsDataTable->FindRow<F_HitReactionsData>("Weak", TEXT(""));
		if (HitsDataWeak)
		{
			WeakHitReactions = HitsDataWeak->HitReactions;
		}

		F_HitReactionsData* HitsDataMed = HitReactionsDataTable->FindRow<F_HitReactionsData>("Med", TEXT(""));
		if (HitsDataMed)
		{
			MedHitReactions = HitsDataMed->HitReactions;
		}

		F_HitReactionsData* HitsDataStun = HitReactionsDataTable->FindRow<F_HitReactionsData>("Stun", TEXT(""));
		if (HitsDataStun)
		{
			StunHitReactions = HitsDataStun->HitReactions;
		}

		F_HitReactionsData* HitsDataLaunch = HitReactionsDataTable->FindRow<F_HitReactionsData>("Launch", TEXT(""));
		if (HitsDataLaunch)
		{
			LaunchHitReactions = HitsDataLaunch->HitReactions;
		}

		F_HitReactionsData* HitsDataFall = HitReactionsDataTable->FindRow<F_HitReactionsData>("Fall", TEXT(""));
		if (HitsDataFall)
		{
			FallHitReactions = HitsDataFall->HitReactions;
		}

		F_HitReactionsData* HitsDataJuggle = HitReactionsDataTable->FindRow<F_HitReactionsData>("Juggle", TEXT(""));
		if (HitsDataJuggle)
		{
			JuggleHitReactions = HitsDataJuggle->HitReactions;
		}

		F_HitReactionsData* HitsDataLaying = HitReactionsDataTable->FindRow<F_HitReactionsData>("Laying", TEXT(""));
		if (HitsDataLaying)
		{
			LayingHitReactions = HitsDataLaying->HitReactions;
		}

		F_HitReactionsData* HitsDataStagger = HitReactionsDataTable->FindRow<F_HitReactionsData>("Stagger", TEXT(""));
		if (HitsDataStagger)
		{
			StaggerHitReactions = HitsDataStagger->HitReactions;
		}

		F_HitReactionsData* HitsDataDeath = HitReactionsDataTable->FindRow<F_HitReactionsData>("Death", TEXT(""));
		if (HitsDataDeath)
		{
			DeathMontages = HitsDataDeath->HitReactions;
		}

		F_HitReactionsData* HitsDataDeathLaunch = HitReactionsDataTable->FindRow<F_HitReactionsData>("DeathLaunch", TEXT(""));
		if (HitsDataDeathLaunch)
		{
			DeathLaunchMontages = HitsDataDeathLaunch->HitReactions;
		}

		F_HitReactionsData* HitsDataDeathFell = HitReactionsDataTable->FindRow<F_HitReactionsData>("DeathFell", TEXT(""));
		if (HitsDataDeathLaunch)
		{
			DeathFallMontages = HitsDataDeathFell->HitReactions;
		}

		F_HitReactionsData* HitsDataDeathJuggle = HitReactionsDataTable->FindRow<F_HitReactionsData>("DeathJuggle", TEXT(""));
		if (HitsDataDeathLaunch)
		{
			DeathJuggleMontages = HitsDataDeathJuggle->HitReactions;
		}
	}

	if (PoiseValue >= 1.0f)
	{
		bHasPoise = true;
	}
}


void UComponent_Damagable::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bIsStunned)
	{
		DecreaseStun(DeltaTime);
	}
	/*if (bReceiveStagger)
	{
		DrawDebugSphere(GetWorld(), GetOwner()->GetActorLocation(), 50.0f, 12, FColor::Red, false, 0.1f);
	}*/

}

float UComponent_Damagable::GetHealth()
{
	return Health;
}

