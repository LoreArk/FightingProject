// Fill out your copyright notice in the Description page of Project Settings.


#include "StaticFunctions.h"
#include "F_AttackSettings.h"
#include "Kismet/KismetSystemLibrary.h"

FHitResult StaticFunctions::SphereCollisionTraceObject(UObject* WorldContextObject, FVector Start, FVector End, TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes, TArray<AActor*> ActorsToIgnore, float Radius)
{
	FHitResult HitResult;

	bool bHasHit =
		UKismetSystemLibrary::SphereTraceSingleForObjects
		(
			WorldContextObject,
			Start,
			End,
			Radius,
			ObjectTypes,
			true,
			ActorsToIgnore,
			EDrawDebugTrace::None,
			HitResult,
			true,
			FLinearColor::Red, FLinearColor::Green, 0.1f
		);

	return HitResult;
}

FHitResult StaticFunctions::SphereCollisionTraceChannel(UObject* WorldContextObject, FVector Start, FVector End, ETraceTypeQuery TraceChannel, TArray<AActor*> ActorsToIgnore, float Radius)
{
	FHitResult HitResult;


	bool bHasHit =
		UKismetSystemLibrary::SphereTraceSingle
		(
			WorldContextObject,
			Start,
			End,
			Radius,
			TraceChannel,
			true,
			ActorsToIgnore,
			EDrawDebugTrace::None,
			HitResult,
			true,
			FLinearColor::Red, FLinearColor::Green, 0.1f
		);

	return HitResult;
}

TArray<FHitResult> StaticFunctions::SphereCollisionMultiTraceByChannel(UObject* WorldContextObject, FVector Start, FVector End, ETraceTypeQuery TraceChannel, TArray<AActor*> ActorsToIgnore, float Radius)
{

	TArray<FHitResult> OutHits;
	bool bHasHit = UKismetSystemLibrary::SphereTraceMulti(
		WorldContextObject,
		Start,
		End,
		Radius,
		TraceChannel,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		OutHits,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		5.0f
	);


	return OutHits;
}

TPair<FName, FName> StaticFunctions::GetSocketNameFromFightTraceEnum(EFightTrace FightTrace)
{

	TPair<FName, FName> FightTracePair = TPair<FName, FName>();

	switch (FightTrace)
	{
	case EFightTrace::RightHand:
		FightTracePair.Key = "HandR_Start";
		FightTracePair.Value = "HandR_End";
		break;
	case EFightTrace::LeftHand:
		FightTracePair.Key = "HandL_Start";
		FightTracePair.Value = "HandL_End";
		break;
	case EFightTrace::RightFoot:
		FightTracePair.Key = "FootR_Start";
		FightTracePair.Value = "FootR_End";
		break;
	case EFightTrace::LeftFoot:
		FightTracePair.Key = "FootL_Start";
		FightTracePair.Value = "FootL_End";
		break;
	case EFightTrace::RightKnee:
		FightTracePair.Key = "KneeR_Start";
		FightTracePair.Value = "KneeR_End";
		break;
	case EFightTrace::LeftKnee:
		FightTracePair.Key = "KneeL_Start";
		FightTracePair.Value = "KneeL_End";
		break;
	case EFightTrace::RightElbow:
		FightTracePair.Key = "ElbowR_Start";
		FightTracePair.Value = "ElbowR_End";
		break;
	case EFightTrace::LeftElbow:
		FightTracePair.Key = "ElbowL_Start";
		FightTracePair.Value = "ElbowL_End";
		break;
	case EFightTrace::Body:
		FightTracePair.Key = "Body_Start";
		FightTracePair.Value = "Body_End";
	case EFightTrace::Club:
		FightTracePair.Key = "Club_Start";
		FightTracePair.Value = "Club_End";
	case EFightTrace::Katana:
		FightTracePair.Key = "Katana_Start";
		FightTracePair.Value = "Katana_End";
	default:
		break;
	}
	
	return FightTracePair;
}

