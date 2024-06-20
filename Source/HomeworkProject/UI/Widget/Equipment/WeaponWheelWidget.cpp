// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/Equipment/WeaponWheelWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/CharacterComponents/CharacterEquipmentComponent.h"
#include "Inventory/Items/InventoryItem.h"
#include "Utils/HomeworkDataTableUtils.h"

void UWeaponWheelWidget::InitializeWeaponWheelWidget(UCharacterEquipmentComponent* EquipmentComponent)
{
	LinkedEquipmentComponent = EquipmentComponent;
}

void UWeaponWheelWidget::NextSegment()
{
	CurrentSegmentIndex++;
	if (CurrentSegmentIndex == EquipmentSlotsSegment.Num())
	{
		CurrentSegmentIndex = 0;
	}
	SelectSegment();
}

void UWeaponWheelWidget::PreviousSegment()
{
	CurrentSegmentIndex--;
	if (CurrentSegmentIndex < 0)
	{
		CurrentSegmentIndex = EquipmentSlotsSegment.Num() - 1;
	}
	SelectSegment();
}

void UWeaponWheelWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (RadialBackground && !BackgroundMaterial)
	{
		BackgroundMaterial = RadialBackground->GetDynamicMaterial();
		BackgroundMaterial->SetScalarParameterValue(FName("Segments"), EquipmentSlotsSegment.Num());
	}

	for (int i = 0; i < EquipmentSlotsSegment.Num(); ++i)
	{
		FName WidgetName = FName(FString::Printf(TEXT("ImageSegment%i"), i));
		UImage* WeaponImage = WidgetTree->FindWidget<UImage>(WidgetName);
		if (!WeaponImage)
		{
			continue;
		}

		FWeaponTableRow* WeaponData = GetTableRowForSegment(i);
		if (WeaponData == nullptr)
		{
			WeaponImage->SetOpacity(0.f);
		} else
		{
			WeaponImage->SetOpacity(1.f);
			WeaponImage->SetBrushFromTexture(WeaponData->ItemDescription.Icon);
		}
	}
}


void UWeaponWheelWidget::SelectSegment()
{
	BackgroundMaterial->SetScalarParameterValue(FName("Index"), CurrentSegmentIndex);
	FWeaponTableRow* WeaponData = GetTableRowForSegment(CurrentSegmentIndex);
	if (WeaponData == nullptr)
	{
		WeaponNameText->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		WeaponNameText->SetVisibility(ESlateVisibility::Visible);
		WeaponNameText->SetText(WeaponData->ItemDescription.Name);
	}
}

FWeaponTableRow* UWeaponWheelWidget::GetTableRowForSegment(int32 SegmentIndex) const
{
	const EEquipmentSlots& SegmentSlot = EquipmentSlotsSegment[SegmentIndex];
	AEquipableItem* EquipableItem = LinkedEquipmentComponent->GetItems()[(int32)SegmentSlot];
	if (!EquipableItem)
	{
		return nullptr;
	}

	return HomeworkDataTableUtils::FindWeaponData(EquipableItem->GetDataTableID());
}

void UWeaponWheelWidget::ConfirmSelection()
{
	EEquipmentSlots SelectedSlot = EquipmentSlotsSegment[CurrentSegmentIndex];
	LinkedEquipmentComponent->EquipItemInSlot(SelectedSlot);
	RemoveFromParent();
}
