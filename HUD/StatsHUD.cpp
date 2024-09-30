// Fill out your copyright notice in the Description page of Project Settings.


#include "StatsHUD.h"
#include "Components/Image.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Blueprint/WidgetTree.h"
#include "Components/SizeBox.h"
#include "Components/ProgressBar.h"
#include "UObject/ConstructorHelpers.h"

void UStatsHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!DamagableComponent.IsValid())
		return;

	
	HealthBar->SetPercent(DamagableComponent->GetHealth() / DamagableComponent->GetMaxHealth());
	if (HealthBar->GetPercent() <= 0.0f)
	{
		HealthBar->SetVisibility(ESlateVisibility::Hidden);
		StunBar->SetVisibility(ESlateVisibility::Hidden);
	}

	StunBar->SetPercent(DamagableComponent->StunValue / DamagableComponent->MaxStunValue);
	
}

void UStatsHUD::UpdateOrbHUD()
{
	if (SpecialMovesComponent == nullptr)
		return;

	if (OrbImage && EmptyOrbImage && OrbsBox && EmptyOrbsBox)
	{
		// Create orbs if necessary
		if (CreatedOrbs.Num() < SpecialMovesComponent->MaxOrbsNumber - 1)
		{
			for (int32 i = CreatedOrbs.Num(); i < SpecialMovesComponent->MaxOrbsNumber - 1; ++i)
			{
				// Create new orb image
				UImage* NewImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), FName(*FString::Printf(TEXT("OrbImage_%d"), i)));
				NewImage->SetBrush(OrbImage->Brush);
				CreatedOrbs.Add(NewImage);
				UHorizontalBoxSlot* NewOrbSlot = OrbsBox->AddChildToHorizontalBox(NewImage);
				NewOrbSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);

				// Create new empty orb image
				UImage* NewEmptyImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), FName(*FString::Printf(TEXT("EmptyOrbImage_%d"), i)));
				NewEmptyImage->SetBrush(EmptyOrbImage->Brush);
				CreatedEmptyOrbs.Add(NewEmptyImage);
				UHorizontalBoxSlot* NewEmptyOrbSlot = EmptyOrbsBox->AddChildToHorizontalBox(NewEmptyImage);
				NewEmptyOrbSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
			}
		}

		// Hide all orb images initially
		for (UImage* Orb : CreatedOrbs)
		{
			Orb->SetVisibility(ESlateVisibility::Hidden);
		}

		// Update visibility based on OrbsNumber
		if (SpecialMovesComponent->OrbsNumber > 0)
		{
			OrbImage->SetVisibility(ESlateVisibility::Visible);
			for (int32 i = 0; i < SpecialMovesComponent->OrbsNumber - 1; ++i)
			{
				if (i < CreatedOrbs.Num())
				{
					CreatedOrbs[i]->SetVisibility(ESlateVisibility::Visible);
				}
			}
		}
		else
		{
			OrbImage->SetVisibility(ESlateVisibility::Hidden);
		}

		// Hide extra empty orbs if MaxOrbsNumber decreases
		for (UImage* EmptyOrb : CreatedEmptyOrbs)
		{
			EmptyOrb->SetVisibility(ESlateVisibility::Hidden);
		}

		// Show correct number of empty orbs based on MaxOrbsNumber
		if (SpecialMovesComponent->MaxOrbsNumber > 0)
		{
			EmptyOrbImage->SetVisibility(ESlateVisibility::Visible);
			for (int32 i = 0; i < SpecialMovesComponent->MaxOrbsNumber - 1; ++i)
			{
				if (i < CreatedEmptyOrbs.Num())
				{
					CreatedEmptyOrbs[i]->SetVisibility(ESlateVisibility::Visible);
				}
			}
		}
		else
		{
			EmptyOrbImage->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UStatsHUD::ToggleBarsVisibility(bool bVisible)
{
	if (bVisible)
	{
		HealthBar->SetVisibility(ESlateVisibility::Visible);
		StunBar->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		HealthBar->SetVisibility(ESlateVisibility::Hidden);
		StunBar->SetVisibility(ESlateVisibility::Hidden);
	}
}

