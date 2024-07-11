// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HomeworkProjectTypes.h"
#include "Blueprint/UserWidget.h"
#include "ReticleWidget.generated.h"

class ABaseCharacter;
class AEquipableItem;
class UProgressBar;
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
	void OnStartStringTension(float TenseTime);

	UFUNCTION(BlueprintNativeEvent)
	void OnEndStringTension();

	UFUNCTION(BlueprintNativeEvent)
	void OnReticalShake(FVector Offset);

	UFUNCTION()
	void OnUpdateStringTensionPercent(float Percent);

	UFUNCTION(BlueprintNativeEvent)
	void OnEquippedItemChanged(const AEquipableItem* EquippedItem);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reticle")
	EReticleType CurrentReticle = EReticleType::None;

	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* StringTenseProgressBar;

private:
	TWeakObjectPtr<const AEquipableItem> CurrentEquippedItem;

	void SetupCurrentReticle();
};
