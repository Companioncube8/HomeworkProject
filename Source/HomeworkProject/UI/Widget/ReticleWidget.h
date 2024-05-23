// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HomeworkProjectTypes.h"
#include "Blueprint/UserWidget.h"
#include "ReticleWidget.generated.h"

class ABaseCharacter;
class AEquipableItem;
/**
 * 
 */
UCLASS()
class HOMEWORKPROJECT_API UReticleWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	UFUNCTION(BlueprintNativeEvent)
	void OnAimingStateChange(bool bIsAiming);

	UFUNCTION(BlueprintNativeEvent)
	void OnEquippedItemChanged(const AEquipableItem* EquippedItem);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reticle")
	EReticleType CurrentReticle = EReticleType::None;

	virtual void NativeConstruct() override;
private:
	TWeakObjectPtr<const AEquipableItem> CurrentEquippedItem;

	void SetupCurrentReticle();
};
