// Fill out your copyright notice in the Description page of Project Settings.


#include "Component_Inputs.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerController.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "FightingProject/FightingProjectCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UComponent_Inputs::UComponent_Inputs()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UComponent_Inputs::BeginPlay()
{
	Super::BeginPlay();

	CurrentInputMode = EInputMode::None;
}

void UComponent_Inputs::UIMoveAction()
{
	//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, ("UI MOVE ACTION"));

}

void UComponent_Inputs::UISelectAction()
{
	
	switch (CurrentInputMode)
	{
	case EInputMode::None:
		break;
	case EInputMode::PauseMenu:
		break;
	case EInputMode::SpecialMoveMenu:
		OnSelectSpecialMove.Broadcast(2);
		break;
	case EInputMode::Finisher:
		break;
	case EInputMode::Grab:
		OnGrabActionEvent.Broadcast(0);
		break;
	default:
		break;
	}
}

void UComponent_Inputs::UICancelAction()
{
	switch (CurrentInputMode)
	{
	case EInputMode::None:
		break;
	case EInputMode::PauseMenu:
		break;
	case EInputMode::SpecialMoveMenu:
		OnSelectSpecialMove.Broadcast(3);
		break;
	case EInputMode::Finisher:
		break;
	case EInputMode::Grab:
		break;
	default:
		break;
	}
}

void UComponent_Inputs::UIActionButton1()
{
	switch (CurrentInputMode)
	{
	case EInputMode::None:
		break;
	case EInputMode::PauseMenu:
		break;
	case EInputMode::SpecialMoveMenu:
		OnSelectSpecialMove.Broadcast(0);
		break;
	case EInputMode::Finisher:
		break;
	case EInputMode::Grab:
		OnGrabActionEvent.Broadcast(1);
		break;
	default:
		break;
	}
}

void UComponent_Inputs::UIActionButton2()
{
	switch (CurrentInputMode)
	{
	case EInputMode::None:
		break;
	case EInputMode::PauseMenu:
		break;
	case EInputMode::SpecialMoveMenu:
		OnSelectSpecialMove.Broadcast(1);
		break;
	case EInputMode::Finisher:
		break;
	case EInputMode::Grab:
		OnGrabActionEvent.Broadcast(2);
		break;
	default:
		break;
	}
}

void UComponent_Inputs::ExitSpecialSelection()
{
	if (CurrentInputMode == EInputMode::SpecialMoveMenu)
	{
		OnExitSpecialMoveMenu.Broadcast();
	}
}

// Called every frame
void UComponent_Inputs::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UComponent_Inputs::SwitchInputMapping(EInputMode InputMode)
{
	UInputMappingContext* Context = DefaultMappingContext;
	CurrentInputMode = InputMode;

	switch (InputMode)
	{
	case EInputMode::None:
		Context = DefaultMappingContext;
		break;
	case EInputMode::PauseMenu:
		Context = UIMappingContext;
		break;
	case EInputMode::SpecialMoveMenu:
		Context = UIMappingContext;
		break;
	case EInputMode::Finisher:
		Context = FinisherMappingContext;
		break;
	case EInputMode::Grab:
		Context = GrabMappingContext;
	default:
		break;
	}

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PlayerController) return;

	// Get the enhanced input local player subsystem
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
	if (!Subsystem) return;

	// Clear existing mapping contexts
	Subsystem->ClearAllMappings();

	if (!Context)
		return;

	// Add the default mapping context
	Subsystem->AddMappingContext(Context, 1);
}

void UComponent_Inputs::SetupInputs(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(SelectAction, ETriggerEvent::Started, this, &UComponent_Inputs::UISelectAction);
		EnhancedInputComponent->BindAction(CancelAction, ETriggerEvent::Started, this, &UComponent_Inputs::UICancelAction);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Started, this, &UComponent_Inputs::UIMoveAction);
		EnhancedInputComponent->BindAction(ActionButton1, ETriggerEvent::Started, this, &UComponent_Inputs::UIActionButton1);
		EnhancedInputComponent->BindAction(ActionButton2, ETriggerEvent::Started, this, &UComponent_Inputs::UIActionButton2);
		EnhancedInputComponent->BindAction(ExitSpecialAction, ETriggerEvent::Started, this, &UComponent_Inputs::ExitSpecialSelection);
	}
}

