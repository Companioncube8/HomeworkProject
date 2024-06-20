// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterEquipmentComponent.h"

#include "CharacterInventoryComponent.h"
#include "HomeworkProjectTypes.h"
#include "Actors/Equipment/Throwables/ThrowableItem.h"
#include "Actors/Equipment/Weapons/MeleeWeaponItem.h"
#include "Actors/Equipment/Weapons/RangeWeaponItem.h"
#include "Characters/BaseCharacter.h"
#include "Inventory/Items/Ammo/AmmoInventoryItem.h"
#include "Net/UnrealNetwork.h"
#include "UI/Widget/Equipment/EquipmentViewWidget.h"
#include "UI/Widget/Equipment/WeaponWheelWidget.h"
#include "Utils/HomeworkDataTableUtils.h"


UCharacterEquipmentComponent::UCharacterEquipmentComponent()
{
	SetIsReplicatedByDefault(true);
}

void UCharacterEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();
	checkf(GetOwner()->IsA<ABaseCharacter>(), TEXT("UCharacterEquipmentComponent::BeginPlay() CharacterEquipmentComponent can beused only with a BaseCharacter"));
	CachedBaseCharacter = StaticCast<ABaseCharacter *>(GetOwner());
	CreateLoadout();
	AutoEquip();

}

void UCharacterEquipmentComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UCharacterEquipmentComponent, CurrentEquippedSlot);
	DOREPLIFETIME(UCharacterEquipmentComponent, AmmunitionArray);
	DOREPLIFETIME(UCharacterEquipmentComponent, ItemsArray);
}

void UCharacterEquipmentComponent::CreateLoadout()
{
	if (GetOwner()->GetLocalRole() < ROLE_Authority) {
		return;
	}
	AmmunitionArray.AddZeroed((uint32)EAmunitionType::MAX);
	for (const TPair<EAmunitionType, int32>& AmmoPair : MaxAmunitionAmount)
	{
		AmmunitionArray[(uint32)AmmoPair.Key] = FMath::Max(AmmoPair.Value, 0);
	}

	ItemsArray.AddZeroed((uint32)EEquipmentSlots::MAX);
	for (const TPair<EEquipmentSlots, TSubclassOf<AEquipableItem>>& ItemPair : ItemsLoadout)
	{
		if (!IsValid(ItemPair.Value))
		{
			continue;
		}
		AddEquipmentItemToSlot(ItemPair.Value, (int32)ItemPair.Key);
	}

	OnThrowableItemsCountChangedEvent.ExecuteIfBound(AmmunitionArray[(uint32)EAmunitionType::FragGrenades]);
	InitializeAmunition();
}

void UCharacterEquipmentComponent::InitializeAmunition()
{
	TArray<FName> NamesArray;
	HomeworkDataTableUtils::GetAllAmmoNames(NamesArray);
	for (FName ItemID : NamesArray)
	{
		if (FAmmoTableRow* AmmoData = HomeworkDataTableUtils::FindAmmoData(ItemID))
		{
			TWeakObjectPtr<UAmmoInventoryItem> Ammo = NewObject<UAmmoInventoryItem>(CachedBaseCharacter.Get());
			Ammo->Initialize(ItemID, AmmoData->ItemDescription);
			Ammo->SetAmmunitionType(AmmoData->AmunitionType);
			Ammo->SetCount(AmmunitionArray[(int32)AmmoData->AmunitionType]);
			CachedBaseCharacter->PickupItem(Ammo);
		}
	}
}

EEquipableItemType UCharacterEquipmentComponent::GetCurrentEquippedItemType() const
{
	EEquipableItemType Result = EEquipableItemType::None;
	if (IsValid(CurrentEquippedItem))
	{
		Result = CurrentEquippedItem->GetItemType();
	}
	return Result;
}

ARangeWeaponItem* UCharacterEquipmentComponent::GetCurrentRangeWeapon() const
{
	return CurrentEquippedWeapon;
}

