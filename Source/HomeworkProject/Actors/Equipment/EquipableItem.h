// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HomeworkProjectTypes.h"
#include "Characters/BaseCharacter.h"
#include "GameFramework/Actor.h"
#include "EquipableItem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquipmentStateChanged, bool, bIsEquipped);

class UAnimMontage;
UCLASS(Abstract, NotBlueprintable)
class HOMEWORKPROJECT_API AEquipableItem : public AActor
{
	GENERATED_BODY()

public:
	AEquipableItem();

	virtual void SetOwner(AActor* NewOwner) override;

	EEquipableItemType GetItemType() const;

	FName GetUnEquippedSocketName() const;

	FName GetEquippedSocketName() const;

	UAnimMontage* GetCharacterEquipAnimMontage();

	virtual void Equip();
	virtual void UnEquip();

	virtual EReticleType GetReticleType() const;

protected:
	UPROPERTY(BlueprintAssignable)
	FOnEquipmentStateChanged OnEquipmentStateChanged;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Equipable item")
	EEquipableItemType ItemType = EEquipableItemType::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipable item")
	FName UnEquippedSocketName = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipable item")
	FName EquippedSocketName = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipable item")
	UAnimMontage* CharacterEquipAnimMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Reticle")
	EReticleType ReticleType = EReticleType::None;

	ABaseCharacter* GetCharacterOwner() const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	FName DataTableId = NAME_None;

private:
	TWeakObjectPtr<ABaseCharacter> CachedCharacterOwner;

};
