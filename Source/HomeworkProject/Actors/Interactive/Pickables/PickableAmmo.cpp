// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Interactive/Pickables/PickableAmmo.h"

#include "HomeworkProjectTypes.h"
#include "Characters/BaseCharacter.h"
#include "Components/CharacterComponents/CharacterEquipmentComponent.h"
#include "Components/CharacterComponents/CharacterInventoryComponent.h"
#include "Inventory/Items/InventoryItem.h"
#include "Inventory/Items/Ammo/AmmoInventoryItem.h"
#include "Utils/HomeworkDataTableUtils.h"


APickableAmmo::APickableAmmo()
{
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);
}

FName APickableAmmo::GetActionEventName() const
{
	return ActionInteract;
}

void APickableAmmo::SetCount(int32 NewCount)
{
	Count = NewCount;
}


void APickableAmmo::Interact(ABaseCharacter* Character)
{
	if (FAmmoTableRow* AmmoData = HomeworkDataTableUtils::FindAmmoData(DataTableId))
	{
		const UCharacterInventoryComponent* CharacterInventoryComponent = Character->GetCharacterInventoryComponent();
		int32 RemainingCount = Character->IncreaseCountInExistSlot(DataTableId, AmmoData->MaxAmmoInSlot, Count, AmmoData->AmunitionType);

		if (RemainingCount > 0)
		{
			TWeakObjectPtr<UAmmoInventoryItem> Ammo = NewObject<UAmmoInventoryItem>(Character);
			Ammo->Initialize(DataTableId, AmmoData->ItemDescription);
			Ammo->SetAmmunitionType(AmmoData->AmunitionType);
			Ammo->SetCount(RemainingCount);
			if (Character->PickupItem(Ammo))
			{
				Destroy();
			}
		} else
		{
			Destroy();
		}
	}
}