void UCharacterEquipmentComponent::OnCurrentWeaponAmmoChanged(int32 Ammo)
{
	if (OnCurrentWeaponAmmoChangedEvent.IsBound())
	{
		OnCurrentWeaponAmmoChangedEvent.Broadcast(Ammo, GetAvailableAmunitionForCurrentWeapon());
	}
}

void UCharacterEquipmentComponent::ReloadCurrentWeapon()
{
	check(IsValid(CurrentEquippedWeapon));
	int32 AvailableAmunition = GetAvailableAmunitionForCurrentWeapon();
	if (AvailableAmunition <= 0)
	{
		return;
	}

	CurrentEquippedWeapon->StartReload();
}

int32 UCharacterEquipmentComponent::GetAvailableAmunitionForCurrentWeapon()
{
	check(GetCurrentRangeWeapon());
	return AmmunitionArray[(uint32)GetCurrentRangeWeapon()->GetAmmoType()];
}

void UCharacterEquipmentComponent::OnWeaponReloadComplete()
{
	ReloadAmmoInCurrentWeapon();
}

void UCharacterEquipmentComponent::ReloadAmmoInCurrentWeapon(int32 NumberOfAmmo, bool bCheckIsFull)
{
	int32 AvailableAmunition = GetAvailableAmunitionForCurrentWeapon();
	int32 CurrentAmmo = CurrentEquippedWeapon->GetAmmo();
	int32 AmmoToReload = CurrentEquippedWeapon->GetMaxAmmo() - CurrentAmmo;
	int32 ReloadedAmmo = FMath::Min(AvailableAmunition, AmmoToReload);

	if (NumberOfAmmo > 0)
	{
		ReloadedAmmo = FMath::Min(ReloadedAmmo, NumberOfAmmo);
	}

	AmmunitionArray[(uint32)CurrentEquippedWeapon->GetAmmoType()] -= ReloadedAmmo;
	CurrentEquippedWeapon->SetAmmo(ReloadedAmmo + CurrentAmmo);

	if (bCheckIsFull)
	{
		AvailableAmunition = AmmunitionArray[(uint32)CurrentEquippedWeapon->GetAmmoType()];
		bool bIsFullyReloaded = CurrentEquippedWeapon->GetAmmo() == CurrentEquippedWeapon->GetMaxAmmo();
		if (AvailableAmunition == 0 || bIsFullyReloaded)
		{
			CurrentEquippedWeapon->EndReload(true);
		}
	}

	CachedBaseCharacter->UpdateAmunitionCountInInventory(ReloadedAmmo, CurrentEquippedWeapon->GetAmmoType());
}

void UCharacterEquipmentComponent::UnEquipCurrentItem()
{
	if (IsValid(CurrentEquippedItem))
	{
		CurrentEquippedItem->AttachToComponent(CachedBaseCharacter->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, CurrentEquippedItem->GetUnEquippedSocketName());
		CurrentEquippedItem->UnEquip();
	}
	if (IsValid(CurrentEquippedWeapon))
	{
		CurrentEquippedWeapon->StopFire();
		CurrentEquippedWeapon->EndReload(false);
		CurrentEquippedWeapon->OnAmmoChanged.Remove((OnCurrentWeaponAmmoChangedHandle));
		CurrentEquippedWeapon->OnReloadComplete.Remove(OnCurrentReloadAmmoChangedHandle);
	}

	PreviousEquippedSlot = CurrentEquippedSlot;
	CurrentEquippedSlot = EEquipmentSlots::None;
}

void UCharacterEquipmentComponent::AttachCurrentItemToEquippedSocket()
{
	if (IsValid(CurrentEquippedItem))
	{
		CurrentEquippedItem->AttachToComponent(CachedBaseCharacter->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, CurrentEquippedItem->GetEquippedSocketName());
	}
}

