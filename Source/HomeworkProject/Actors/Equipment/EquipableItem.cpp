// Fill out your copyright notice in the Description page of Project Settings.


#include "EquipableItem.h"


EEquipableItemType AEquipableItem::GetItemType() const
{
	return ItemType;
}

FName AEquipableItem::GetUnEquippedSocketName() const
{
	return UnEquippedSocketName;
}

FName AEquipableItem::GetEquippedSocketName() const
{
	return EquippedSocketName;
}

UAnimMontage* AEquipableItem::GetCharacterEquipAnimMontage()
{
	return CharacterEquipAnimMontage;
}

void AEquipableItem::Equip()
{
	if (OnEquipmentStateChanged.IsBound())
	{
		OnEquipmentStateChanged.Broadcast(true);
	}
}

void AEquipableItem::UnEquip()
{
	if (OnEquipmentStateChanged.IsBound())
	{
		OnEquipmentStateChanged.Broadcast(false);
	}
}

EReticleType AEquipableItem::GetReticleType() const
{
	return ReticleType;
}

void AEquipableItem::SetOwner(AActor* NewOwner)
{
	Super::SetOwner(NewOwner);
	if (IsValid(NewOwner))
	{
		checkf(GetOwner()->IsA<ABaseCharacter>(), TEXT("AEquipableItem::SetOwner() only character can be owner of an equipable item"))
		CachedCharacterOwner = StaticCast<ABaseCharacter*>(GetOwner());
	} else
	{
		CachedCharacterOwner = nullptr;
	}
}

ABaseCharacter* AEquipableItem::GetCharacterOwner() const
{
	return CachedCharacterOwner.IsValid() ? CachedCharacterOwner.Get() : nullptr;
}
