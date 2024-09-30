// Copyright Epic Games, Inc. All Rights Reserved.

#include "FightingProjectCharacter.h"
#include "CombatCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "MotionWarpingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Pistol.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"
#include "EnhancedInputComponent.h"
#include "F_AttackData.h"
#include "F_AttackSettings.h"
#include "Engine/DataTable.h"
#include "EnhancedInputSubsystems.h"
#include "Interface_Interactable.h"
#include "Interface_Enemy.h"
#include "EnemyBase.h"
#include "Weapon.h"
#include "Interface_Carriable.h"
#include "Interface_Enemy.h"
#include "PickableActor.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "StaticFunctions.h"
#include "Components/WidgetComponent.h"
#include "StatsHUD.h"
#include "Public/Component_Damagable.h"
#include "FighterGameInstance.h"


AFightingProjectCharacter::AFightingProjectCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 400.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	SpecialActionsCameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpecialActionsCameraBoom"));
	SpecialActionsCameraBoom->SetupAttachment(RootComponent);
	SpecialActionsCameraBoom->bUsePawnControlRotation = false;

	HoldingObjectCameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("LeftSideCameraBoom"));
	HoldingObjectCameraBoom->SetupAttachment(RootComponent);
	HoldingObjectCameraBoom->bUsePawnControlRotation = true;

	GrabCameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("GrabPoseCameraBoom"));
	//GrabCameraBoom->SetupAttachment(GetMesh(), "spine_04");
	GrabCameraBoom->SetupAttachment(RootComponent);
	GrabCameraBoom->bUsePawnControlRotation = false;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("DefaultCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = true; 

	SpecialMovesComponent = CreateDefaultSubobject<UComponent_SpecialMoves>(TEXT("Special Moves Component"));
	InputsComponent = CreateDefaultSubobject<UComponent_Inputs>(TEXT("Inputs Component"));
	AttacksComponent = CreateDefaultSubobject<UComponent_Attacks>(TEXT("Attacks Component"));

	bCanDodge = true;
	bCanTurn180 = true;
	bCanAct = true;

}

void AFightingProjectCharacter::BeginPlay()
{
	Super::BeginPlay();

	UGameInstance* instance = UGameplayStatics::GetGameInstance(GetWorld());
	UFighterGameInstance* GameInstance = Cast<UFighterGameInstance>(instance);
	if(GameInstance->bShouldLoadData)
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, "PLAYER DATA LOAD");
	
	LoadPersistentData();

	DoOnce.Reset();

	if (HudClass)
	{
		StatsHud = CreateWidget<UStatsHUD>(UGameplayStatics::GetPlayerController(GetWorld(), 0), HudClass);
		StatsHud->AddToViewport();
		StatsHud->SetVisibility(ESlateVisibility::Hidden);
		StatsHud->SetVisibility(ESlateVisibility::Visible);
	}

	StatsHud->SetDamagableComponent(DamagableComponent);
	StatsHud->SetSpecialMovesComponent(SpecialMovesComponent);
	StatsHud->UpdateOrbHUD();

	DamagableComponent->OnDeath.AddDynamic(this, &AFightingProjectCharacter::OnDeathEvent);
	InputsComponent->OnExitSpecialMoveMenu.AddDynamic(this, &AFightingProjectCharacter::ExitSpecialAttackSelection);
	InputsComponent->OnSelectSpecialMove.AddDynamic(this, &AFightingProjectCharacter::HandleSpecialAttackRequest);
	InputsComponent->OnGrabActionEvent.AddDynamic(this, &AFightingProjectCharacter::PerformGrabAttack);

	UE_LOG(LogTemp, Warning, TEXT(" Map contains %d elements"), DamagableComponent-> WeakHitReactions.Num());
}

void AFightingProjectCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (StatsHud != nullptr)
	{
		StatsHud->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("StatsHud is null in PossessedBy."));
	}

	if (NewController)
	{
		if (APlayerController* PlayerController = Cast<APlayerController>(NewController))
		{
			if (ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
			{
				if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
				{
					Subsystem->AddMappingContext(InputsComponent->DefaultMappingContext, 0);
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Subsystem is null in PossessedBy."));
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("LocalPlayer is null in PossessedBy."));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("NewController is not a PlayerController in PossessedBy."));
		}
	}
}

void AFightingProjectCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	HandleMovementInput();

	if (AttackTarget && bDodgeStance && PriorityHitMontage == nullptr)
	{
		FVector PlayerLocation = GetActorLocation();
		FVector TargetLocation = AttackTarget->GetActorLocation();
		FRotator LookAtRotation = (TargetLocation - PlayerLocation).Rotation();

		FRotator NewRotation = FRotator(0.0f, LookAtRotation.Yaw, 0.0f);
		FRotator NewControlRotation = FRotator(LookAtRotation.Pitch, LookAtRotation.Yaw, 0.0f);
		
		PlayerLocation.Z = 0.0f;
		TargetLocation.Z = 0.0f;

		float Distance = UKismetMathLibrary::Vector_Distance(PlayerLocation, TargetLocation);

		float ControlAimSpeed = 50.0f;
		float AimSpeed = 200.f;
		if (Distance < 50.f)
		{
			return;
		}

		FRotator LerpedRotation = FMath::RInterpTo(GetActorRotation(), NewRotation, DeltaTime, AimSpeed);
		SetActorRotation(NewRotation);

		FRotator LerpedControlRotation = FMath::RInterpTo(GetControlRotation(), NewControlRotation, DeltaTime, ControlAimSpeed);

		AController* Contrl = GetController();
		if (Contrl)
		{
			Contrl->SetControlRotation(NewControlRotation);
		}
	}

	if (FollowCamera && PriorityHitMontage == nullptr)
	{
		FVector CameraLocation = FollowCamera->GetRelativeLocation();
		FVector TargetLocation = FVector::ZeroVector;
		FVector Location = FMath::Lerp(CameraLocation, TargetLocation, DeltaTime * 8.f);
		FollowCamera->SetRelativeLocation(Location);

		FVector CameraRelativeLocation = FollowCamera->GetRelativeLocation();
		//FString CameraLocationString = CameraRelativeLocation.ToString();

		FRotator ControllerRotation = GetControlRotation();
		FRotator CameraRotation = FollowCamera->GetRelativeRotation();
		FRotator TargetRotation = FRotator::ZeroRotator;
		//TargetRotation.Pitch = CameraRotation.Pitch;
		//TargetRotation.Yaw = CameraRotation.Yaw;
		FRotator Rotation = FMath::Lerp(CameraRotation, TargetRotation, DeltaTime * 8.f);
		FollowCamera->SetRelativeRotation(TargetRotation);
	}
}

//////////////////////////////////////////////////////////////////////////
// Input


void AFightingProjectCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{

	InputsComponent->SetupInputs(PlayerInputComponent);
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFightingProjectCharacter::Move);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &AFightingProjectCharacter::MoveCompleted);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Started, this, &AFightingProjectCharacter::LookStart);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFightingProjectCharacter::Look);

		//Interacting
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AFightingProjectCharacter::Interact);

		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Started, this, &AFightingProjectCharacter::RunStart);
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Triggered, this, &AFightingProjectCharacter::Run);
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Completed, this, &AFightingProjectCharacter::StopRun);

		//Attacks
		EnhancedInputComponent->BindAction(Attack_1, ETriggerEvent::Started, this, &AFightingProjectCharacter::Attack1);
		EnhancedInputComponent->BindAction(Attack_1, ETriggerEvent::Triggered, this, &AFightingProjectCharacter::HoldAttackInput);
		EnhancedInputComponent->BindAction(Attack_1, ETriggerEvent::Completed, this, &AFightingProjectCharacter::EndHoldAttack);

		EnhancedInputComponent->BindAction(Attack_2, ETriggerEvent::Started, this, &AFightingProjectCharacter::Attack2);
		EnhancedInputComponent->BindAction(Attack_2, ETriggerEvent::Triggered, this, &AFightingProjectCharacter::HoldAttackInput);
		EnhancedInputComponent->BindAction(Attack_2, ETriggerEvent::Completed, this, &AFightingProjectCharacter::EndHoldAttack);

		EnhancedInputComponent->BindAction(Attack_3, ETriggerEvent::Started, this, &AFightingProjectCharacter::Attack3);
		EnhancedInputComponent->BindAction(Attack_3, ETriggerEvent::Triggered, this, &AFightingProjectCharacter::HoldAttackInput);
		EnhancedInputComponent->BindAction(Attack_3, ETriggerEvent::Completed, this, &AFightingProjectCharacter::EndHoldAttack);

		EnhancedInputComponent->BindAction(DodgeStanceAction, ETriggerEvent::Started, this, &AFightingProjectCharacter::DodgeStanceStart);
		EnhancedInputComponent->BindAction(DodgeStanceAction, ETriggerEvent::Triggered, this, &AFightingProjectCharacter::DodgeStance);
		EnhancedInputComponent->BindAction(DodgeStanceAction, ETriggerEvent::Completed, this, &AFightingProjectCharacter::DodgeStanceInputCompleted);
		EnhancedInputComponent->BindAction(ChangeTargetAction, ETriggerEvent::Started, this, &AFightingProjectCharacter::ChangeTargetStart);

		EnhancedInputComponent->BindAction(SpecialAction, ETriggerEvent::Started, this, &AFightingProjectCharacter::HandleSpecialAttackInput);

		//Pause
		EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Started, this, &AFightingProjectCharacter::PauseEvent);
	}
}

void AFightingProjectCharacter::EndHitReaction()
{
	PriorityHitMontage = nullptr;
	bCanAct = true;
	DamagableComponent->CurrentFrozenState = EFrozenState::None;
}

//MOVEMENT AND INPUT ACTIONS HANDLERS

void AFightingProjectCharacter::HandleMovementInput()
{
	float TargetWalkSpeed = 230.f;
	bool bCanRun = false;

	if (bRunInput && !bIsAttacking)
	{
		bCanRun = true;
	}
	if (bIsAttacking || !bRunInput || MovementVector2D.X < -0.5f || MovementVector2D.X > 0.5f ||  MovementVector2D.Y < 0.8f )
	{
		bCanRun = false;
	}

	bRunning = bCanRun;

	if (bRunning)
	{
		GetCharacterMovement()->MaxAcceleration = 500;
		TargetWalkSpeed = 380.f;
		if (bSprintInputValid)
		{
			
			bSprinting = true;
			GetCharacterMovement()->MaxAcceleration = 1500;
			TargetWalkSpeed = 600.f;
		}
	}
	else
	{
		bSprintInputValid = false;
		bSprinting = false;

		TargetWalkSpeed = 210.f;
		GetCharacterMovement()->MaxAcceleration = 1000;
		if (MovementVector2D.X != 0 && MovementVector2D.Y == 0)
		{
			GetCharacterMovement()->MaxAcceleration = 500;
			TargetWalkSpeed = 160.f;
		}
	}
	if (MovementVector2D.Y < 0)
	{
		TargetWalkSpeed = 120.f;
	}
	

	GetCharacterMovement()->MaxWalkSpeed = TargetWalkSpeed;

}

void AFightingProjectCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();
	MovementVector2D = MovementVector;
	MovementVector2D.Normalize();

	//FString MovementString = FString::Printf(TEXT("Movement Vector: (%f, %f)"), MovementVector2D.X, MovementVector2D.Y);
	//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, MovementString);

	if (PriorityHitMontage != nullptr || bIsAttacking)
	{
		return;
	}

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
		
	}
}

void AFightingProjectCharacter::MoveCompleted(const FInputActionValue& Value)
{
	MovementVector2D.X = 0;
	MovementVector2D.Y = 0;
}

void AFightingProjectCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (bDodgeStance && AttackTarget || PriorityHitMontage != nullptr || bCanDodge == false)
	{
		return;
	}

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AFightingProjectCharacter::LookStart(const FInputActionValue& Value)
{
	if (!bCanAct)
		return;

	FVector2D InputVector = Value.Get<FVector2D>();
	InputVector.Normalize();

	if (CarriedActor)
	{
		if (CarriedActor->ObjectType == EPickableType::Box)
		{
			return;
		}
	}

	if (bDodgeStance && bCanDodge)
	{
		EndAttackCombo();
		//FString MovementString = FString::Printf(TEXT("Movement Vector: (%f, %f)"), MovementVector.X, MovementVector.Y);
		//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, MovementString);

		if (InputVector.X < 0.f && InputVector.Y > -0.75f && InputVector.Y < 0.75f)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, "DODGE LEFT");
			DodgeAction(DodgeMontages[3]);
			StartDodgeTimer(0.3f);
		}
		else if (InputVector.X > 0 && InputVector.Y > -0.75f && InputVector.Y < 0.75f)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, "DODGE RIGHT");
			DodgeAction(DodgeMontages[2]);
			StartDodgeTimer(0.3f);

		}
		else if (InputVector.Y < 0)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, "DODGE FORWARD");
			DodgeAction(DodgeMontages[4]);
			StartDodgeTimer(0.3f);
		}
		else
		{
			//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, "DODGE BACKWARDS");
			DodgeAction(DodgeMontages[1]);
			StartDodgeTimer(0.3f);
		}
	}
}

void AFightingProjectCharacter::Interact(const FInputActionValue& Value)
{
	if (!bCanAct)
		return;

	if (InteractableCombatCharacter && CanPlayAttackMontage() && CarriedActor == nullptr)
	{
		if (IInterface_CombatCharacter::Execute_CanBeGrabbed(InteractableCombatCharacter)) 
		{
			GrabCharacter();
			return;
		}
	}

	AActor* InteractionActor = nullptr;

	if (GatherableItem != nullptr)
	{
		InteractionActor = GatherableItem;
	}

	if (InteractionActor == nullptr)
	{
		if (CarriedActor != nullptr)
		{
			if (IInterface_Carriable::Execute_CanThrow(CarriedActor) == true && CanPlayAttackMontage())
			{
				bCanAct = false;
				ThrowObject();
				return;
			}
			else
			{
				CarriedActor = nullptr;
			}
			return;
		}
	}

	if (InteractionTarget != nullptr && InteractionActor == nullptr)
	{
		InteractionActor = InteractionTarget;
	}

	if (InteractionActor == nullptr)
	{
		return;
	}

	bool bIsCarriable = InteractionActor->GetClass()->ImplementsInterface(UInterface_Carriable::StaticClass());

	if (bIsCarriable == true && CarriedActor == nullptr)
	{
		CarryObject(IInterface_Carriable::Execute_GetCarriable(InteractionActor));
	}

	if (InteractionActor->GetClass()->ImplementsInterface(UInterface_Interactable::StaticClass()))
	{
		IInterface_Interactable::Execute_Interact(InteractionActor);
	}
}

void AFightingProjectCharacter::RunStart(const FInputActionValue& Value)
{
	if (!bCanAct)
		return;
	if (CarriedActor)
	{
		if (CarriedActor->ObjectType == EPickableType::Box)
		{
			return;
		}
	}

	if (MovementVector2D.Y < 0 && bCanTurn180)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Orange, FString::Printf(TEXT("MovementVector2D.Y: %.2f"), MovementVector2D.Y));
		//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Orange, "TURN 180 START");

		FRotator ControllerRotation = GetControlRotation();
		ControllerRotation.Yaw += 180.0f;
		Controller->SetControlRotation(ControllerRotation);
		Start180TurnTimer(0.4f);
	}

	if (bSprintInputFlag == true)
	{
		bSprintInputValid = true;

		if (bCanDodge && CanPlayAttackMontage())
		{
			EndAttackCombo();
			DodgeAction(DodgeMontages[0]);
			StartDodgeTimer(0.4f);
		}
	}
	else
	{
		StartSprintInputTimer();
	}
}

