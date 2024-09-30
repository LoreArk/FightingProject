// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CombatCharacter.h"
#include "DoOnce.h"
#include "InputActionValue.h"
#include "F_AttackSettings.h"
#include "Components/TimelineComponent.h"
#include "Engine/World.h"
#include "Templates/Tuple.h"
#include "Public/Component_Damagable.h"
#include "Component_ObjectPool.h"
#include "GameFramework/Character.h"
#include "TimerManager.h"
#include "Public/Interface_CombatCharacter.h"
#include "Public/Component_Inputs.h"
#include "Components/SkeletalMeshComponent.h"
#include "Public/Component_SpecialMoves.h"
#include "Component_Attacks.h"
#include "Interface_Damagable.h"
#include "Public/Interface_CombatCharacter.h"
#include "Public/F_HitReactionSettings.h"
#include "Public/Interface_Player.h"
#include "Public/Interface_Finishers.h"
#include "Public/Component_Finishers.h"
#include "F_PersistentData.h"
#include "FightingProjectCharacter.generated.h"

UENUM(BlueprintType)
enum class ECombatState : uint8
{
	None UMETA(DisplayName = "None"),
	SpecialMove UMETA(DisplayName = "SpecialMove"),
	Finisher UMETA(DisplayName = "Finisher")
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSpecialMoveExecute);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGamePause);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeaponPickUp, FString, Name, int32, Uses);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponDrop);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponUsed, int32, Uses);


UCLASS(config=Game)
class AFightingProjectCharacter : public ACombatCharacter, public IInterface_Player
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpecialActionsCameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* HoldingObjectCameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* GrabCameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	
	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* InteractAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* RunAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* Attack_1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* Attack_2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* Attack_3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	const class UInputAction* CurrentAttackInput;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* DodgeStanceAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ChangeTargetAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* SpecialAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* PauseAction;
	//
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction", meta = (AllowPrivateAccess = "true"))
	class AActor* InteractionTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction", meta = (AllowPrivateAccess = "true"))
	class AActor* InteractableCombatCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class AActor* GatherableItem;

	UPROPERTY()
	FVector2D MovementVector2D;

	UPROPERTY()
	bool bRunning;

	UPROPERTY()
	bool bRunInput;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UComponent_Inputs* InputsComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	UAnimMontage* PickUpBoxAnimation;

	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	UAnimMontage* PickUpWeaponAnimation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations", meta = (AllowPrivateAccess = "true"))
	TMap<UAnimMontage*, UAnimMontage*> GrabPosePair;

	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	UAnimMontage* GrabPoseAnimation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animations", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* TargetCarryAnimation;

	UPROPERTY()
	TArray<AActor*> AttackRangeActors;

	UPROPERTY()
	TArray<AActor*> AttackRangeActorsSaved;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack Target", meta = (AllowPrivateAccess = "true"))
	AActor* AttackTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Aim System", meta = (AllowPrivateAccess = "true"))
	TArray<AActor*> AttackTargets;

	UPROPERTY()
	TArray<FTimerHandle> TimerHandles;

	class UAIPerceptionStimuliSourceComponent* StimulusSource;

	void SetupStimulusSource();

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> HudClass;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> SpecialMoveUI;

	FOnSpecialMoveExecute OnSpecialMoveExecute;


	UPROPERTY()
	UAnimMontage* PriorityFinisherMontage;

	UPROPERTY()
	UAnimMontage* PriorityHitMontage;

	UPROPERTY()
	UDataTable* CarriedObjectMovesetData;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grab System", meta = (AllowPrivateAccess = "true"))
	class AActor* GrabbedCharacter;

