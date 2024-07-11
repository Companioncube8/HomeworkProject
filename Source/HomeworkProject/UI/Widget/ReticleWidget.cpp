// Fill out your copyright notice in the Description page of Project Settings.


#include "ReticleWidget.h"
#include "Actors/Equipment/EquipableItem.h"
<<<<<<< Updated upstream
=======
#include "Characters/PlayersControllers/BasePlayerController.h"
#include "Components/ProgressBar.h"
>>>>>>> Stashed changes
#include "Components/CharacterComponents/CharacterEquipmentComponent.h"

void UReticleWidget::NativeConstruct()
{
	Super::NativeConstruct();
	checkf(GetOwningPlayerPawn()->IsA<ABaseCharacter>(), TEXT("UReticleWidget::NativeConstruct() UReticleWidget can be used only with ABaseCharacter"));
<<<<<<< Updated upstream
	ABaseCharacter* Character = StaticCast<ABaseCharacter*>(GetOwningPlayerPawn());
	if (Character) {
		Character->OnAimingStateChanged.AddUObject(this, &UReticleWidget::OnAimingStateChange);
		Character->GetCharacterEquipmentComponent_Mutable()->OnEquippedItemChanged.AddUObject(this, &UReticleWidget::OnEquippedItemChanged);
=======
	ABaseCharacter* Player = StaticCast<ABaseCharacter*>(GetOwningPlayerPawn());
	if (Player) {
		Player->OnAimingStateChanged.AddUObject(this, &UReticleWidget::OnAimingStateChange);
		Player->GetCharacterEquipmentComponent_Mutable()->OnEquippedItemChanged.AddUObject(this, &UReticleWidget::OnEquippedItemChanged);
		Player->OnAimingStateChanged.AddUObject(this, &UReticleWidget::OnAimingStateChange);
		Player->OnStartStringTension.AddUObject(this, &UReticleWidget::OnStartStringTension);
		Player->OnUpdateStringTensionPercent.AddUObject(this, &UReticleWidget::OnUpdateStringTensionPercent);
		Player->OnEndStringTension.AddUObject(this, &UReticleWidget::OnEndStringTension);
		Player->OnReticalShake.AddUObject(this, &UReticleWidget::OnReticalShake);
>>>>>>> Stashed changes
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

void UReticleWidget::OnStartStringTension_Implementation(float TenseTime)
{
	
}

void UReticleWidget::OnReticalShake_Implementation(FVector Offset)
{
	
}


void UReticleWidget::OnUpdateStringTensionPercent(float Percent)
{
	StringTenseProgressBar->SetPercent(Percent);
}

void UReticleWidget::OnEndStringTension_Implementation()
{
	StringTenseProgressBar->SetPercent(0);
}
