// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Component_Inputs.generated.h"


UENUM(BlueprintType)
enum class EInputMode : uint8
{
	None UMETA(DisplayName = "None"),
	PauseMenu UMETA(DisplayName = "PauseMenu"),
	SpecialMoveMenu UMETA(DisplayName = "SpecialMoveMenu"),
	Finisher UMETA(DisplayName = "Finisher"),
	Grab UMETA(DisplayName = "Grab")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnExitSpecialMove);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpecialMoveSelect, int32, ActionIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGrabAction, int32, ActionIndex);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FIGHTINGPROJECT_API UComponent_Inputs : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UComponent_Inputs();

	FOnExitSpecialMove OnExitSpecialMoveMenu;
	FOnSpecialMoveSelect OnSelectSpecialMove;
	FOnGrabAction OnGrabActionEvent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	EInputMode CurrentInputMode;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* UIMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* FinisherMappingContext;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* GrabMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* SelectAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* CancelAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ActionButton1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ActionButton2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ExitSpecialAction;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION()
	void UIMoveAction();
	UFUNCTION()
	void UISelectAction();

	UFUNCTION()
	void UICancelAction();

	UFUNCTION()
	void UIActionButton1();

	UFUNCTION()
	void UIActionButton2();

	UFUNCTION()
	void ExitSpecialSelection();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void SwitchInputMapping(EInputMode InputMode);

	UFUNCTION()
	void SetupInputs(class UInputComponent* PlayerInputComponent);


};
