// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AttributeProgressBar.generated.h"

class UHomeworkCharacterAttributeSet;
class UProgressBar;
/**
 * 
 */
UCLASS()
class HOMEWORKPROJECT_API UAttributeProgressBar : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetProgressPercentage(float Percentage);

	void SetAttributeSet(UHomeworkCharacterAttributeSet* AttributeSet_In);

	UFUNCTION(BlueprintCallable)
	float GetHealthPercent() const;
protected:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthProgressBar;

	TWeakObjectPtr<UHomeworkCharacterAttributeSet> AttributeSet;
};