void AFightingProjectCharacter::Run(const FInputActionValue& Value)
{
	bRunInput = true;
}

void AFightingProjectCharacter::StopRun(const FInputActionValue& Value)
{
	bRunInput = false;
}

void AFightingProjectCharacter::InteractTrace()
{
	InteractionTarget = nullptr;
	InteractableCombatCharacter = nullptr;

	const FVector TraceStart = GetActorLocation();
	const FRotator Rotation = GetActorRotation();
	const FVector ForwardVector = UKismetMathLibrary::GetForwardVector(Rotation);

	const float distance = 100;
	const FVector TraceEnd = TraceStart + ForwardVector * distance;
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.TraceTag = FName("Debug");
	Params.bDebugQuery = true;

	AActor* InteractionCandidate = nullptr;

	FVector FeetTraceStart = TraceStart;
	FeetTraceStart.Z -= 80;
	FVector FeetTraceEnd = FeetTraceStart + ForwardVector * distance;

	if (GetWorld()->LineTraceSingleByChannel(HitResult, FeetTraceStart, FeetTraceEnd, ECollisionChannel::ECC_GameTraceChannel1, Params, FCollisionResponseParams()))
	{
		if (HitResult.bBlockingHit)
		{
			AActor* HitActor = HitResult.GetActor();
			bool IsInteractable = HitActor->GetClass()->ImplementsInterface(UInterface_Interactable::StaticClass());

			if (IsInteractable)
			{
				InteractionCandidate = HitActor;
			}
		}
	}
	
	if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_GameTraceChannel1, Params, FCollisionResponseParams()))
	{
		//DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 1.0f, 0, 1.0f);

		if (HitResult.bBlockingHit) 
		{
			//DrawDebugPoint(GetWorld(), HitResult.ImpactPoint, 10.0f, FColor::Green, false, 5.0f);

			AActor* HitActor = HitResult.GetActor();
			IInterface_CombatCharacter* CharacterActor = Cast<IInterface_CombatCharacter>(HitActor);
			bool IsInteractable = HitActor->GetClass()->ImplementsInterface(UInterface_Interactable::StaticClass());

			if (IsInteractable)
			{
				InteractionCandidate = HitActor;
			}
			if (CharacterActor != nullptr)
			{
				InteractableCombatCharacter = HitActor;
			}
		}
	}

	FVector HeadTraceStart = TraceStart;
	HeadTraceStart.Z += 60;
	FVector HeadTraceEnd = HeadTraceStart + ForwardVector * distance;

	if (GetWorld()->LineTraceSingleByChannel(HitResult, HeadTraceStart, HeadTraceEnd, ECollisionChannel::ECC_GameTraceChannel1, Params, FCollisionResponseParams()))
	{
		if (HitResult.bBlockingHit)
		{
			AActor* HitActor = HitResult.GetActor();
			bool IsInteractable = HitActor->GetClass()->ImplementsInterface(UInterface_Interactable::StaticClass());

			if (IsInteractable)
			{
				InteractionCandidate = HitActor;
			}
		}
	}

	if (InteractionCandidate) 
	{
		InteractionTarget = InteractionCandidate;
	}

}


void AFightingProjectCharacter::ThrowObject_Implementation()
{
	bCanAct = true;
	const FRotator Rotation = FollowCamera->GetComponentRotation();
	const FVector ForwardVector = UKismetMathLibrary::GetForwardVector(Rotation);
	FVector TargetVector = ForwardVector;

	if (CarriedActor == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("CarriedActor is null, cannot throw object."));
		return;
	}

	if (AttackTarget != nullptr)
	{
		FVector TargetLocation = AttackTarget->GetActorLocation();
		TargetLocation.Z += 50;
		TargetVector = TargetLocation - CarriedActor->GetActorLocation();
		TargetVector.Normalize();
	}

	bool bHasWeaponData = false;
	if (CarriedObjectMovesetData) 
	{
		bHasWeaponData = true;
		CarriedObjectMovesetData = nullptr;
	}

	CarriedActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	CarriedActor->OnThrow.Broadcast();
	
	CarriedActor->StaticMesh->AddImpulse(TargetVector * 4000 * 100);
	//DrawDebugLine(GetWorld(), CarriedActor->GetActorLocation(), CarriedActor->GetActorLocation() + TargetVector * 1000.0f, FColor::Green, false, 5.0f, 0, 5.0f);


	CarriedActor = nullptr;
	ChangeCameraBoom(CameraBoom);

	if (bHasWeaponData)
	{
		OnWeaponDrop.Broadcast();
	}
}

void AFightingProjectCharacter::CarryObject_Implementation(APickableActor* Actor)
{
	if (Actor == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, ("ACTOR TO CARRY NULL"));
		return;
	}

	if (CarriedActor == nullptr) 
	{
		CarriedActor = Actor;

		FName CarrySocket = TEXT("Carry");

		if (AWeapon* WeaponClass = Cast<AWeapon>(CarriedActor))
		{
			CarriedObjectMovesetData = WeaponClass->AttackDataTable;
			TargetCarryAnimation = PickUpWeaponAnimation;
			OnWeaponPickUp.Broadcast(WeaponClass->WeaponName, WeaponClass->Durability);
		}
		else
		{
			TargetCarryAnimation = PickUpBoxAnimation; 
			ChangeCameraBoom(HoldingObjectCameraBoom);
		}

		CarrySocket = Actor->CarrySocketName;

		CarriedActor->ThrowrActor = this;
		CarriedActor->OnPickUp.Broadcast();

		CarriedActor->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, CarrySocket);
		
	}
}

//ATTACK AND DODGE ACTIONS

void AFightingProjectCharacter::Attack1(const FInputActionInstance& Instance)
{
	if (!bCanAct)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Black, ("CANNOT ACT"));
		return;
	}
	
	if (CarriedActor != nullptr) 
	{
		if (CarriedActor->Durability <= 0)
		{
			return;
		}
		if (bIsAttacking) 
		{
			bSaveAttack = bCanSaveAttack;
			return;
		}
		else if (IInterface_Carriable::Execute_CanThrow(CarriedActor))
		{
			if (CanPlayAttackMontage() == true) 
			{
				if (CarriedObjectMovesetData)
				{

					TMap<UAnimMontage*, FAttackSettings> AttackMap;
					F_AttackData* AttackData = CarriedObjectMovesetData->FindRow<F_AttackData>("Attack1", TEXT(""));
					if (AttackData)
					{
						AttackMap = AttackData->AttackMapAction;
					}
						
					bIsAttacking = true;

					ComboNumber = 1;
					GetAttack(AttackMap);

					return;
				}
			}
			return;
		}
		else
		{
			CarriedActor = nullptr;
		}
	}
	
	if (MovementVector2D.Y < -0.5f )
	{
		if (!AttacksComponent->AttackMovesetData.AttackSlot_1_D.IsEmpty())
		{
			SingleAttack(AttacksComponent->AttackMovesetData.AttackSlot_1_D, Instance);
		}

		return;
	}

	if (bSprinting)
	{
		SingleAttack(SprintAttack_1, Instance);

		return;
	}
	
	if (bIsAttacking) 
	{
		bSaveAttack = bCanSaveAttack;
	}
	else 
	{
		bIsAttacking = true;

		ComboNumber = 1;

		if (!AttacksComponent->AttackMovesetData.AttackSlot_1.IsEmpty())
		{
			GetAttack(AttacksComponent->AttackMovesetData.AttackSlot_1);
		}
	}
}

