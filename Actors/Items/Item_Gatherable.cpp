// Fill out your copyright notice in the Description page of Project Settings.


#include "Item_Gatherable.h"
#include "Interface_Player.h"
#include "Components/SphereComponent.h"
#include "StaticFunctions.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/PrimitiveComponent.h"


AItem_Gatherable::AItem_Gatherable()
{
    PrimaryActorTick.bCanEverTick = true;

    SphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollider"));
    SphereCollider->InitSphereRadius(100.0f);
    SphereCollider->SetupAttachment(RootComponent);

    SphereCollider->OnComponentBeginOverlap.AddDynamic(this, &AItem_Gatherable::OnBeginOverlap);
    SphereCollider->OnComponentEndOverlap.AddDynamic(this, &AItem_Gatherable::OnEndOverlap);

    ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemStaticMesh"));
    ItemMesh->SetupAttachment(SphereCollider);
    
    ReachTargetLocationSpeed = 1.f;
    RotationSpeed = 200.f;
}

void AItem_Gatherable::Interact_Implementation()
{
    if (InteractingActor)
    {
        AActor* Gatherable = IInterface_Player::Execute_GetGatherableItem(InteractingActor);

        if (Gatherable == this)
        {
            IInterface_Player::Execute_SetGatherableItem(InteractingActor, nullptr);
            InteractingActor = nullptr;
        }
    }
    Destroy();
}

void AItem_Gatherable::BeginPlay()
{
    Super::BeginPlay();

    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(this);

    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel6));

    FHitResult HitResult = StaticFunctions::SphereCollisionTraceObject(GetWorld(), GetActorLocation(), GetActorLocation(), ObjectTypes, ActorsToIgnore, 100.f);

    if (HitResult.bBlockingHit)
    {
        bool bIsPlayer = HitResult.GetActor()->GetClass()->ImplementsInterface(UInterface_Player::StaticClass());

        if (bIsPlayer)
        {
            InteractingActor = HitResult.GetActor();
            IInterface_Player::Execute_SetGatherableItem(HitResult.GetActor(), this);
        }
    }

    FVector Start = GetActorLocation();
    FVector End = Start - FVector(0.0f, 0.0f, 10000.0f);
    TargetLocation = Start;

    FHitResult HitScenario;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this); // Ignore the game mode actor

    UWorld* World = GetWorld();
    if (World && World->LineTraceSingleByChannel(HitScenario, Start, End, ECC_Visibility, QueryParams))
    {
        AActor* HitActor = HitScenario.GetActor();

        if (HitActor)
        {
            // Print the name of the hit actor
            FString ActorName = HitActor->GetName();
            //GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString::Printf(TEXT("Hit Actor: %s"), *ActorName));
        }

        float DistanceToGround = (Start.Z - HitScenario.ImpactPoint.Z);
        float DesiredHeightAboveGround = 50.0f; // Change this to your desired height above the ground

        if (DistanceToGround > DesiredHeightAboveGround)
        {
            TargetLocation.Z = HitScenario.ImpactPoint.Z + DesiredHeightAboveGround;
        }
       // DrawDebugLine(World, Start, TargetLocation, FColor::Red, false, 1.0f, 0, 1.0f);
    }

    bShouldLerpToLocation = true;
}

// Called every frame
void AItem_Gatherable::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bShouldLerpToLocation == true)
    {
        float TargetLocationDistance = UKismetMathLibrary::Vector_Distance(GetActorLocation(), TargetLocation);
        if (TargetLocationDistance > 1.f)
        {
            FVector LerpLocation = FMath::Lerp(GetActorLocation(), TargetLocation, DeltaTime * ReachTargetLocationSpeed);

            SetActorLocation(LerpLocation);
        }
        else
        {
            bShouldLerpToLocation = false;
        }
    }
   
    FRotator Rotation = FRotator(0.f, DeltaTime * RotationSpeed, 0.f);
    AddActorWorldRotation(Rotation);

    if (InteractingActor)
    {
        AActor* Gatherable = IInterface_Player::Execute_GetGatherableItem(InteractingActor);

        if (!IsOverlappingActor(InteractingActor) && Gatherable == this)
        {
            IInterface_Player::Execute_SetGatherableItem(InteractingActor, nullptr);
            InteractingActor = nullptr;
        }
    }
}

void AItem_Gatherable::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && OtherActor != this)
    {
      //  GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Green,
      //      FString::Printf(TEXT("Overlap Begin: %s"), *OtherActor->GetName()));

        bool bIsPlayer = OtherActor->GetClass()->ImplementsInterface(UInterface_Player::StaticClass());

        if (bIsPlayer)
        {
            InteractingActor = OtherActor;
            IInterface_Player::Execute_SetGatherableItem(OtherActor, this);
        }
    }
}

void AItem_Gatherable::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    /*
    if (OtherActor && OtherActor != this)
    {
       // GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Red,
        //    FString::Printf(TEXT("Overlap End: %s"), *OtherActor->GetName()));

        bool bIsPlayer = OtherActor->GetClass()->ImplementsInterface(UInterface_Player::StaticClass());

        if (bIsPlayer)
        {
           AActor* Gatherable = IInterface_Player::Execute_GetGatherableItem(OtherActor);
           
           if (Gatherable != this)
               return;

           InteractingActor = nullptr;
           IInterface_Player::Execute_SetGatherableItem(OtherActor, nullptr);
        }
    }*/
}

