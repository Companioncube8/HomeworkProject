// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HomeworkProjectTypes.h"
#include "Engine/DataTable.h"
#include "UObject/NoExportTypes.h"
#include "InventoryItem.generated.h"

class ABaseCharacter;
class UInventoryItem;
class APickableItem;
class AEquipableItem;

USTRUCT(BlueprintType)
struct FInventoryItemDescription : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item view")
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item view")
	UTexture2D* Icon;
};

USTRUCT()
struct FInventoryItemRow: public  FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon view")
	TSubclassOf<APickableItem> PickableActor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon view")
	FInventoryItemDescription ItemDescription;
};

USTRUCT()
struct FWeaponTableRow: public FInventoryItemRow
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon view")
	TSubclassOf<AEquipableItem> EquipableActor;

};

USTRUCT(BlueprintType)
struct FItemTableRow: public FInventoryItemRow
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item view")
	TSubclassOf<UInventoryItem> InventoryItemClass;
};

USTRUCT(BlueprintType)
struct FAmmoTableRow : public FInventoryItemRow
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item view")
	EAmunitionType AmunitionType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item view")
	int32 MaxAmmoInSlot;
};

UCLASS(Blueprintable)
class HOMEWORKPROJECT_API UInventoryItem : public UObject
{
	GENERATED_BODY()

public:
	void Initialize(FName DataTableID_In, const FInventoryItemDescription& Description_In);

	FName GetDataTableID() const;
	const FInventoryItemDescription& GetDescription() const;

	virtual bool IsEquipable() const;
	virtual bool IsConsumable() const;

	virtual void SetCount(int32 NewCount) { Count = NewCount; };
	virtual int32 GetCount() { return Count; }

	virtual  bool Consume(ABaseCharacter* ConsumeTarget) PURE_VIRTUAL(UInventoryItem::Consume, return false; );

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory item")
	FName DataTableID = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory item")
	FInventoryItemDescription Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory item")
	bool bIsEquipable = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Inventory item")
	int32 Count;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory item")
	bool bIsConsumable = false;
private:
	bool bIsInitialize = false;
};