void AFightingProjectCharacter::SingleAttack(TMap<UAnimMontage*, FAttackSettings> AttackMap, const FInputActionInstance& Instance)
{
	if (CanPlayAttackMontage() == false)
		return;

	if (bIsAttacking)
	{
		bSaveAttack = bCanSaveAttack;

		if (bSaveAttack)
		{
			const UInputAction* HeldAction = Instance.GetSourceAction();
			CurrentAttackInput = HeldAction;

			for (const TPair<UAnimMontage*, FAttackSettings>& Kvp : AttackMap)
			{
				SavedAttack.Empty();
				SavedAttack.Add(Kvp.Key, Kvp.Value);
			}
		}
	}
	else
	{
		bIsAttacking = true;
		UAnimMontage* TargetMontage = nullptr;
		for (const TPair<UAnimMontage*, FAttackSettings>& Kvp : AttackMap)
		{
			CurrentAttack.Empty();
			CurrentAttack.Add(Kvp.Key, Kvp.Value);
			TargetMontage = Kvp.Key;
		}
		if (TargetMontage != nullptr)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, "ATTACK 1 BACKWARDS");
			const UInputAction* HeldAction = Instance.GetSourceAction();
			CurrentAttackInput = HeldAction;
			

			PlayAttackMontageOnce(TargetMontage, 0);
		}
	}
}

void AFightingProjectCharacter::Attack2(const FInputActionInstance& Instance)
{
	//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, "Attack 2 Input");
	if (!bCanAct)
		return;
	
	if (CarriedActor)
	{
		if (CarriedActor->Durability <= 0)
		{
			return;
		}
		if (IInterface_Carriable::Execute_CanThrow(CarriedActor))
		{
			if (CanPlayAttackMontage() == true)
			{
				if (AWeapon* WeaponClass = Cast<AWeapon>(CarriedActor))
				{

					UDataTable* Data = WeaponClass->AttackDataTable;
					if (Data)
					{
						TMap<UAnimMontage*, FAttackSettings> AttackMap;
						F_AttackData* AttackData = Data->FindRow<F_AttackData>("Attack3", TEXT(""));
						if (AttackData)
						{
							AttackMap = AttackData->AttackMapAction;
						}

						SingleAttack(AttackMap, Instance);
						return;
					}
				}
			}
			return;
		}
		else
		{
			CarriedActor = nullptr;
		}
	}

	if (MovementVector2D.Y < -0.5f)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, "BACK ATTACK");
		if (!AttacksComponent->AttackMovesetData.AttackSlot_2_D.IsEmpty())
		{
			SingleAttack(AttacksComponent->AttackMovesetData.AttackSlot_2_D, Instance);
		}

		return;
	}

	if (bSprinting)
	{
		SingleAttack(SprintAttack_2, Instance);

		return;
	}

	if (MovementVector2D.Y > 0.5f)
	{
		if (bLowAttackInputFlag)
		{
			SingleAttack(AttacksComponent->LowAttack, Instance);
			return;
		}
		if (TryFrozenAttack(Instance) == true)
		{
			return;
		}
	}

	if (!AttacksComponent->AttackMovesetData.AttackSlot_2.IsEmpty())
	{
		SingleAttack(AttacksComponent->AttackMovesetData.AttackSlot_2, Instance);
	}
}

void AFightingProjectCharacter::Attack3(const FInputActionInstance& Instance)
{
	//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, "Attack 3 Input");
	if (!bCanAct)
		return;

	if (CarriedActor)
	{
		if (CarriedActor->Durability <= 0)
		{
			return;
		}
		if (IInterface_Carriable::Execute_CanThrow(CarriedActor))
		{
			if (CanPlayAttackMontage() == true)
			{
				if (AWeapon* WeaponClass = Cast<AWeapon>(CarriedActor))
				{
					UDataTable* Data = WeaponClass->AttackDataTable;
					if (Data)
					{
						TMap<UAnimMontage*, FAttackSettings> AttackMap;
						F_AttackData* AttackData = Data->FindRow<F_AttackData>("Attack2", TEXT(""));
						if (AttackData)
						{
							AttackMap = AttackData->AttackMapAction;
						}

						SingleAttack(AttackMap, Instance);
						return;
					}
				}
			}
			return;
		}
		else
		{
			CarriedActor = nullptr;
		}
	}

	if (MovementVector2D.Y < -0.5f)
	{
		if (!AttacksComponent->AttackMovesetData.AttackSlot_3_D.IsEmpty())
		{
			SingleAttack(AttacksComponent->AttackMovesetData.AttackSlot_3_D, Instance);
		}

		return;
	}

	if (bSprinting)
	{
		SingleAttack(SprintAttack_2, Instance);

		return;
	}

	if (!AttacksComponent->AttackMovesetData.AttackSlot_3.IsEmpty())
	{
		SingleAttack(AttacksComponent->AttackMovesetData.AttackSlot_3, Instance);
	}
}

void AFightingProjectCharacter::HoldAttackInput(const FInputActionInstance& Instance)
{
	const UInputAction* HeldAction = Instance.GetSourceAction();
	if (HeldAction == CurrentAttackInput)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, ("HOLDING ATTACK INPUT"));

		HoldingAttackInput = true;
	}
}

void AFightingProjectCharacter::EndHoldAttack()
{
	HoldingAttackInput = false;
}

// SPECIAL ATTACK ACTION
void AFightingProjectCharacter::HandleSpecialAttackInput()
{
	if (CanPlayAttackMontage() == false || bIsAttacking)
	{
		return;
	}
	if (CarriedActor)
	{
		return;
	}
	if (SpecialMovesComponent->HasAvailableMoves() == false) 
	{
		GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, ("Not enough orbs for any action"));
		return;
	}

	if (CombatState == ECombatState::None)
	{
		EnterSpecialAttackSelection();
	}
}

void AFightingProjectCharacter::ExitSpecialAttackSelection()
{
	if (CurrentSpecialMoveMontage !=nullptr)
		return;

	CombatState = ECombatState::None;
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);	
	
	ChangeCameraBoom(CameraBoom);
	GetCharacterMovement()->bOrientRotationToMovement = false;

	DamagableComponent->bIsInvincible = false;

	this->CustomTimeDilation = 1.0f;
	InputsComponent->SwitchInputMapping(EInputMode::None);
	SpecialMovesComponent->CloseMenu();
	GetMesh()->GetAnimInstance()->Montage_Stop(0.2f, SpecialMovesComponent->SpecialMovePose);

	EndAttackCombo();

	if (bDodgeStance)
	{
		DodgeStanceExit(nullptr);
	}
}

void AFightingProjectCharacter::SpecialAttackAction_Implementation(int32 ActionIndex)
{
	SpecialMovesComponent->bExecutingFinisher = false;
	
	ExitSpecialAttackSelection();
}

void AFightingProjectCharacter::HandleSpecialAttackRequest(int32 Index)
{
	if (SpecialMovesComponent->bExecutingFinisher)
		return;
	TArray<UAnimMontage*> Keys;
	SpecialMovesComponent->SpecialMoves.GenerateKeyArray(Keys);

	if (Index >= 0 && Index < Keys.Num())
	{
		FSpecialAttackSettings Value = SpecialMovesComponent->SpecialMoves[Keys[Index]];
		if (Value.OrbsCost <= SpecialMovesComponent->OrbsNumber)
		{
			SpecialMovesComponent->OrbsNumber -= Value.OrbsCost;
			OnSpecialMoveExecute.Broadcast();
			StatsHud->UpdateOrbHUD();
			SpecialMovesComponent->bExecutingFinisher = true;
			SpecialAttackAction(Index);
			DamagableComponent->bIsInvincible = true;
		}
		else 
		{
			GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, ("Not enough orbs for this action"));
		}
	}
}


void AFightingProjectCharacter::EnterSpecialAttackSelection()
{
	CombatState = ECombatState::SpecialMove;

	ChangeCameraBoom(SpecialActionsCameraBoom);

	GetCharacterMovement()->bOrientRotationToMovement = true;

	InputsComponent->SwitchInputMapping(EInputMode::SpecialMoveMenu);
	SpecialMovesComponent->OpenMenu();
	GetMesh()->GetAnimInstance()->Montage_Play(SpecialMovesComponent->SpecialMovePose, 1.0f, EMontagePlayReturnType::MontageLength, 0.0f, true);

	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.1f);
	this->CustomTimeDilation = 1.0f / UGameplayStatics::GetGlobalTimeDilation(GetWorld());

	if (bDodgeStance == false)
	{
		DodgeStanceStart();
	}
}

