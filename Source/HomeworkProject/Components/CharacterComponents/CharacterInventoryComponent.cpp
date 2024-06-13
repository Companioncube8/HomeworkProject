// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/CharacterComponents/CharacterInventoryComponent.h"

#include "Inventory/Items/InventoryItem.h"
#include "UI/Widget/Inventory/InventoryViewWidget.h"


void UCharacterInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	InventorySlots.AddDefaulted(Capacity);
}

void UCharacterInventoryComponent::CreateViewWidget(APlayerController* PlayerController)
{
	if (InventoryViewWidget)
	{
		return;
	}

	if (!PlayerController || !InventoryViewWidgetClass)
	{
		return;
	}

	InventoryViewWidget = CreateWidget<UInventoryViewWidget>(PlayerController, InventoryViewWidgetClass);
	InventoryViewWidget->InitializeViewWidget(InventorySlots);
}


void UCharacterInventoryComponent::OpenViewInventory(APlayerController* PlayerController)
{
	if (!InventoryViewWidget)
	{
		CreateViewWidget(PlayerController);
	}

	if (!InventoryViewWidget->IsVisible())
	{
		InventoryViewWidget->AddToViewport();
	}
}

void UCharacterInventoryComponent::CloseViewInventory()
{
	if (InventoryViewWidget->IsVisible())
	{
		InventoryViewWidget->RemoveFromParent();
	}
}

bool UCharacterInventoryComponent::IsViewVisible() const
{
	bool Result = false;
	if (InventoryViewWidget)
	{
		Result = InventoryViewWidget->IsVisible();
	}
	return Result;
}

int32 UCharacterInventoryComponent::GetCapacity() const
{
	return Capacity;
}

bool UCharacterInventoryComponent::HasFreeSlot() const
{
	return ItemsInInventory < Capacity;
}

bool UCharacterInventoryComponent::AddItem(TWeakObjectPtr<UInventoryItem> ItemToAdd, int32 Count)
{
	if (!ItemToAdd.IsValid() || Count < 0)
	{
		return false;
	}

	bool Result = false;

	FInventorySlot* FreeSlot = FindFreeSlot();

	if(FreeSlot != nullptr)
	{
		FreeSlot->Item = ItemToAdd;
		FreeSlot->Count = Count;
		ItemsInInventory++;
		Result = true;
		FreeSlot->UpdateSlotState();
	}

	return Result;
}

bool UCharacterInventoryComponent::RemoveItem(FName ItemID)
{
	FInventorySlot* ItemSlot = FindItemSlot(ItemID);
	if (ItemSlot != nullptr)
	{
		InventorySlots.RemoveAll([=](const FInventorySlot& Slot) {return Slot.Item->GetDataTableID() == ItemID;});
		return true;
	}
	return false;
}

TArray<FInventorySlot> UCharacterInventoryComponent::GetAllItemsCopy() const
{
	return InventorySlots;
}

TArray<FText> UCharacterInventoryComponent::GetAllItemsNames() const
{
	TArray<FText> Result;
	for (const FInventorySlot& Slot : InventorySlots)
	{
		if (Slot.Item.IsValid())
		{
			Result.Add(Slot.Item->GetDescription().Name);
		}
	}
	return Result;
}

FInventorySlot* UCharacterInventoryComponent::FindItemSlot(FName ItemID)
{
	return InventorySlots.FindByPredicate([=](const FInventorySlot& Slot) { return Slot.Item->GetDataTableID() == ItemID;});
}

FInventorySlot* UCharacterInventoryComponent::FindFreeSlot()
{
	return InventorySlots.FindByPredicate([=](const FInventorySlot& Slot) {return !Slot.Item.IsValid();});
}


void FInventorySlot::BindOnInventorySlotUpdate(const FInventorySlotUpdate& Callback) const
{
	OnInventorySlotUpdate = Callback;
}

void FInventorySlot::UnbindOnInventorySlotUpdate()
{
	OnInventorySlotUpdate.Unbind();
}

void FInventorySlot::UpdateSlotState()
{
	OnInventorySlotUpdate.ExecuteIfBound();
}

void FInventorySlot::ClearSlot()
{
	Item = nullptr;
	Count = 0;
	OnInventorySlotUpdate.ExecuteIfBound();
}

