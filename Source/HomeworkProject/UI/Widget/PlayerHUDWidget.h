// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AmmoWidget.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUDWidget.generated.h"

class UReticleWidget;
class UWidgetCharacterAttributes;

UCLASS()
class HOMEWORKPROJECT_API UPlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UAmmoWidget* GetAmmoWidget();
	UReticleWidget* GetReticalWidget();
	UWidgetCharacterAttributes* GetHealthWidget();
	UWidgetCharacterAttributes* GetStaminaWidget();
	UWidgetCharacterAttributes* GetOxigenWidget();

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget names")
	FName ReticleWidgetName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget names")
	FName AmmoWidgetName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget names")
	FName HealthWidgetName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget names")
	FName StaminaWidgetName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget names")
	FName OxigenWidgetName;

};
