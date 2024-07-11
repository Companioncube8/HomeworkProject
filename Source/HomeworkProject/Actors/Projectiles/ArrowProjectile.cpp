// Fill out your copyright notice in the Description page of Project Settings.


#include "ArrowProjectile.h"

#include "HomeworkProjectTypes.h"
#include "Components/CharacterComponents/CharacterInventoryComponent.h"
#include "Characters/BaseCharacter.h"
#include "Inventory/Items/Ammo/AmmoInventoryItem.h"
#include "Utils/HomeworkDataTableUtils.h"

AArrowProjectile::AArrowProjectile()
{
	SetReplicates(true);
}

void AArrowProjectile::Interact(ABaseCharacter* Character)
{
	if (FAmmoTableRow* AmmoData = HomeworkDataTableUtils::FindAmmoData(ItemID))
	{
		const UCharacterInventoryComponent* CharacterInventoryComponent = Character->GetCharacterInventoryComponent();
		int32 RemainingCount = Character->IncreaseCountInExistSlot(ItemID, AmmoData->MaxAmmoInSlot, 1, AmmoData->AmunitionType);
		SetOwner(Character);

		if (RemainingCount > 0)
		{
			TWeakObjectPtr<UAmmoInventoryItem> Ammo = NewObject<UAmmoInventoryItem>(Character);
			Ammo->Initialize(ItemID, AmmoData->ItemDescription);
			Ammo->SetAmmunitionType(AmmoData->AmunitionType);
			Ammo->SetCount(RemainingCount);
			if (Character->PickupItem(Ammo))
			{
				Character->DestroyInteractiveObject(this);
			}
		}
		else
		{
			Character->DestroyInteractiveObject(this);
		}
	}
}

FName AArrowProjectile::GetActionEventName() const
{
	return ActionInteract;
}