void UCharacterEquipmentComponent::EquipItemInSlot(EEquipmentSlots Slot)
{
	if (bIsEquipping)
	{
		return;
	}

	if (Slot == EEquipmentSlots::PrimaryItemSlot && AmmunitionArray[(int32)EAmunitionType::FragGrenades] == 0)
	{
		return;
	}


	if ((uint32)Slot >= (uint32)ItemsArray.Num())
	{
		return;
	}

	UnEquipCurrentItem();

	CurrentEquippedItem = ItemsArray[(uint32)Slot];
	CurrentEquippedWeapon = Cast<ARangeWeaponItem>(CurrentEquippedItem);
	CurrentThrowableItem = Cast<AThrowableItem>(CurrentEquippedItem);
	CurrentMeleeWeapon = Cast<AMeleeWeaponItem>(CurrentEquippedItem);

	if (IsValid(CurrentEquippedItem))
	{
		UAnimMontage* EquipMontage = CurrentEquippedItem->GetCharacterEquipAnimMontage();
		if (IsValid(EquipMontage))
		{
			bIsEquipping = true;
			UAnimInstance* CharacterAnimInstanse = CachedBaseCharacter->GetMesh()->GetAnimInstance();
			float EquipDuration = CharacterAnimInstanse->Montage_Play(EquipMontage, 1.f, EMontagePlayReturnType::Duration);
			GetWorld()->GetTimerManager().SetTimer(EquipTimer, this, &UCharacterEquipmentComponent::EquipAnimationFinished, EquipDuration, false);
		}
		else
		{
			AttachCurrentItemToEquippedSocket();
		}
		CurrentEquippedItem->Equip();
	}

	if (IsValid(CurrentEquippedWeapon)) 
	{
		OnCurrentWeaponAmmoChangedHandle = CurrentEquippedWeapon->OnAmmoChanged.AddUFunction(this, FName("OnCurrentWeaponAmmoChanged"));
		OnCurrentReloadAmmoChangedHandle = CurrentEquippedWeapon->OnReloadComplete.AddUFunction(this, FName("OnWeaponReloadComplete"));
		if (CurrentEquippedWeapon->CanUseAmmo())
		{
			OnCurrentWeaponAmmoChanged(CurrentEquippedWeapon->GetAmmo());
		}
	}
	if (OnEquippedItemChanged.IsBound())
	{
		OnEquippedItemChanged.Broadcast(CurrentEquippedItem);
	}

	CurrentEquippedSlot = Slot;
	if(GetOwner()->GetLocalRole() == ROLE_AutonomousProxy)
	{
		Server_EquipItemInSlot(CurrentEquippedSlot);
	}
}


void UCharacterEquipmentComponent::EquipNextItem()
{
	if (CachedBaseCharacter->IsPlayerControlled())
	{
		if (IsSelectingWeapon())
		{
			WeaponWheelWidget->NextSegment();
		} else
		{
			APlayerController* PlayerController = CachedBaseCharacter->GetController<APlayerController>();
			OpenWeaponWheel(PlayerController);
		}
		return;
	}

	uint32 CurrentSlotIndex = (uint32)CurrentEquippedSlot;
	uint32 NextSlotIndex = NextItemArraySlotIndex(CurrentSlotIndex);
	while (CurrentSlotIndex == NextSlotIndex || IgnoreSlotsWhileSwitching.Contains((EEquipmentSlots)NextSlotIndex) || !IsValid(ItemsArray[NextSlotIndex]))
	{
		NextSlotIndex = NextItemArraySlotIndex(NextSlotIndex);
	}
	if (CurrentSlotIndex != NextSlotIndex)
	{
		EquipItemInSlot((EEquipmentSlots)NextSlotIndex);
	}
}

void UCharacterEquipmentComponent::EquipPreviousItem()
{
	if (CachedBaseCharacter->IsPlayerControlled())
	{
		if (IsSelectingWeapon())
		{
			WeaponWheelWidget->PreviousSegment();
		}
		else
		{
			APlayerController* PlayerController = CachedBaseCharacter->GetController<APlayerController>();
			OpenWeaponWheel(PlayerController);
		}
		return;
	}

	uint32 CurrentSlotIndex = (uint32)CurrentEquippedSlot;
	uint32 PreviousSlotIndex = PreviousItemArraySlotIndex(CurrentSlotIndex);
	while (CurrentSlotIndex == PreviousSlotIndex || IgnoreSlotsWhileSwitching.Contains((EEquipmentSlots)PreviousSlotIndex) || !IsValid(ItemsArray[PreviousSlotIndex]))
	{
		PreviousSlotIndex = PreviousItemArraySlotIndex(PreviousSlotIndex);
	}
	if (CurrentSlotIndex != PreviousSlotIndex)
	{
		EquipItemInSlot((EEquipmentSlots)PreviousSlotIndex);
	}
}

