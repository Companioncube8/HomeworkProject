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
	EEquipableItemType GetCurrentEquippedItemType() const;

	ARangeWeaponItem* GetCurrentRangeWeapon() const;

	AMeleeWeaponItem* GetCurrentMeleeWeapon() const;

	TDelegate<void(int32)> OnThrowableItemsCountChangedEvent;
	FOnCurrentWeaponAmmoChanged OnCurrentWeaponAmmoChangedEvent;
	FOnEquippedItemChanged OnEquippedItemChanged;

	bool IsAmmunitionForCurrentWeaponAvalible() const { return GetAvailableAmunitionForCurrentWeapon() > 0; }

	void ReloadCurrentWeapon();

	void EquipItemInSlot(EEquipmentSlots Slot);

	void EquipNextItem();
	void EquipPreviousItem();
	void AttachCurrentItemToEquippedSocket();

	bool IsEquipping() const;

	void ReloadAmmoInCurrentWeapon(int32 NumberOfAmmo = 0, bool bCheckIsFull = false);

	void LaunchCurrentThrowableItem();

	void ChangeCurrentWeaponFireMode();
<<<<<<< Updated upstream
=======

	bool AddEquipmentItemToSlot(const TSubclassOf<AEquipableItem> EquipableItemClass, int32 SlotIndex);
	void RemoveItemFromSlot(int32 SlotIndex);

	void OpenViewEquipment(APlayerController* PlayerController);
	void CloseViewEquipment();
	bool IsViewVisible() const;

	void OpenWeaponWheel(APlayerController* PlayerController);

	bool IsSelectingWeapon() const;

	void ConfirmWeaponSelection() const;

	const TArray<AEquipableItem*>& GetItems() const;

	virtual void OnLevelDeserialized_Implementation() override;

	void AttachProjectile(FName SocketName);
>>>>>>> Stashed changes
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lodout")
	TMap<EAmunitionType, int32> MaxAmunitionAmount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lodout")
	TMap<EEquipmentSlots, TSubclassOf<class AEquipableItem>> ItemsLoadout;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lodout")
	TSet<EEquipmentSlots> IgnoreSlotsWhileSwitching;

private:
	void CreateLoadout();

	void EquipAnimationFinished();

	class ARangeWeaponItem* CurrentEquippedWeapon;
	TWeakObjectPtr<class ABaseCharacter> CachedBaseCharacter;

	UFUNCTION()
	void OnCurrentWeaponAmmoChanged(int32 Ammo);

	TAmmunitionArray AmmunitionArray;
	TItemsArray ItemsArray;

	int32 GetAvailableAmunitionForCurrentWeapon() const;

	UFUNCTION()
	void OnWeaponReloadComplete();
	void UnEquipCurrentItem();

	AEquipableItem* CurrentEquippedItem;
	AThrowableItem* CurrentThrowableItem;
	AMeleeWeaponItem* CurrentMeleeWeapon;

	EEquipmentSlots CurrentEquippedSlot;
	EEquipmentSlots PreviousEquippedSlot;

	FDelegateHandle OnCurrentWeaponAmmoChangedHandle;
	FDelegateHandle OnCurrentReloadAmmoChangedHandle;

	uint32 NextItemArraySlotIndex(uint32 CurrentSlotIndex);
	uint32 PreviousItemArraySlotIndex(uint32 CurrentSlotIndex);

	bool bIsEquipping = false;
	FTimerHandle EquipTimer;
<<<<<<< Updated upstream
};
=======

	UEquipmentViewWidget* ViewWidget;

	void AutoEquip();
	void InitializeAmunition();

	UPROPERTY(VisibleAnywhere)
	UWeaponWheelWidget* WeaponWheelWidget;
};
>>>>>>> Stashed changes
