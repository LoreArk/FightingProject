// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"

#include "Components/BoxComponent.h"

AWeapon::AWeapon()
{
	InteractCollider = CreateDefaultSubobject<UBoxComponent>(FName("Interaction Collider"));
	InteractCollider->SetupAttachment(StaticMesh);

	InteractCollider->SetCollisionProfileName(UCollisionProfile::CustomCollisionProfileName);
	InteractCollider->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	InteractCollider->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractCollider->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
}

APickableActor* AWeapon::GetCarriable_Implementation()
{
	Super::GetCarriable_Implementation();
	return this;
}

void AWeapon::Drop()
{
	StaticMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	StaticMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	StaticMesh->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	StaticMesh->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	StaticMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	StaticMesh->SetCollisionResponseToChannel(ECC_Vehicle, ECR_Block);
	StaticMesh->SetCollisionResponseToChannel(ECC_Destructible, ECR_Block);
	StaticMesh->SetSimulatePhysics(true);

	InteractCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractCollider->SetCollisionProfileName(UCollisionProfile::CustomCollisionProfileName);
	InteractCollider->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	InteractCollider->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractCollider->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
}

void AWeapon::OnPickUpHandler()
{
	Super::OnPickUpHandler();

	//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, "ON WEAPON PICKUP Collision set up");
	InteractCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
