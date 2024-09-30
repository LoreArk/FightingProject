
#include "AnimNotifyState_HoldAttack.h"
#include "Interface_Player.h"
#include "Kismet/GameplayStatics.h"
#include "FightingProjectCharacter.h"

//Keep track of held attack input during animation to perform a charged attack

void UAnimNotifyState_HoldAttack::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

    DefaultAnimPlayRate = Animation->RateScale;
    HeldAnimPlayRate = DefaultAnimPlayRate / AnimScaleDivider;
    HoldingInput = false;

    // Reset times
    HoldStartTime = 0.f;
    HoldEndTime = 0.f;
    AnimationTotalDuration = TotalDuration * DefaultAnimPlayRate / HeldAnimPlayRate;

    //GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Magenta, FString::Printf(TEXT("AnimationTotalDuration: %f"), AnimationTotalDuration));

    // Cache the player character reference once
    AActor* OwnerActor = MeshComp->GetOwner();
    if (OwnerActor && OwnerActor->GetClass()->ImplementsInterface(UInterface_Player::StaticClass()))
    {
        CachedPlayerCharacter = IInterface_Player::Execute_GetPlayerCharacter(OwnerActor);
    }
    else
    {
        CachedPlayerCharacter = nullptr;
    }

    if (CachedPlayerCharacter)
    {
        UWorld* World = MeshComp->GetWorld();
        if (!World) return;

        HoldingInput = CachedPlayerCharacter->HoldingAttackInput;

        if (HoldingInput)
        {
            if (HoldStartTime == 0.f)
            {
                HoldStartTime = World->GetTimeSeconds(); // Record start time
            }

            UGameplayStatics::PlaySoundAtLocation(MeshComp, ChargingSoundCue, MeshComp->GetComponentLocation());

            //GEngine->AddOnScreenDebugMessage(-1, 0.01f, FColor::Magenta, "HOLDING INPUT");
            Animation->RateScale = HeldAnimPlayRate;
        }

    }
}

void UAnimNotifyState_HoldAttack::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);

    if (CachedPlayerCharacter)
    {
        UWorld* World = MeshComp->GetWorld();
        if (!World) return;

        // If input is held, start tracking hold time
        if (HoldingInput == false)
        {
            // Input is released, record end time if not already recorded
            if (HoldStartTime > 0.f && HoldEndTime == 0.f)
            {
                HoldEndTime = World->GetTimeSeconds(); // Record end time
                //GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Magenta, "RELEASE HOLDING INPUT");
                Animation->RateScale = DefaultAnimPlayRate;
            }
        }
        HoldingInput = CachedPlayerCharacter->HoldingAttackInput;
    }
    
}

void UAnimNotifyState_HoldAttack::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);

    Animation->RateScale = DefaultAnimPlayRate;

    UWorld* World = MeshComp->GetWorld();
    if (!World) return;

    // If the input was still being held at the end of the AnimNotifyState
    if (HoldingInput && HoldEndTime == 0.f)
    {
        //GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Magenta, "FULL HELD ATTACK");

        UGameplayStatics::PlaySoundAtLocation(MeshComp, ChargeAttackSoundCue, MeshComp->GetComponentLocation());

        // Set end time to the current time since the input was held till the end
        HoldEndTime = World->GetTimeSeconds();
    }

    // Calculate the percentage of the hold time relative to the AnimNotifyState duration
    float HoldDuration = HoldEndTime - HoldStartTime;

    // Calculate the percentage of the hold time relative to the effective animation duration
    float HoldPercentage = (AnimationTotalDuration > 0.f) ? FMath::Clamp(HoldDuration / AnimationTotalDuration, 0.f, 1.f) : 0.f;

   // GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Magenta, FString::Printf(TEXT("HOLD DURATION: %f"), HoldDuration));
    
    if (CachedPlayerCharacter)
    {
        CachedPlayerCharacter->ChargedAttackPercentage = HoldPercentage;
    }

    // Debug message showing the percentage of holding input
    //FString DebugMessage = FString::Printf(TEXT("Holding Input Percentage: %f"), HoldPercentage);
   // GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, DebugMessage);
}
