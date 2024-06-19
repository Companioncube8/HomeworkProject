// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/Items/Ammo/AmmoInventoryItem.h"

void UAmmoInventoryItem::SetAmmunitionType(EAmunitionType NewAmunitionType)
{
	AmunitionType = NewAmunitionType;
}

EAmunitionType UAmmoInventoryItem::GetAmmunitionType() const
{
	return AmunitionType;
}
