// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HomeworkProjectTypes.h"
#include "Inventory/Items/InventoryItem.h"
#include "AmmoInventoryItem.generated.h"

/**
 * 
 */
UCLASS()
class HOMEWORKPROJECT_API UAmmoInventoryItem : public UInventoryItem
{
	GENERATED_BODY()
public:
	void SetAmmunitionType(EAmunitionType NewAmunitionType);
	EAmunitionType GetAmmunitionType() const;

private:
	UPROPERTY()
	EAmunitionType AmunitionType;
};
