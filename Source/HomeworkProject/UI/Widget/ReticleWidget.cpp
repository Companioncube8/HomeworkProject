// Fill out your copyright notice in the Description page of Project Settings.


#include "ReticleWidget.h"
#include "Actors/Equipment/EquipableItem.h"
#include "Characters/PlayersControllers/BasePlayerController.h"
#include "Components/CharacterComponents/CharacterEquipmentComponent.h"

void UReticleWidget::NativeConstruct()
{
	Super::NativeConstruct();
	checkf(GetOwningPlayerPawn()->IsA<ABaseCharacter>(), TEXT("UReticleWidget::NativeConstruct() UReticleWidget can be used only with ABaseCharacter"));
	ABaseCharacter* Player = StaticCast<ABaseCharacter*>(GetOwningPlayerPawn());
	if (Player) {
		Player->OnAimingStateChanged.AddUObject(this, &UReticleWidget::OnAimingStateChange);
		Player->GetCharacterEquipmentComponent_Mutable()->OnEquippedItemChanged.AddUObject(this, &UReticleWidget::OnEquippedItemChanged);
	}
}

void UReticleWidget::SetupCurrentReticle()
{
	CurrentReticle = CurrentEquippedItem.IsValid() ? CurrentEquippedItem->GetReticleType() : EReticleType::None;
}

void UReticleWidget::OnEquippedItemChanged_Implementation(const AEquipableItem* EquippedItem)
{
	CurrentEquippedItem = EquippedItem;
	SetupCurrentReticle();
}

void UReticleWidget::OnAimingStateChange_Implementation(bool bIsAiming)
{
	SetupCurrentReticle();
}
