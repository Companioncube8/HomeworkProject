// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WidgetCharacterAttributes.generated.h"

/**
 * 
 */

class UCharacterAttributeComponent;

UENUM(BlueprintType)
enum class EWidgetType : uint8
{
	Health,
	Oxygen,
	Stamina
};

UCLASS()
class HOMEWORKPROJECT_API UWidgetCharacterAttributes : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION()
	void UpdatePercent(float NewPercent);
protected:
	virtual void NativeConstruct() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance", meta = (Units = "Percent"))
	float Percent = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance")
	FColor ProgressBarColor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance")
	bool bNeedShowWhenFill = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Type")
	EWidgetType WidgetType = EWidgetType::Health;
};