public:
	
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnWeaponPickUp OnWeaponPickUp;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnWeaponDrop OnWeaponDrop;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnWeaponUsed OnWeaponUse;

	FOnGamePause OnGamePause;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UComponent_SpecialMoves* SpecialMovesComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UComponent_Attacks* AttacksComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bDodgeStance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bSaveAttack;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bCanSaveAttack;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int AttackCount;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int ComboNumber;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanDodge;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanTurn180;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bCanAct;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<UAnimMontage*, FAttackSettings> CurrentAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackSlots")
	TMap<UAnimMontage*, FAttackSettings> SprintAttack_1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackSlots")
	TMap<UAnimMontage*, FAttackSettings> SprintAttack_2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackSlots")
	TMap<UAnimMontage*, FAttackSettings> SprintAttack_3;

	UPROPERTY()
	ECombatState CombatState;

	UPROPERTY()
	TMap<UAnimMontage*, FAttackSettings> SavedAttack;

	UPROPERTY(BlueprintReadWrite)
	UAnimMontage* CurrentSpecialMoveMontage;

	UPROPERTY()
	bool bSprintInputFlag;

	UPROPERTY()
	bool bSprintInputValid;

	UPROPERTY()
	bool bSprinting;

	UPROPERTY()
	bool HoldingAttackInput;

	UPROPERTY()
	bool bLowAttackInputFlag;

	UFUNCTION()
	void StartSprintInputTimer();
	
	UFUNCTION()
	void StartDodgeTimer(float time);

	UFUNCTION()
	void Start180TurnTimer(float time);

	UFUNCTION()
	void SprintTimer(FTimerHandle TimerHandle);

	UFUNCTION()
	void DodgeEnableTimer(FTimerHandle TimerHandle);

	UFUNCTION()
	void Turn180EnableTimer(FTimerHandle TimerHandle);

	virtual void TakeDamage_Implementation(FDamageSettings DamageSettings, AActor* InstigatorActor) override;

	virtual void PlayHitReaction_Implementation(UAnimMontage* TargetMontage, AActor* DamageInstigator) override;

	virtual void SpawnWeaponSpawnableActor_Implementation() override;

	virtual void ExecuteFinisher_Implementation(FVector WarpingLocation, FRotator WarpingRotation, UAnimMontage* Montage) override;

	virtual AActor* GetAttackTarget_Implementation() override;

	virtual void AttackAnticipationTrace_Implementation() override;
	virtual AFightingProjectCharacter* GetPlayerCharacter_Implementation() override;
	virtual void SetGatherableItem_Implementation(AActor* Item) override;
	virtual AActor* GetGatherableItem_Implementation() override;
	virtual void AddOrbs_Implementation(int32 OrbsToAdd) override;
	virtual void AddHealth_Implementation(float HealthToAdd) override;
	virtual void AddMaxHealth_Implementation(float HealthToAdd) override;
	virtual void AddMaxOrbs_Implementation(int32 OrbsToAdd) override;

	virtual void ConsumeWeaponUse() override;

	virtual void ExitGrab() override;

	UFUNCTION()
	void OnFinisherAttackEnd(UAnimMontage* Montage, bool bInterrupted);

	AFightingProjectCharacter();
	
	UFUNCTION(BlueprintCallable)
	void HandleAttackCombo(float DeltaTime);

	UFUNCTION(BlueprintCallable)
	void EndAttackCombo();

	UFUNCTION()
	void DodgeStanceStart();

	UFUNCTION()
	void DodgeStance();

	UFUNCTION()
	void DodgeStanceExit(AActor* Actor);

	UFUNCTION()
	void DodgeStanceInputCompleted();

	UFUNCTION()
	void ChangeTargetStart();

	UFUNCTION()
	float CalculateTargetScore(AActor* Target);

	UFUNCTION(BlueprintNativeEvent)
	void DodgeAction(UAnimMontage* DodgeMontage);
	void DodgeAction_Implementation(UAnimMontage* DodgeMontage);

	UFUNCTION(BlueprintNativeEvent)
	void DisableYawControl();
	void DisableYawControl_Implementation();

	UFUNCTION(BlueprintNativeEvent)
	void EnableYawControl();
	void EnableYawControl_Implementation();

	UFUNCTION(BlueprintCallable)
	void EndHitReaction();

	UFUNCTION(BlueprintCallable)
	void ShutDownAfterDeathMontage();

	UFUNCTION()
	void ExitSpecialAttackSelection();

	UFUNCTION(BlueprintNativeEvent)
	void SpecialAttackAction(int32 ActionIndex);
	void SpecialAttackAction_Implementation(int32 ActionIndex);

	UFUNCTION()
	void HandleSpecialAttackRequest(int32 Index);

	UFUNCTION()
	void GrabAttackEnd();

	UFUNCTION()
	bool TryFrozenAttack(const FInputActionInstance& Instance);

	UFUNCTION()
	void LoadPersistentData();