uint32 UCharacterEquipmentComponent::NextItemArraySlotIndex(uint32 CurrentSlotIndex)
{
	if (CurrentSlotIndex == ItemsArray.Num() - 1)
	{
		return  0;
	}
	else
	{
		return  CurrentSlotIndex + 1;
	}
}

uint32 UCharacterEquipmentComponent::PreviousItemArraySlotIndex(uint32 CurrentSlotIndex)
{
	if (CurrentSlotIndex == 0)
	{
		return ItemsArray.Num() - 1;
	}
	else
	{
		return  CurrentSlotIndex - 1;
	}
}

bool UCharacterEquipmentComponent::IsEquipping() const
{
	return bIsEquipping;
}

void UCharacterEquipmentComponent::EquipAnimationFinished()
{
	bIsEquipping = false;
	AttachCurrentItemToEquippedSocket();
}

void UCharacterEquipmentComponent::LaunchCurrentThrowableItem()
{
	if (CurrentThrowableItem)
	{
		AmmunitionArray[(uint32)EAmunitionType::FragGrenades] -= 1;
		OnThrowableItemsCountChangedEvent.ExecuteIfBound(AmmunitionArray[(uint32)EAmunitionType::FragGrenades]);

		CachedBaseCharacter->UpdateAmunitionCountInInventory(AmmunitionArray[(uint32)CurrentEquippedWeapon->GetAmmoType()], CurrentEquippedWeapon->GetAmmoType());

		bIsEquipping = false;
		if (CachedBaseCharacter->IsLocallyControlled()) {
			CurrentThrowableItem->Throw();
			EquipItemInSlot(PreviousEquippedSlot);
		}
	}
}

void UCharacterEquipmentComponent::ChangeCurrentWeaponFireMode()
{
	CurrentEquippedWeapon->ChangeFireMode();
	OnCurrentWeaponAmmoChanged(CurrentEquippedWeapon->GetAmmo());
}

AMeleeWeaponItem* UCharacterEquipmentComponent::GetCurrentMeleeWeapon() const
{
	return CurrentMeleeWeapon;
}

void UCharacterEquipmentComponent::AutoEquip()
{
	if (AutoEqupItemInSlot != EEquipmentSlots::None)
	{
		EquipItemInSlot(AutoEqupItemInSlot);
	}
}

void UCharacterEquipmentComponent::Server_EquipItemInSlot_Implementation(EEquipmentSlots Slot)
{
	EquipItemInSlot(Slot);
}

void UCharacterEquipmentComponent::OnRep_CurrentEquipSlot(EEquipmentSlots CurrentEquippedSlot_Old)
{
	if (CurrentEquippedSlot != CurrentEquippedSlot_Old)
	{
		EquipItemInSlot(CurrentEquippedSlot);
	}
}

void UCharacterEquipmentComponent::OnRep_ItemsArray()
{
	for (AEquipableItem* Item : ItemsArray)
	{
		if (Item)
		{
			Item->UnEquip();
		}
	}
}