//----END SECTION

void AFightingProjectCharacter::DodgeStanceStart()
{
	bDodgeStance = true;

	TArray<AActor*> Targets = DodgeStanceTrace();
	AttackTarget = GetClosestTarget(Targets);

	if (AttackTarget != nullptr)
	{
		bool b = AttackTarget->GetClass()->ImplementsInterface(UInterface_CombatCharacter::StaticClass());
		if (b)
		{
			UComponent_Damagable* Damagable = IInterface_CombatCharacter::Execute_GetDamagableComponent(AttackTarget);
			IInterface_Enemy::Execute_GetOwnerClass(AttackTarget)->bIsAimed = true;
		}
	}
}

AActor* AFightingProjectCharacter::GetClosestTarget(TArray<AActor*> Targets)
{
	if (Targets.Num() == 0)
	{
		return nullptr;
	}

	SortTargets(Targets);
	return Targets[0];
}

void AFightingProjectCharacter::SortTargets(TArray<AActor*>& Targets)
{
	FVector PlayerPosition = GetActorLocation();
	FVector PlayerForwardVector = GetActorForwardVector();

	Targets.Sort([PlayerPosition, PlayerForwardVector](const AActor& A, const AActor& B)
		{
			FVector DirectionToA = (A.GetActorLocation() - PlayerPosition).GetSafeNormal();
			FVector DirectionToB = (B.GetActorLocation() - PlayerPosition).GetSafeNormal();

			float AngleA = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(PlayerForwardVector.GetSafeNormal(), DirectionToA.GetSafeNormal())));
			
			float AngleB = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(PlayerForwardVector.GetSafeNormal(), DirectionToB.GetSafeNormal())));

			if (AngleA == AngleB)
			{
				float DistanceA = FVector::Dist(PlayerPosition, A.GetActorLocation());
				float DistanceB = FVector::Dist(PlayerPosition, B.GetActorLocation());
				return DistanceA < DistanceB;
			}

			return AngleA < AngleB;
		});
}

void AFightingProjectCharacter::DodgeStance()
{
	if (AttackTarget != nullptr)
	{
		if (AttackTarget->GetClass()->ImplementsInterface(UInterface_CombatCharacter::StaticClass()))
		{
			if (IInterface_CombatCharacter::Execute_GetDamagableComponent(AttackTarget)->bIsDead == true)
			{
				UComponent_Damagable* Damagable = IInterface_CombatCharacter::Execute_GetDamagableComponent(AttackTarget);
				AttackTarget = nullptr;

				ChangeTargetStart();
			}
		}
	}
}

TArray<AActor*> AFightingProjectCharacter::DodgeStanceTrace()
{
	TArray<AActor*> OverlappingTargets;

	// Get the player position
	FVector PlayerPosition = GetActorLocation();

	// Sphere trace settings
	float SphereRadius = 1000.0f; // Adjust the radius as needed
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this); // Ignore self

	// Sphere trace to find all overlapping actors
	TArray<FHitResult> OutHits;
	bool bHasHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
		GetWorld(),
		PlayerPosition,
		PlayerPosition,
		SphereRadius,
		ObjectTypes,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		OutHits,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		2.f
	);

	if (bHasHit)
	{
		for (const FHitResult& Hit : OutHits)
		{
			AActor* HitActor = Hit.GetActor();

			if (HitActor && HitActor->GetClass()->ImplementsInterface(UInterface_CombatCharacter::StaticClass()))
			{
				// Perform a line trace to check for visibility
				FHitResult LineTraceHit;
				FVector TargetPosition = HitActor->GetActorLocation();
				bool bIsVisible = GetWorld()->LineTraceSingleByChannel(
					LineTraceHit,
					PlayerPosition,
					TargetPosition,
					ECollisionChannel::ECC_GameTraceChannel2,
					FCollisionQueryParams(FName(TEXT("VisibilityTrace")), true, this)
				);

				// Check if the line trace hit the target actor
				if (bIsVisible && LineTraceHit.GetActor() == HitActor)
				{
					OverlappingTargets.AddUnique(HitActor);
				}
			}
		}
	}

	// Debugging
	for (AActor* Actor : OverlappingTargets)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("Visible Combat Character: %s"), *Actor->GetName()));
	}

	return OverlappingTargets;
}

void AFightingProjectCharacter::DodgeStanceExit(AActor* Actor)
{
	if (AttackTarget != nullptr)
	{
		bool b = AttackTarget->GetClass()->ImplementsInterface(UInterface_CombatCharacter::StaticClass());
		if (b)
		{
			UComponent_Damagable* Damagable = IInterface_CombatCharacter::Execute_GetDamagableComponent(AttackTarget);
			//Damagable->OnDeath.RemoveDynamic(this, &AFightingProjectCharacter::DodgeStanceExit);
			if (Actor != nullptr)
			{
				if (AttackTarget == Actor)
				{
					bDodgeStance = false;
					AttackTarget = nullptr;
				}
				else
				{
					return;
				}
			}
		}
	}
	bDodgeStance = false;
	AttackTarget = nullptr;
}

void AFightingProjectCharacter::DodgeStanceInputCompleted()
{
	DodgeStanceExit(AttackTarget);
}

void AFightingProjectCharacter::ChangeTargetStart()
{
	TArray<AActor*> Targets = DodgeStanceTrace();

	if (Targets.Num() == 0)
	{
		return;
	}

	// Populate the AttackTargets array
	AttackTargets = Targets;

	// Sort targets by calculated score
	AttackTargets.Sort([this](AActor& A, AActor& B)
		{
			return CalculateTargetScore(&A) < CalculateTargetScore(&B);
		});

	// Find the index of the current target
	int32 CurrentTargetIndex = AttackTargets.IndexOfByKey(AttackTarget);

	// If current target is not found or there is no current target, start from the beginning
	if (CurrentTargetIndex == INDEX_NONE)
	{
		CurrentTargetIndex = -1;
	}

	// Get the next target in the sorted list
	CurrentTargetIndex = (CurrentTargetIndex + 1) % AttackTargets.Num();
	AttackTarget = AttackTargets[CurrentTargetIndex];
	if (AttackTarget)
	{
		IInterface_Enemy::Execute_GetOwnerClass(AttackTarget)->bIsAimed = true;
	}
	//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, FString::Printf(TEXT("New target: %s"), *AttackTarget->GetName()));
}

float AFightingProjectCharacter::CalculateTargetScore(AActor* Target)
{
	if (!Target)
	{
		return FLT_MAX;
	}

	FVector PlayerPosition = GetActorLocation();
	FVector PlayerForwardVector = GetActorForwardVector();
	FVector TargetLocation = Target->GetActorLocation();
	FVector DirectionToTarget = (TargetLocation - PlayerPosition).GetSafeNormal();

	float Distance = FVector::Dist(PlayerPosition, TargetLocation);
	float AngleDifference = FVector::DotProduct(PlayerForwardVector, DirectionToTarget);

	// Normalize values
	float NormalizedDistance = FMath::Clamp(Distance / 1000.0f, 0.0f, 1.0f);
	float NormalizedAngle = (1.0f + AngleDifference) / 2.0f; // Normalize to [0, 1]

	// Calculate score (higher score is worse)
	float DistanceWeight = 0.5f;
	float AngleWeight = 0.5f;
	float Score = DistanceWeight * NormalizedDistance + AngleWeight * (1.0f - NormalizedAngle);

	return Score;
}

void AFightingProjectCharacter::DodgeAction_Implementation(UAnimMontage* DodgeMontage)
{
}

void AFightingProjectCharacter::DisableYawControl_Implementation()
{
}

void AFightingProjectCharacter::EnableYawControl_Implementation()
{
}

