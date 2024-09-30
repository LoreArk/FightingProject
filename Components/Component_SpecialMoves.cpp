// Fill out your copyright notice in the Description page of Project Settings.


#include "Component_SpecialMoves.h"
#include "F_AttackSettings.h"
#include "FightingProject/FightingProjectCharacter.h"
#include "Interface_Player.h"
#include "DrawDebugHelpers.h" 
#include <LevelSequenceActor.h>
#include <LevelSequencePlayer.h>
#include <MovieSceneSequencePlayer.h>

UComponent_SpecialMoves::UComponent_SpecialMoves()
{
	PrimaryComponentTick.bCanEverTick = true;


	OrbsNumber = MaxOrbsNumber;
}

void UComponent_SpecialMoves::BeginPlay()
{
	Super::BeginPlay();

	OrbsNumber = MaxOrbsNumber;
}


void UComponent_SpecialMoves::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UComponent_SpecialMoves::OpenMenu()
{
	GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, ("SPECIAL MOVES MENU OPENED"));
}

void UComponent_SpecialMoves::CloseMenu()
{
	GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, ("SPECIAL MOVES MENU CLOSED"));

}

void UComponent_SpecialMoves::SpawnGun()
{
	GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, ("SPAWN GUN"));
}

void UComponent_SpecialMoves::SpawnProjectile(TSubclassOf<AProjectile> Projectile, USkeletalMeshComponent* SkeletalMesh, FName SocketName, FDamageSettings DamageSettings)
{
	//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, ("SPAWN PROJECTILE"));
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();
	SpawnParams.Instigator = GetOwner()->GetInstigator();

	FVector MuzzleLocation = SkeletalMesh->GetSocketLocation(SocketName);
	FRotator MuzzleRotation = SkeletalMesh->GetSocketRotation(SocketName);


	UWorld* World = GetWorld();
	if (World)
	{
		AProjectile* SpawnedProjectile = World->SpawnActor<AProjectile>(Projectile, MuzzleLocation, MuzzleRotation, SpawnParams);

		SpawnedProjectile->DamageSettings = DamageSettings;
	}
}


bool UComponent_SpecialMoves::HasAvailableMoves()
{
	for (const TPair<UAnimMontage*, FSpecialAttackSettings>& Kvp : SpecialMoves)
	{
		if (Kvp.Value.OrbsCost <= OrbsNumber)
		{
			return true;
		}
	}
	return false;
}



