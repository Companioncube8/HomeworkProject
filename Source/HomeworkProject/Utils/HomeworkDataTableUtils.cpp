// Fill out your copyright notice in the Description page of Project Settings.


#include "Utils/HomeworkDataTableUtils.h"
#include "HomeworkProjectTypes.h"
#include "Inventory/Items/InventoryItem.h"

FWeaponTableRow* HomeworkDataTableUtils::FindWeaponData(FName WeaponID)
{
	static const FString ContextString(TEXT("Find Weapon Data"));

	UDataTable* WeaponDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/HomeworkProject/Core/Data/DataTables/DT_WeaponList.DT_WeaponList"));

	if (WeaponDataTable == nullptr)
	{
		return nullptr;
	}

	return WeaponDataTable->FindRow<FWeaponTableRow>(WeaponID, ContextString);
}
