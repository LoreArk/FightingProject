// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifyState_WallTrace.h"
#include "Interface_CombatCharacter.h"
#include "StaticFunctions.h"

void UAnimNotifyState_WallTrace::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

    OwnerActor = MeshComp->GetOwner();

}

void UAnimNotifyState_WallTrace::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);
    
    if (OwnerActor)
    {
        FVector TraceStart = OwnerActor->GetActorLocation();

        FVector TraceEnd = TraceStart + OwnerActor->GetActorForwardVector() * -100.f;

        FHitResult HitResult;
        TArray<AActor*> ActorsToIgnore = TArray<AActor*>();
        ActorsToIgnore.Add(OwnerActor);

        DrawDebugLine(OwnerActor->GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 1.0f, 0, 5.0f);

        HitResult = StaticFunctions::SphereCollisionTraceChannel(OwnerActor->GetWorld(), TraceStart, TraceEnd, ETraceTypeQuery::TraceTypeQuery1, ActorsToIgnore, 30.f);

        if (HitResult.bBlockingHit)
        {
            // Debug line to visualize the trace

            if (MeshComp->GetOwner()->GetClass()->ImplementsInterface(UInterface_CombatCharacter::StaticClass()))
            {
                IInterface_CombatCharacter::Execute_HitWall(OwnerActor, HitResult);
            }
        }
    }
}

void UAnimNotifyState_WallTrace::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);

}