void AFightingProjectCharacter::HandleAttackCombo(float DeltaTime)
{
	if (CanPlayAttackMontage() == false)
		return;

	if (bSaveAttack) 
	{
		bSaveAttack = false;

		if (CarriedActor != nullptr)
		{
			if (CarriedActor->Durability > 0)
			{
				if (IInterface_Carriable::Execute_CanThrow(CarriedActor))
				{
					if (CarriedObjectMovesetData)
					{
						TMap<UAnimMontage*, FAttackSettings> AttackMap;
						F_AttackData* AttackData = CarriedObjectMovesetData->FindRow<F_AttackData>("Attack1", TEXT(""));
						if (AttackData)
						{
							AttackMap = AttackData->AttackMapAction;
						}

						bIsAttacking = true;

						GetAttack(AttackMap);
						//CarriedObjectAttack();
						return;
					}
				}
				else
				{
					CarriedActor = nullptr;
				}
			}
			else
			{
				EndAttackCombo();
			}
		}
		GetAttack(AttacksComponent->AttackMovesetData.AttackSlot_1);
	}
}



void AFightingProjectCharacter::EndAttackCombo()
{
	//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, "END COMBO");
	
	AttackCount = 0;
	bIsAttacking = false;
}

void AFightingProjectCharacter::GetAttack(TMap<UAnimMontage*, FAttackSettings> AttackMap)
{
	if (SavedAttack.Num() > 0) 
	{
		TMap<UAnimMontage*, FAttackSettings>::TIterator It = SavedAttack.CreateIterator();
		UAnimMontage* Key = It.Key();
		FAttackSettings Value = It.Value();

		CurrentAttack.Empty();
		CurrentAttack.Add(Key, Value);

		PlayAttackMontageOnce(Key, 0);
		SavedAttack.Empty();
		return;
	}


	int32 AttackCounter = AttackCount+1;
	PlayAttackMontageOnce(FindAttackMontage(AttackMap, 1, AttackCount), AttackCounter);

}

UAnimMontage* AFightingProjectCharacter::FindAttackMontage(TMap<UAnimMontage*, FAttackSettings> AttackMap, int32 Combo, int32 Num)
{
	// Ensure the index is valid
	if (Num < 0 || Num >= AttackMap.Num())
	{
		return nullptr; // Invalid index
	}

	// Iterate through the map to find the element at the given index
	int32 CurrentIndex = 0;
	for (auto& Elem : AttackMap)
	{
		if (CurrentIndex == Num)
		{
			OnStartAttack(Elem.Value.Distance, AttackTarget);
			return Elem.Key; // Return the value (FAttackSettings) at the index
		}
		CurrentIndex++;
	}

	return nullptr; // In case the index is out of range

}

void AFightingProjectCharacter::PlayAttackMontageOnce_Implementation(UAnimMontage* TargetMontage, int32 AttackCounter)
{
	if (TargetMontage == nullptr) 
	{
		//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, FString::Printf(TEXT("%d"), AttackCounter));
		return;
	}

	FTimerHandle NewTimer = FTimerHandle();

	//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, FString::Printf(TEXT("%d"), AttackCounter));
	bCanSaveAttack = false;
	AttackCount = AttackCounter;
}

void AFightingProjectCharacter::OnDeathEvent(AActor* Actor)
{
	InputsComponent->SwitchInputMapping(EInputMode::PauseMenu);
	DodgeStanceExit(nullptr);
}


void AFightingProjectCharacter::ShutDownAfterDeathMontage()
{
	GetController()->UnPossess();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Set the mesh component to use the "WorldDynamic" collision channel
	GetMesh()->SetCollisionObjectType(ECC_WorldDynamic);

	// Set the collision response to ignore all channels
	GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);

	// Set the collision response to block only world static objects
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);

	//GetMesh()->SetAnimInstanceClass(nullptr);

	if (UCharacterMovementComponent* MovementComponent = GetCharacterMovement())
	{
		MovementComponent->DisableMovement();
		MovementComponent->StopMovementImmediately();
	}
}

void AFightingProjectCharacter::SetupStimulusSource()
{
	StimulusSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("Stimulus"));
	if (StimulusSource)
	{
		StimulusSource->RegisterForSense(TSubclassOf<UAISense_Sight>());
		StimulusSource->RegisterWithPerceptionSystem();
	}
}

//TIMERS

void AFightingProjectCharacter::StartSprintInputTimer()
{
	bSprintInputFlag = true;
	FTimerHandle NewHandle;
	TimerHandles.Add(NewHandle);
	GetWorld()->GetTimerManager().SetTimer(NewHandle, [this, NewHandle]()
		{
			SprintTimer(NewHandle);
		}, 0.5f, false);
}

void AFightingProjectCharacter::StartDodgeTimer(float time)
{
	bCanDodge = false;
	FTimerHandle NewHandle;
	TimerHandles.Add(NewHandle);
	GetWorld()->GetTimerManager().SetTimer(NewHandle, [this, NewHandle]()
		{
			DodgeEnableTimer(NewHandle);
		}, time, false);
}

void AFightingProjectCharacter::Start180TurnTimer(float time)
{
	bCanTurn180 = false;
	FTimerHandle NewHandle;
	TimerHandles.Add(NewHandle);
	GetWorld()->GetTimerManager().SetTimer(NewHandle, [this, NewHandle]()
		{
			Turn180EnableTimer(NewHandle);
		}, time, false);
}

void AFightingProjectCharacter::SprintTimer(FTimerHandle TimerHandle)
{
	bSprintInputFlag = false;
	TimerHandles.Remove(TimerHandle);
	TimerHandle.Invalidate();
}

void AFightingProjectCharacter::DodgeEnableTimer(FTimerHandle TimerHandle)
{
	bCanDodge = true;
	TimerHandles.Remove(TimerHandle);
	TimerHandle.Invalidate();
}

void AFightingProjectCharacter::Turn180EnableTimer(FTimerHandle TimerHandle)
{
	bCanTurn180 = true;
	TimerHandles.Remove(TimerHandle);
	TimerHandle.Invalidate();
}

void AFightingProjectCharacter::TakeDamage_Implementation(FDamageSettings DamageSettings, AActor* InstigatorActor)
{
	if (DamagableComponent->Health <= 0)
		return;

	ReceivedDamageSettings = DamageSettings;

	bool bWasDamaged = DamagableComponent->TakeDamage(DamageSettings, InstigatorActor);

	if (DamagableComponent->bIsInvincible)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, ("IS INVINCIBLE"));
		return;
	}

	if (bWasDamaged)
	{
		if (GrabbedCharacter)
		{
			IInterface_CombatCharacter::Execute_ExitGrabPose(GrabbedCharacter);
			ExitGrab();
		}
		if(CombatState == ECombatState::SpecialMove)
		{
			ExitSpecialAttackSelection();
		}
		
		DamagableComponent->ProcessDamageForHitReaction(ReceivedDamageSettings, InstigatorActor);
		
		if (CarriedActor != nullptr)
		{
			if (CarriedActor->ObjectType == EPickableType::Box)
			{
				CarriedActor->Break();
				ChangeCameraBoom(CameraBoom);
				CarriedActor = nullptr;
			}
		}
		
	}
}

void AFightingProjectCharacter::PlayHitReaction_Implementation(UAnimMontage* TargetMontage, AActor* DamageInstigator)
{
	Super::PlayHitReaction_Implementation(TargetMontage, DamageInstigator);

	PriorityHitMontage = TargetMontage;
	bCanAct = false;
}

AActor* AFightingProjectCharacter::GetAttackTarget_Implementation()
{
	return AttackTarget;
}

void AFightingProjectCharacter::SpawnWeaponSpawnableActor_Implementation()
{
	if (CarriedActor)
	{
		CarriedActor->OnObjectSpawn.Broadcast(this);
		CarriedActor->Durability--;
		OnWeaponUse.Broadcast(CarriedActor->Durability);
	}
}

