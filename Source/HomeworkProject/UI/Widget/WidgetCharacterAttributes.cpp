// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/WidgetCharacterAttributes.h"
#include "Characters/BaseCharacter.h"
#include "Components/CharacterComponents/CharacterAttributeComponent.h"

void UWidgetCharacterAttributes::NativeConstruct()
{
	Super::NativeConstruct();
	checkf(GetOwningPlayerPawn()->IsA<ABaseCharacter>(), TEXT("UWidgetCharacterAttributes::NativeConstruct() UWidgetCharacterAttributes can be used only with ABaseCharacter"));
	ABaseCharacter* Character = StaticCast<ABaseCharacter*>(GetOwningPlayerPawn());
	if (Character) {
		switch (WidgetType)
		{
		case EWidgetType::Health:
			Character->GetCharacterAttributeComponent_Mutable()->OnHealthPercentChanged.BindUObject(this, &UWidgetCharacterAttributes::UpdatePercent);
			break;
		case EWidgetType::Oxygen:
			Character->GetCharacterAttributeComponent_Mutable()->OnOxigenPercentChanged.BindUObject(this, &UWidgetCharacterAttributes::UpdatePercent);
			break;
		case EWidgetType::Stamina:
			Character->GetCharacterAttributeComponent_Mutable()->OnStaminaPercentChanged.BindUObject(this, &UWidgetCharacterAttributes::UpdatePercent);
			break;
		}
	}
}

void UWidgetCharacterAttributes::UpdatePercent(float NewPercent)
{
	Percent = NewPercent;
	if (Percent < 1 || bNeedShowWhenFill)
	{
		SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		SetVisibility(ESlateVisibility::Hidden);
	}
}