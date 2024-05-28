// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/WidgetCharacterAttributes.h"
#include "Characters/BaseCharacter.h"
#include "Characters/PlayersControllers/BasePlayerController.h"
#include "Components/CharacterComponents/CharacterAttributeComponent.h"

void UWidgetCharacterAttributes::NativeConstruct()
{
	Super::NativeConstruct();
	checkf(GetOwningPlayer()->IsA<ABasePlayerController>(), TEXT("UWidgetCharacterAttributes::NativeConstruct() UWidgetCharacterAttributes can be used only with ABaseCharacter"));
	ABasePlayerController* PlayerController = StaticCast<ABasePlayerController*>(GetOwningPlayer());
	if (PlayerController->GetBaseCharacter()) {
		switch (WidgetType)
		{
		case EWidgetType::Health:
			PlayerController->GetBaseCharacter()->GetCharacterAttributeComponent_Mutable()->OnHealthPercentChanged.BindUObject(this, &UWidgetCharacterAttributes::UpdatePercent);
			break;
		case EWidgetType::Oxygen:
			PlayerController->GetBaseCharacter()->GetCharacterAttributeComponent_Mutable()->OnOxigenPercentChanged.BindUObject(this, &UWidgetCharacterAttributes::UpdatePercent);
			break;
		case EWidgetType::Stamina:
			PlayerController->GetBaseCharacter()->GetCharacterAttributeComponent_Mutable()->OnStaminaPercentChanged.BindUObject(this, &UWidgetCharacterAttributes::UpdatePercent);
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