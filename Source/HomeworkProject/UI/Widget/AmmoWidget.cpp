// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/AmmoWidget.h"
#include "Characters/BaseCharacter.h"
#include "Components/CharacterComponents/CharacterEquipmentComponent.h"

void UAmmoWidget::NativeConstruct()
{
	Super::NativeConstruct();
	checkf(GetOwningPlayerPawn()->IsA<ABaseCharacter>(), TEXT("UAmmoWidget::NativeConstruct() UAmmoWidget can be used only with ABaseCharacter"));
	ABaseCharacter* Character = StaticCast<ABaseCharacter*>(GetOwningPlayerPawn());
	if (Character) {
		Character->GetCharacterEquipmentComponent_Mutable()->OnCurrentWeaponAmmoChangedEvent.AddUObject(this, &UAmmoWidget::UpdateAmmoCount);
		Character->GetCharacterEquipmentComponent_Mutable()->OnThrowableItemsCountChangedEvent.BindUObject(this, &UAmmoWidget::UpdateThrowableAmmoCount);
	}
}

void UAmmoWidget::UpdateAmmoCount(int32 NewAmmo, int32 NewTotalAmmo)
{
	Ammo = NewAmmo;
	TotalAmmo = NewTotalAmmo;
}

void UAmmoWidget::UpdateThrowableAmmoCount(int32 NewThrowableAmmo)
{
	ThrowableAmmo = NewThrowableAmmo;
}