void AFightingProjectCharacter::ExecuteFinisher_Implementation(FVector WarpingLocation, FRotator WarpingRotation, UAnimMontage* Montage)
{
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, "EXECUTE FINISHER");
	
	UCameraComponent* VictimCamera = IInterface_Finishers::Execute_GetFinisherCamera(FinishersComponent->Victim);

	APlayerController* PlayerControl = Cast<APlayerController>(GetController());

	PlayerControl->SetViewTargetWithBlend(FinishersComponent->Victim, 0.3f, EViewTargetBlendFunction::VTBlend_Linear, 0.0f, false);

	EndAttackCombo();

	DamagableComponent->bIsInvincible = true;

	InputsComponent->SwitchInputMapping(EInputMode::Finisher);

	AController* Contrl = GetController();
	if (Contrl)
	{
		//Contrl->SetControlRotation(WarpingRotation);
	}

	MotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation("Victim", WarpingLocation, WarpingRotation);

	PriorityFinisherMontage = Montage;

	GetMesh()->GetAnimInstance()->StopAllMontages(0.0f);

	/*UAnimMontage* PlayingMontage = GetMesh()->GetAnimInstance()->GetCurrentActiveMontage();
	
	if (PlayingMontage != nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, PlayingMontage->GetName());
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "CURRENT MONTAGE NULL");
	}*/

	GetMesh()->GetAnimInstance()->Montage_Play(Montage, 1.0f, EMontagePlayReturnType::MontageLength, 0.0f, true);
	GetMesh()->GetAnimInstance()->OnMontageBlendingOut.AddDynamic(this, &AFightingProjectCharacter::OnFinisherAttackEnd);
	FinishersComponent->FinishableActors.Empty();
}


void AFightingProjectCharacter::AttackAnticipationTrace_Implementation()
{
	FVector Start = GetActorLocation();
	FVector End = GetActorForwardVector() * 250 + Start;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;

	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	if (CarriedActor)
		ActorsToIgnore.Add(CarriedActor);

	FHitResult HitResult = StaticFunctions::SphereCollisionTraceObject(GetWorld(), Start, End, ObjectTypes, ActorsToIgnore, 100);

	if (HitResult.bBlockingHit)
	{
		if (HitResult.GetActor() != nullptr)
		{
			bool bHasInterface = HitResult.GetActor()->GetClass()->ImplementsInterface(UInterface_Enemy::StaticClass());
			if (bHasInterface == true)
			{
				TArray<AActor*> ActorsToRemoveFromSaved = TArray<AActor*>();
				if (!AttackRangeActors.Contains(HitResult.GetActor()))
				{
					AttackRangeActorsSaved.AddUnique(HitResult.GetActor());
					AttackRangeActors.Add(HitResult.GetActor());
					IInterface_Enemy::Execute_SetIsTargeted(HitResult.GetActor(), true);
				}
				for (AActor* Actor : AttackRangeActorsSaved)
				{
					if (Actor != HitResult.GetActor() && Actor != nullptr)
					{
						IInterface_Enemy::Execute_SetIsTargeted(Actor, false);
						ActorsToRemoveFromSaved.Add(Actor);
					}
				}
				for (AActor* Actor : ActorsToRemoveFromSaved)
				{
					if (AttackRangeActorsSaved.Contains(Actor))
					{
						AttackRangeActorsSaved.Remove(Actor);
					}
				}
				ActorsToRemoveFromSaved.Empty();
			}
			AttackRangeActors.Empty();
		}
	}
	else
	{
		for (AActor* Actor : AttackRangeActorsSaved)
		{
			if (Actor != nullptr)
			{
				IInterface_Enemy::Execute_SetIsTargeted(Actor, false);
			}
		}
		AttackRangeActorsSaved.Empty();
	}
}

AFightingProjectCharacter* AFightingProjectCharacter::GetPlayerCharacter_Implementation()
{
	return this;
}

void AFightingProjectCharacter::SetGatherableItem_Implementation(AActor* Item)
{
	GatherableItem = Item;
}

AActor* AFightingProjectCharacter::GetGatherableItem_Implementation()
{
	return GatherableItem;
}

void AFightingProjectCharacter::AddOrbs_Implementation(int32 OrbsToAdd)
{
	//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Orange, ("ADD ORBS"));

	SpecialMovesComponent->OrbsNumber += OrbsToAdd;
	if (SpecialMovesComponent->OrbsNumber > SpecialMovesComponent->MaxOrbsNumber)
	{
		SpecialMovesComponent->OrbsNumber = SpecialMovesComponent->MaxOrbsNumber;
	}
	StatsHud->UpdateOrbHUD();
}

void AFightingProjectCharacter::AddHealth_Implementation(float HealthToAdd)
{
	//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Orange, ("ADD HEALTH"));

	DamagableComponent->Health += HealthToAdd;
	if (DamagableComponent->Health > DamagableComponent->MaxHealth)
	{
		DamagableComponent->Health = DamagableComponent->MaxHealth;
	}
}

void AFightingProjectCharacter::AddMaxHealth_Implementation(float HealthToAdd)
{
	DamagableComponent->MaxHealth += HealthToAdd;
}

void AFightingProjectCharacter::AddMaxOrbs_Implementation(int32 OrbsToAdd)
{
	SpecialMovesComponent->MaxOrbsNumber += OrbsToAdd;
	StatsHud->UpdateOrbHUD();
}

void AFightingProjectCharacter::ConsumeWeaponUse()
{
	if (!Cast<APistol>(CarriedActor))
	{
		CarriedActor->Durability--;
		OnWeaponUse.Broadcast(CarriedActor->Durability);
		if (CarriedActor->Durability <= 0)
		{
			OnWeaponDrop.Broadcast();
			ChangeCameraBoom(CameraBoom);
			CarriedActor->Break();
			CarriedActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			CarriedActor = nullptr;
		}
	}
}

void AFightingProjectCharacter::OnFinisherAttackEnd(UAnimMontage* Montage, bool bInterrupted)
{
	//FollowCamera->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	//FollowCamera->AttachToComponent(CameraBoom, FAttachmentTransformRules::SnapToTargetIncludingScale);
	
	PriorityFinisherMontage = nullptr;

	APlayerController* PlayerControl = Cast<APlayerController>(GetController());
	if (PlayerControl)
	{
		PlayerControl->SetViewTargetWithBlend(this, 0.3f, EViewTargetBlendFunction::VTBlend_Linear, 0.0f, false);
	}

	GetMesh()->GetAnimInstance()->OnMontageBlendingOut.RemoveAll(this);
	InputsComponent->SwitchInputMapping(EInputMode::None);

	bIsAttacking = false;
	DamagableComponent->bIsInvincible = false;
	CameraBoom->bUsePawnControlRotation = true;

	DodgeStanceExit(nullptr);
}

bool AFightingProjectCharacter::CanPlayAttackMontage()
{
	if (PriorityHitMontage == nullptr && PriorityFinisherMontage == nullptr)
	{
		return true;
	}

	return false;
}

void AFightingProjectCharacter::ChangeCameraBoom(USpringArmComponent* CameraSpringArm)
{
	if (CameraSpringArm && FollowCamera)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Change camera boom");
		FollowCamera->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		FollowCamera->AttachToComponent(CameraSpringArm, FAttachmentTransformRules::KeepWorldTransform);
	}
}

