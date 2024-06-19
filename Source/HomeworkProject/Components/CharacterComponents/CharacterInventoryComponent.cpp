// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/CharacterComponents/CharacterInventoryComponent.h"

#include "Inventory/Items/InventoryItem.h"
#include "UI/Widget/Inventory/InventoryViewWidget.h"

UCharacterInventoryComponent::UCharacterInventoryComponent()
{
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

void UCharacterInventoryComponent::DecreaseCountInExistSlot(FName ItemID, int32 DecreaseCount)
{
	int32 RemainingCount = DecreaseCount;
	while (RemainingCount > 0)
	{
		int32 Index = InventorySlots.FindLastByPredicate([=](const FInventorySlot& Slot)
		{
			if (!Slot.Item.IsValid())
			{
				return false;
			}
			return Slot.Item->GetDataTableID() == ItemID;
		});
		if (Index == INDEX_NONE)
		{
			return;
		}
		FInventorySlot& Slot = InventorySlots[Index];
		if (Slot.Item->GetCount() < DecreaseCount)
		{
			RemainingCount -= Slot.Item->GetCount();
			Slot.ClearSlot();
		} else
		{
			Slot.Item->SetCount(Slot.Item->GetCount() - RemainingCount);
			RemainingCount = 0;
		}
		Slot.UpdateSlotState();
	}
}

int32 UCharacterInventoryComponent::IncreaseCountInExistSlot(FName ItemID, int32 MaxCountForSlot, int32 AddedCount)
{
	int32 RemainingCount = AddedCount;
	for (FInventorySlot& Slot : InventorySlots)
	{
		if (Slot.Item == nullptr)
		{
			continue;
		}
		if (Slot.Item->GetDataTableID() == ItemID) {

			int32 Count = Slot.Item->GetCount();
			if (Count < MaxCountForSlot)
			{
				int32 AddedForSlotCount = MaxCountForSlot - Slot.Item->GetCount();
				if (RemainingCount < AddedForSlotCount)
				{
					Slot.Item->SetCount(Slot.Item->GetCount() + RemainingCount);
					Slot.UpdateSlotState();
					return 0;
				}
				else
				{
					Slot.Item->SetCount(Slot.Item->GetCount() + AddedForSlotCount);
					Slot.UpdateSlotState();
					RemainingCount -= AddedForSlotCount;
				}
			}
		}
	}
	return RemainingCount;
}

void UCharacterInventoryComponent::UpdateCount(int32 Count, FName ItemID)
{
	FInventorySlot* Slot = FindItemSlot(ItemID);
	Slot->Item->SetCount(Count);
	Slot->UpdateSlotState();
}

bool UCharacterInventoryComponent::AddItem(TWeakObjectPtr<UInventoryItem> ItemToAdd)
{
	if (!ItemToAdd.IsValid())
	{
		return false;
	}

	bool Result = false;

	FInventorySlot* FreeSlot = FindFreeSlot();

	if(FreeSlot != nullptr)
	{
		FreeSlot->Item = ItemToAdd;
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
	OnInventorySlotUpdate.ExecuteIfBound();
}

