// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HomeworkProjectTypes.h"
#include "Blueprint/UserWidget.h"
#include "WeaponWheelWidget.generated.h"

class UCharacterEquipmentComponent;
struct FWeaponTableRow;
class UImage;
class UTextBlock;
/**
 * 
 */
UCLASS()
class HOMEWORKPROJECT_API UWeaponWheelWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitializeWeaponWheelWidget(UCharacterEquipmentComponent* EquipmentComponent);

	void NextSegment();
	void PreviousSegment();
	void ConfirmSelection();

protected:
	virtual void NativeConstruct() override;
	void SelectSegment();

	UPROPERTY(meta = (BindWidget))
	UImage* RadialBackground;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* WeaponNameText;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon wheel settings")
	TArray<EEquipmentSlots> EquipmentSlotsSegment;

private:
	FWeaponTableRow* GetTableRowForSegment(int32 SegmentIndex) const;

	int32 CurrentSegmentIndex;
	UMaterialInstanceDynamic* BackgroundMaterial;
	TWeakObjectPtr<UCharacterEquipmentComponent> LinkedEquipmentComponent;
};
