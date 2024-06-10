// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUDWidget.h"

#include "HighlightInteractable.h"
#include "ReticleWidget.h"
#include "WidgetCharacterAttributes.h"
#include "Blueprint/WidgetTree.h"
#include "Characters/BaseCharacter.h"
#include "Components/CharacterComponents/CharacterAttributeComponent.h"

UReticleWidget* UPlayerHUDWidget::GetReticalWidget()
{
	return WidgetTree->FindWidget<UReticleWidget>(ReticleWidgetName);
}

UAmmoWidget* UPlayerHUDWidget::GetAmmoWidget()
{
	return WidgetTree->FindWidget<UAmmoWidget>(AmmoWidgetName);
}

UWidgetCharacterAttributes* UPlayerHUDWidget::GetHealthWidget()
{
	return WidgetTree->FindWidget<UWidgetCharacterAttributes>(HealthWidgetName);
}

UWidgetCharacterAttributes* UPlayerHUDWidget::GetOxigenWidget()
{
	return WidgetTree->FindWidget<UWidgetCharacterAttributes>(OxigenWidgetName);
}

UWidgetCharacterAttributes* UPlayerHUDWidget::GetStaminaWidget()
{
	return WidgetTree->FindWidget<UWidgetCharacterAttributes>(StaminaWidgetName);
}

void UPlayerHUDWidget::SetHighlightInteractableActionText(FName KeyName)
{
	if (InteractableKey)
	{
		InteractableKey->SetActionText(KeyName);
	}
}

void UPlayerHUDWidget::SetHighlightInteractableVisibility(bool bIsVisible)
{
	if (!InteractableKey)
	{
		return;
	}
	if (bIsVisible)
	{
		InteractableKey->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		InteractableKey->SetVisibility(ESlateVisibility::Hidden);
	}
}
