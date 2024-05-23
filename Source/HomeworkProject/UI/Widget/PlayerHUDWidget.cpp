// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUDWidget.h"

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