void AFightingProjectCharacter::GrabCharacter()
{
	GrabbedCharacter = InteractableCombatCharacter;

	InputsComponent->SwitchInputMapping(EInputMode::Grab);
	FVector WarpingLocation = StaticFunctions::GetLocationAtDistanceXFromActorB(this, GrabbedCharacter, 100.f);
	FRotator WarpingRotation = StaticFunctions::GetRotationForFacingDirection(this, GrabbedCharacter);

	GetController()->SetControlRotation(WarpingRotation);

	DodgeStanceExit(AttackTarget);
	ChangeCameraBoom(GrabCameraBoom);

	bUseControllerRotationYaw = false;
	CameraBoom->bUsePawnControlRotation = false;

	auto FirstElement = GrabPosePair.CreateConstIterator();

	auto GrabberAnimation = FirstElement->Key;
	auto GrabbedAnimation = FirstElement->Value;

	//MotionWarpingComponent->AddOrUpdateWarpTargetFromLocation("Victim", WarpingLocation);
	MotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation("Victim", WarpingLocation, WarpingRotation);

	IInterface_CombatCharacter::Execute_ReceiveGrabAction(GrabbedCharacter, GrabbedAnimation, this);
	GetMesh()->GetAnimInstance()->Montage_Play(GrabberAnimation, 1.f, EMontagePlayReturnType::MontageLength, 0.0f, true);
	//GetMesh()->GetAnimInstance()->OnMontageBlendingOut.AddDynamic(this, &AFightingProjectCharacter::ExitGrab);
}

void AFightingProjectCharacter::ExitGrab()
{
	//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, "GRAB POSE ENDED");
	
	if (GrabbedCharacter)
	{
		//FRotator TargetRotation = StaticFunctions::GetRotationForFacingDirection(this, GrabbedCharacter);
		//GetController()->SetControlRotation(TargetRotation);
	}

	GetCharacterMovement()->bOrientRotationToMovement = false;
	bUseControllerRotationYaw = true;

	int32 CurrentIndex = 0;
	for (const TPair<UAnimMontage*, UAnimMontage*>& Pair : GrabPosePair)
	{
		if (Pair.Key == CurrentGrabAttack)
		{
			if (CurrentIndex == 1)
			{
				FRotator ControllerRotation = GetControlRotation();
				//ControllerRotation.Yaw += 180.0f;
				//Controller->SetControlRotation(ControllerRotation);
			}
			break;
		}
		CurrentIndex++;
	}
	ChangeCameraBoom(CameraBoom);
	CameraBoom->bUsePawnControlRotation = true;

	DamagableComponent->bIsInvincible = false;
	EndAttackCombo();
	InputsComponent->SwitchInputMapping(EInputMode::None);

	CurrentGrabAttack = nullptr;
	GrabbedCharacter = nullptr;

	GetMesh()->GetAnimInstance()->StopAllMontages(0.25f);

	APlayerController* PlayerControl = Cast<APlayerController>(GetController());

	PlayerControl->SetViewTargetWithBlend(this, 0.5f, EViewTargetBlendFunction::VTBlend_Linear, 0.0f, false);
}

void AFightingProjectCharacter::PauseEvent()
{
	OnGamePause.Broadcast();
}


void AFightingProjectCharacter::PerformGrabAttack_Implementation(int32 ActionIndex)
{
	//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, "GRAB ACTION PERFROM");

	if (ActionIndex == 0)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, "EXIT GRAB");
		IInterface_CombatCharacter::Execute_ExitGrabPose(GrabbedCharacter);
		ExitGrab();
		return;
	}
	if (ActionIndex == 2)
	{
		if (IInterface_CombatCharacter::Execute_IsFinishable(GrabbedCharacter))
		{
			ChangeCameraBoom(CameraBoom);
			InputsComponent->SwitchInputMapping(EInputMode::Finisher);
			FinishersComponent->HandleFinisherRequest(GrabbedCharacter);
			GrabbedCharacter = nullptr;
			return;
		}
		else 
		{
			//IInterface_CombatCharacter::Execute_ExitGrabPose(GrabbedCharacter);
			//ExitGrab();
			return;
		}
	}
	else
	{
		int32 CurrentIndex = 0;
		for (const TPair<UAnimMontage*, UAnimMontage*>& Pair : GrabPosePair)
		{
			if (CurrentIndex == ActionIndex)
			{
				ChangeCameraBoom(CameraBoom);
				InputsComponent->SwitchInputMapping(EInputMode::Finisher);
				UAnimMontage* Key = Pair.Key;
				UAnimMontage* Value = Pair.Value;

				FinishersComponent->Victim = GrabbedCharacter;
				GetMesh()->GetAnimInstance()->Montage_Play(Key, 1.f, EMontagePlayReturnType::MontageLength, 0.0f, true);

				CurrentGrabAttack = Key;
				IInterface_CombatCharacter::Execute_ReceiveGrabAttack(GrabbedCharacter, Value);
				DamagableComponent->bIsInvincible = true;
				GetCharacterMovement()->bOrientRotationToMovement = true;
				bUseControllerRotationYaw = false;
				GrabbedCharacter = nullptr;
				return;
			}
			CurrentIndex++;
		}
	}
}

void AFightingProjectCharacter::GrabAttackEnd()
{
	//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, "SET CONTROL ROTATION");

	FRotator ControllerRotation = GetControlRotation();
	ControllerRotation.Yaw += 180.0f;
	Controller->SetControlRotation(ControllerRotation);
}

bool AFightingProjectCharacter::TryFrozenAttack(const FInputActionInstance& Instance)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "TRY FROZEN ATTACK");

	AActor* TargetActor = AttackTarget;

	if (AttackTarget)
	{
		float TargetDist = StaticFunctions::GetHorizontalDistanceBetweenActors(this, AttackTarget);
		if (TargetDist > 110.f)
		{
			TargetActor = InteractableCombatCharacter;
		}
	}

	if (TargetActor == nullptr)
	{
		return false;
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "INTERACTABLE COMBAT CHAR");

	EFrozenState FrozenAttackType = IInterface_CombatCharacter::Execute_CanReceiveFrozenAttack(TargetActor);
	
	if (FrozenAttackType == EFrozenState::Fell)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "FELL FROZEN ATTACK");
		FinishersComponent->Victim = TargetActor;
		SingleAttack(AttacksComponent->FellFrozenAttacks, Instance);
		return true;
	}
	else if (FrozenAttackType == EFrozenState::Juggling)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "JUGGLE FROZEN ATTACK");
		FinishersComponent->Victim = TargetActor;

		CombatPauseGlobalTimeDilation();

		OnAnimationDoDamage.RemoveAll(this);
		OnAnimationDoDamage.AddDynamic(this, &AFightingProjectCharacter::CombatResumeGlobalTimeDilation);

		SingleAttack(AttacksComponent->JuggleFrozenAttacks, Instance);
		return true;
	}

	return false;
}


void AFightingProjectCharacter::LoadPersistentData()
{
	UGameInstance* instance = UGameplayStatics::GetGameInstance(GetWorld());
	UFighterGameInstance* GameInstance = Cast<UFighterGameInstance>(instance);


	FPersistentData PersistentData = GameInstance->PersistentData;
	if (GameInstance->bLoadChallengeLevel)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, "LOAD LEVEL CHALLENGE");
		GameInstance->bLoadChallengeLevel = false;


		AttacksComponent->AttackMovesetData = PersistentData.EquippedMoveset;
		AttacksComponent->MaxComboAttacks = PersistentData.MaxComboAttacks;
		return;
	}

	if (GameInstance->bPlayerInitialized == true)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, "PLAYER INITIALIZED");

		DamagableComponent->MaxHealth = PersistentData.MaxHealth;
		SpecialMovesComponent->MaxOrbsNumber = PersistentData.MaxOrbs;

		AttacksComponent->MaxComboAttacks = PersistentData.MaxComboAttacks;
		AttacksComponent->AttackMovesetData = PersistentData.EquippedMoveset;
	}
	else
	{
		GameInstance->bPlayerInitialized = true;
	}

	if (GameInstance->bGameOver == true)
	{
		DamagableComponent->Health = DamagableComponent->Health / 2;
		SpecialMovesComponent->OrbsNumber = 0;
	}

	if (GameInstance->bShouldLoadData)
	{

		DamagableComponent->Health = PersistentData.Health;
		SpecialMovesComponent->OrbsNumber = PersistentData.Orbs;

		GameInstance->bShouldLoadData = false;
	}
}