float StaticFunctions::GetAngleFromActorAToB(AActor* ActorA, AActor* ActorB)
{
	FVector ForwardA = ActorA->GetActorForwardVector();
	FVector LocationA = ActorA->GetActorLocation();
	FVector LocationB = ActorB->GetActorLocation();
	FVector AToBDirection = LocationB - LocationA;
	AToBDirection.Normalize();
	ForwardA.Normalize();

	float DotProduct = FVector::DotProduct(ActorA->GetActorRightVector(), AToBDirection);

	float Angle = FMath::RadiansToDegrees(FMath::Acos(DotProduct));

	return Angle;
}

float StaticFunctions::GetAngleFromActorWithSideVector(AActor* ActorA, AActor* ActorB)
{
	FVector DodgerLocation = ActorB->GetActorLocation();
	FVector AttackerLocation = ActorA->GetActorLocation();
	FVector AttackerRightVector = ActorA->GetActorRightVector();
	FVector AttackerForwardVector = ActorA->GetActorForwardVector();

	// Calculate the direction from ActorA to ActorB
	FVector DirectionToDodger = DodgerLocation - AttackerLocation;
	DirectionToDodger.Normalize();

	// Cross product to determine if ActorB is to the left or right of ActorA
	FVector CrossProduct = FVector::CrossProduct(DirectionToDodger, AttackerForwardVector);
	float SideValue = FVector::DotProduct(CrossProduct, FVector(0, 0, 1)); // Use Z component for up vector

	// Return the Z component of the cross product
	return SideValue;
}


FVector StaticFunctions::GetLocationAtDistanceXFromActorB(AActor* ActorA, AActor* ActorB, float Distance)
{
	if (!ActorA || !ActorB)
	{
		return FVector::ZeroVector;
	}

	// Get the locations of Actor A and Actor B
	FVector LocationA = ActorA->GetActorLocation();
	FVector LocationB = ActorB->GetActorLocation();

	// Calculate the direction from Actor B to Actor A
	FVector Direction = LocationA - LocationB;

	// Normalize the direction to get the unit vector
	FVector NormalizedDirection = Direction.GetSafeNormal();

	// Scale the normalized direction by the desired distance
	FVector TargetLocation = LocationB + NormalizedDirection * Distance;

	return TargetLocation;
}

FRotator StaticFunctions::GetRotationForFacingDirection(AActor* Actor, AActor* ActorToFace)
{
	FVector DirectionToFace = (ActorToFace->GetActorLocation() - Actor->GetActorLocation()).GetSafeNormal();
	FRotator TargetRotation = DirectionToFace.Rotation();

	return TargetRotation;
}

FName StaticFunctions::GetFXSocketNameFromFightTrace(EFightTrace FightTrace)
{
	FName SocketName = "";
	switch (FightTrace)
	{
	case EFightTrace::RightHand:
		SocketName = "FX_HandR";
		break;
	case EFightTrace::LeftHand:
		SocketName = "FX_HandL";
		break;
	case EFightTrace::RightFoot:
		SocketName = "FX_FootR";
		break;
	case EFightTrace::LeftFoot:
		SocketName = "FX_FootL";
		break;
	case EFightTrace::RightKnee:
		SocketName = "FX_KneeR";
		break;
	case EFightTrace::LeftKnee:
		SocketName = "FX_KneeL";
		break;
	case EFightTrace::RightElbow:
		SocketName = "FX_ElbowR";
		break;
	case EFightTrace::LeftElbow:
		SocketName = "FX_ElbowL";
		break;
	case EFightTrace::Body:
		break;
	case EFightTrace::Club:
		break;
	case EFightTrace::Katana:
		break;
	default:
		break;
	}
	return SocketName;
}

float StaticFunctions::GetHorizontalDistanceBetweenActors(AActor* ActorA, AActor* ActorB)
{
	if (!ActorA || !ActorB) return -1.0f; // Return -1.0f if either actor is null

	FVector Location1 = ActorA->GetActorLocation();
	FVector Location2 = ActorB->GetActorLocation();

	Location1.Z = 0.0f;
	Location2.Z = 0.0f;

	return FVector::Dist(Location1, Location2);
}
