// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/World/AttributeProgressBar.h"

#include "AbilitySystem/AttributeSets/HomeworkCharacterAttributeSet.h"
#include "Components/ProgressBar.h"

void UAttributeProgressBar::SetProgressPercentage(float Percentage)
{
	HealthProgressBar->SetPercent(Percentage);
}

void UAttributeProgressBar::SetAttributeSet(UHomeworkCharacterAttributeSet* AttributeSet_In)
{
	AttributeSet = AttributeSet_In;
}

float UAttributeProgressBar::GetHealthPercent() const
{
	float Result = 0.f;
	if (AttributeSet.IsValid())
	{
		Result = AttributeSet->GetHealthPercent();
	}
	return Result;
}
