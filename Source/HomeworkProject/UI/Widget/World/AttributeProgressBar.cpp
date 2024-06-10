// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/World/AttributeProgressBar.h"

#include "Components/ProgressBar.h"

void UAttributeProgressBar::SetProgressPercentage(float Percentage)
{
	HealthProgressBar->SetPercent(Percentage);
}
