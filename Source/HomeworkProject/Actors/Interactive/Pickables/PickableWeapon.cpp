// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Interactive/Pickables/PickableWeapon.h"
#include "HomeworkProjectTypes.h"
#include "Characters/BaseCharacter.h"
#include "Inventory/Items/InventoryItem.h"
#include "Utils/HomeworkDataTableUtils.h"

APickableWeapon::APickableWeapon()
{
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);
}

FName APickableWeapon::GetActionEventName() const
{
	return ActionInteract;
}

void APickableWeapon::Interact(ABaseCharacter* Character)
{
	if (FWeaponTableRow* WeaponRow = HomeworkDataTableUtils::FindWeaponData(DataTableId))
	{
		Character->AddEquipmentItem(WeaponRow->EquipableActor);
		Destroy();
	}
}
