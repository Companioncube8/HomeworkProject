// Fill out your copyright notice in the Description page of Project Settings.


#include "Utils/HomeworkDataTableUtils.h"
#include "HomeworkProjectTypes.h"
#include "Inventory/Items/InventoryItem.h"

FWeaponTableRow* HomeworkDataTableUtils::FindWeaponData(const FName WeaponID)
{
	static const FString ContextString(TEXT("Find Weapon Data"));

	UDataTable* WeaponDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/HomeworkProject/Core/Data/DataTables/DT_WeaponList.DT_WeaponList"));

	if (WeaponDataTable == nullptr)
	{
		return nullptr;
	}

	return WeaponDataTable->FindRow<FWeaponTableRow>(WeaponID, ContextString);
}

FItemTableRow* HomeworkDataTableUtils::FindInventoryItemData(const FName ItemID)
{
	static const FString ContextString(TEXT("Find Item Data"));

	UDataTable* InventoryItemDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/HomeworkProject/Core/Data/DataTables/DT_InventoryItemList.DT_InventoryItemList"));

	if (InventoryItemDataTable == nullptr)
	{
		return nullptr;
	}

	return InventoryItemDataTable->FindRow<FItemTableRow>(ItemID, ContextString);
}

FAmmoTableRow* HomeworkDataTableUtils::FindAmmoData(const FName ItemID)
{
	static const FString ContextString(TEXT("Find Ammo Data"));

	UDataTable* AmmoDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/HomeworkProject/Core/Data/DataTables/DT_AmmoList.DT_AmmoList"));

	if (AmmoDataTable == nullptr)
	{
		return nullptr;
	}
	return AmmoDataTable->FindRow<FAmmoTableRow>(ItemID, ContextString);
}


void HomeworkDataTableUtils::GetAllAmmoNames(TArray<FName>& OutArray)
{
	static const FString ContextString(TEXT("Find Ammo Data"));

	UDataTable* AmmoDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/HomeworkProject/Core/Data/DataTables/DT_AmmoList.DT_AmmoList"));

	if (AmmoDataTable == nullptr)
	{
		return;
	}
	OutArray = AmmoDataTable->GetRowNames();
}

FInventoryItemRow* HomeworkDataTableUtils::FindItemData(const FName ItemID)
{
	static const FString ContextString(TEXT("Find Data"));

	UDataTable* WeaponDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/HomeworkProject/Core/Data/DataTables/DT_WeaponList.DT_WeaponList"));

	if (FInventoryItemRow* Row = WeaponDataTable->FindRow<FWeaponTableRow>(ItemID, ContextString))
	{
		return Row;
	}

	UDataTable* InventoryItemDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/HomeworkProject/Core/Data/DataTables/DT_InventoryItemList.DT_InventoryItemList"));

	if (FInventoryItemRow* Row = InventoryItemDataTable->FindRow<FItemTableRow>(ItemID, ContextString))
	{
		return Row;
	}

	UDataTable* AmmoDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/HomeworkProject/Core/Data/DataTables/DT_AmmoList.DT_AmmoList"));
	if (FInventoryItemRow* Row = AmmoDataTable->FindRow<FAmmoTableRow>(ItemID, ContextString))
	{
		return Row;
	}

	return nullptr;
}