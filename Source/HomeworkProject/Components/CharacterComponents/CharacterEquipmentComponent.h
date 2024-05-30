// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HomeworkProjectTypes.h"
#include "Actors/Equipment/Weapons/RangeWeaponItem.h"
#include "Components/ActorComponent.h"
#include "CharacterEquipmentComponent.generated.h"

class AMeleeWeaponItem;
typedef TArray<class AEquipableItem*, TInlineAllocator<(uint32)EEquipmentSlots::MAX>> TItemsArray;
typedef TArray<int32, TInlineAllocator<(uint32)EAmunitionType::MAX>> TAmmunitionArray;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCurrentWeaponAmmoChanged, int32, int32)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnEquippedItemChanged, const AEquipableItem*)

class AThrowableItem;
UCLASS(BlueprintType)
class HOMEWORKPROJECT_API UCharacterEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UCharacterEquipmentComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	EEquipableItemType GetCurrentEquippedItemType() const;

	ARangeWeaponItem* GetCurrentRangeWeapon() const;

	AMeleeWeaponItem* GetCurrentMeleeWeapon() const;

	TDelegate<void(int32)> OnThrowableItemsCountChangedEvent;
	FOnCurrentWeaponAmmoChanged OnCurrentWeaponAmmoChangedEvent;
	FOnEquippedItemChanged OnEquippedItemChanged;

	void ReloadCurrentWeapon();

	void EquipItemInSlot(EEquipmentSlots Slot);

	void EquipNextItem();
	void EquipPreviousItem();
	void AttachCurrentItemToEquippedSocket();

	bool IsEquipping() const;

	void ReloadAmmoInCurrentWeapon(int32 NumberOfAmmo = 0, bool bCheckIsFull = false);

	void LaunchCurrentThrowableItem();

	void ChangeCurrentWeaponFireMode();
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lodout")
	TMap<EAmunitionType, int32> MaxAmunitionAmount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lodout")
	TMap<EEquipmentSlots, TSubclassOf<class AEquipableItem>> ItemsLoadout;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lodout")
	TSet<EEquipmentSlots> IgnoreSlotsWhileSwitching;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Lodout")
	EEquipmentSlots AutoEqupItemInSlot = EEquipmentSlots::None;

private:
	UFUNCTION(Server, Reliable)
	void Server_EquipItemInSlot(EEquipmentSlots Slot);

	void CreateLoadout();

	void EquipAnimationFinished();

	class ARangeWeaponItem* CurrentEquippedWeapon;
	TWeakObjectPtr<class ABaseCharacter> CachedBaseCharacter;

	UFUNCTION()
	void OnCurrentWeaponAmmoChanged(int32 Ammo);

	UPROPERTY(Replicated)
	TArray<int32> AmmunitionArray;

	UPROPERTY(ReplicatedUsing=OnRep_ItemsArray)
	TArray<AEquipableItem *> ItemsArray;

	UFUNCTION()
	void OnRep_ItemsArray();

	int32 GetAvailableAmunitionForCurrentWeapon();

	UFUNCTION()
	void OnWeaponReloadComplete();
	void UnEquipCurrentItem();

	AEquipableItem* CurrentEquippedItem;
	AThrowableItem* CurrentThrowableItem;
	AMeleeWeaponItem* CurrentMeleeWeapon;

	UPROPERTY(ReplicatedUsing=OnRep_CurrentEquipSlot)
	EEquipmentSlots CurrentEquippedSlot;

	UFUNCTION()
	void OnRep_CurrentEquipSlot(EEquipmentSlots CurrentEquippedSlot_Old);

	EEquipmentSlots PreviousEquippedSlot;

	FDelegateHandle OnCurrentWeaponAmmoChangedHandle;
	FDelegateHandle OnCurrentReloadAmmoChangedHandle;

	uint32 NextItemArraySlotIndex(uint32 CurrentSlotIndex);
	uint32 PreviousItemArraySlotIndex(uint32 CurrentSlotIndex);

	bool bIsEquipping = false;
	FTimerHandle EquipTimer;

	void AutoEquip();
};
