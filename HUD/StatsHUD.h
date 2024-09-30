// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Component_Damagable.h"
#include "Component_SpecialMoves.h"
#include "StatsHUD.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class FIGHTINGPROJECT_API UStatsHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetDamagableComponent(UComponent_Damagable* DamagableComp) { DamagableComponent = DamagableComp; }
	void SetSpecialMovesComponent(UComponent_SpecialMoves* SpecialComp) { SpecialMovesComponent = SpecialComp; }

	void UpdateOrbHUD();
	void ToggleBarsVisibility(bool bVisible);

protected:

	TWeakObjectPtr<UComponent_Damagable> DamagableComponent;
	TWeakObjectPtr<UComponent_SpecialMoves> SpecialMovesComponent;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite);
	UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget));
	UProgressBar* StunBar;

	UPROPERTY(meta = (BindWidget));
	UTextBlock* CurrentHealthLabel;
	UPROPERTY(meta = (BindWidget));
	UTextBlock* MaxHealthLabel;
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UHorizontalBox* OrbsBox;

	UPROPERTY(meta = (BindWidget))
	UImage* OrbImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UHorizontalBox* EmptyOrbsBox;
	UPROPERTY(meta = (BindWidget))
	UImage* EmptyOrbImage;

	TArray<UImage*> CreatedOrbs;
	TArray<UImage*> CreatedEmptyOrbs;

	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	float InitialHealthBarWidth = 200.0f;
	float HealthBarScaleFactor = 1.0f;

};
