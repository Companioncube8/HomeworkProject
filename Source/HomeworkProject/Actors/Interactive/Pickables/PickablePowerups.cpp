// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Interactive/Pickables/PickablePowerups.h"

#include "HomeworkProjectTypes.h"
#include "Characters/BaseCharacter.h"
#include "Inventory/Items/InventoryItem.h"
#include "Utils/HomeworkDataTableUtils.h"

APickablePowerups::APickablePowerups()
{
	PowerupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PowerupMesh"));
	SetRootComponent(PowerupMesh);
}

FName APickablePowerups::GetActionEventName() const
{
	return ActionInteract;
}

void APickablePowerups::Interact(ABaseCharacter* Character)
{
	FItemTableRow* ItemData = HomeworkDataTableUtils::FindInventoryItemData(GetDataTableID());

	if (ItemData == nullptr)
	{
		return;
	}

	TWeakObjectPtr<UInventoryItem> Item = TWeakObjectPtr<UInventoryItem>(NewObject<UInventoryItem>(Character, ItemData->InventoryItemClass));
	Item->Initialize(DataTableId, ItemData->ItemDescription);
	Item->SetCount(1);

	const bool bPickedUp = Character->PickupItem(Item);
	if (bPickedUp)
	{
		Destroy();
	}
}