protected:

	void HandleMovementInput();
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	void MoveCompleted(const FInputActionValue& Value);

	void Look(const FInputActionValue& Value);
	
	void LookStart(const FInputActionValue& Value);

	UFUNCTION()
	void Interact(const FInputActionValue& Value);

	UFUNCTION()
	void RunStart(const FInputActionValue& Value);

	UFUNCTION()
	void Run(const FInputActionValue& Value);

	UFUNCTION()
	void StopRun(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable)
	void InteractTrace();

	UFUNCTION(BlueprintNativeEvent)
	void ThrowObject();
	void ThrowObject_Implementation();

	UFUNCTION(BlueprintNativeEvent)
	void CarryObject(APickableActor* Actor);
	void CarryObject_Implementation(APickableActor* Actor);

	UFUNCTION()
	void Attack1(const FInputActionInstance& Instance);

	UFUNCTION()
	void SingleAttack(TMap<UAnimMontage*, FAttackSettings> AttackMap, const FInputActionInstance& Instance);

	UFUNCTION()
	void Attack2(const FInputActionInstance& Instance);

	UFUNCTION()
	void Attack3(const FInputActionInstance& Instance);

	UFUNCTION()
	void HoldAttackInput(const FInputActionInstance& Instance);

	UFUNCTION()
	void EndHoldAttack();

	UFUNCTION()
	void HandleSpecialAttackInput();

	UFUNCTION()
	void EnterSpecialAttackSelection();

	UFUNCTION()
	void GetAttack(TMap<UAnimMontage*, FAttackSettings> AttackMap);

	UFUNCTION()
	TArray<AActor*> DodgeStanceTrace();

	UFUNCTION()
	AActor* GetClosestTarget(TArray<AActor*> Targets);

	UFUNCTION()
	void SortTargets(TArray<AActor*>& Targets);

	UFUNCTION()
	UAnimMontage* FindAttackMontage(TMap<UAnimMontage*, FAttackSettings> AttackMap, int32 Combo, int32 Num);

	UFUNCTION(BlueprintNativeEvent)
	void PlayAttackMontageOnce(UAnimMontage* TargetMontage, int32 AttackCounter);
	void PlayAttackMontageOnce_Implementation(UAnimMontage* TargetMontage, int32 AttackCounter);

	UFUNCTION()
	void OnDeathEvent(AActor* Actor);

	UPROPERTY(VisibleAnywhere)
	FDoOnce DoOnce;

	UFUNCTION(BlueprintCallable)
	bool CanPlayAttackMontage();

	UFUNCTION()
	void ChangeCameraBoom(USpringArmComponent* CameraSpringArm);

	UFUNCTION()
	void GrabCharacter();

	UFUNCTION()
	void PauseEvent();

	UFUNCTION(BlueprintNativeEvent)
	void PerformGrabAttack(int32 ActionIndex);
	void PerformGrabAttack_Implementation(int32 ActionIndex);

	UPROPERTY()
	UAnimMontage* CurrentGrabAttack;
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	virtual void BeginPlay();

	virtual void PossessedBy(AController* NewController) override;

	virtual void Tick(float DeltaTime) override;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