bool UCharacterEquipmentComponent::AddEquipmentItemToSlot(const TSubclassOf<AEquipableItem> EquipableItemClass, int32 SlotIndex)
{
	if (!IsValid(EquipableItemClass))
	{
		return false;
	}

	AEquipableItem* DefaultItemObject = EquipableItemClass->GetDefaultObject<AEquipableItem>();
	if (!DefaultItemObject->IsSlotCompatable((EEquipmentSlots)SlotIndex))
	{
		return false;
	}

	if (!IsValid(ItemsArray[SlotIndex]))
	{
		AEquipableItem* Item = GetWorld()->SpawnActor<AEquipableItem>(EquipableItemClass);
		Item->AttachToComponent(CachedBaseCharacter->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Item->GetUnEquippedSocketName());
		Item->SetOwner(CachedBaseCharacter.Get());
		Item->UnEquip();
		ItemsArray[SlotIndex] = Item;

		if (AThrowableItem* ThrowableItem = Cast<AThrowableItem>(Item))
		{
			ThrowableItem->CreateProjectilePool();
		}
	}
	else if (DefaultItemObject->IsA<ARangeWeaponItem>())
	{
		ARangeWeaponItem* RangeWeaponObject = StaticCast<ARangeWeaponItem*>(DefaultItemObject);
		int32 AmmoSlotIndex = (int32)RangeWeaponObject->GetAmmoType();
		AmmunitionArray[SlotIndex] += RangeWeaponObject->GetMaxAmmo();
	}

	return true;
}

void UCharacterEquipmentComponent::RemoveItemFromSlot(int32 SlotIndex)
{
	if ((uint32)CurrentEquippedSlot == SlotIndex)
	{
		UnEquipCurrentItem();
	}
	ItemsArray[SlotIndex]->Destroy();
	ItemsArray[SlotIndex] = nullptr;
}

void UCharacterEquipmentComponent::OpenViewEquipment(APlayerController* PlayerController)
{
	if (!IsValid(ViewWidget))
	{
		CreateEquipmentWidgets(PlayerController);
	}

	if (!ViewWidget->IsVisible())
	{
		ViewWidget->AddToViewport();
	}
}

void UCharacterEquipmentComponent::CloseViewEquipment()
{
	if (ViewWidget->IsVisible())
	{
		ViewWidget->RemoveFromParent();
	}
}

bool UCharacterEquipmentComponent::IsViewVisible() const
{
	bool Result = false;
	if (IsValid(ViewWidget))
	{
		Result = ViewWidget->IsVisible();
	}
	return Result;
}

const TArray<AEquipableItem*>& UCharacterEquipmentComponent::GetItems() const
{
	return ItemsArray;
}

void UCharacterEquipmentComponent::CreateEquipmentWidgets(APlayerController* PlayerController)
{
	checkf(IsValid(ViewWidgetClass), TEXT("UCharacterEquipmentComponent::CreateEquipmentWidgets view widget class is not defined"));

	if (!IsValid(PlayerController))
	{
		return;
	}

	ViewWidget = CreateWidget<UEquipmentViewWidget>(PlayerController, ViewWidgetClass);
	ViewWidget->InitializeEquipmentWidget(this);

	WeaponWheelWidget = CreateWidget<UWeaponWheelWidget>(PlayerController, WeaponWheelClass);
	WeaponWheelWidget->InitializeWeaponWheelWidget(this);
}

int32 UCharacterEquipmentComponent::AddAmmo(int32 NumberOfAmmo, EAmunitionType AmmoType)
{
	AmmunitionArray[(uint32)AmmoType] += NumberOfAmmo;
	if (CurrentEquippedWeapon) {
		if (AmmoType == CurrentEquippedWeapon->GetAmmoType()) {
			OnCurrentWeaponAmmoChanged(CurrentEquippedWeapon->GetAmmo());
		}
	}
	return AmmunitionArray[(uint32)AmmoType];
}

bool UCharacterEquipmentComponent::IsSelectingWeapon() const
{
	return WeaponWheelWidget && WeaponWheelWidget->IsVisible();
}

void UCharacterEquipmentComponent::OpenWeaponWheel(APlayerController* PlayerController)
{
	if (!IsValid(WeaponWheelWidget))
	{
		CreateEquipmentWidgets(PlayerController);
	}

	if (!WeaponWheelWidget->IsVisible())
	{
		WeaponWheelWidget->AddToViewport();
	}
}

void UCharacterEquipmentComponent::ConfirmWeaponSelection() const
{
	WeaponWheelWidget->ConfirmSelection();
}
