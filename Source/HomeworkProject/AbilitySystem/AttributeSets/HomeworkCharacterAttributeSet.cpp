// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AttributeSets/HomeworkCharacterAttributeSet.h"

#include "Net/UnrealNetwork.h"

void UHomeworkCharacterAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
	if (Attribute.AttributeName == FString("Stamina"))
	{
		NewValue = FMath::Clamp(NewValue, 0.f, MaxStamina.GetBaseValue());
	}
	if (Attribute.AttributeName == FString("Health"))
	{
		NewValue = FMath::Clamp(NewValue, 0.f, MaxHealth.GetBaseValue());
	}
}

void UHomeworkCharacterAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	if (Attribute.AttributeName == FString("Stamina"))
	{
		NewValue = FMath::Clamp(NewValue, 0.f, MaxStamina.GetCurrentValue());
	}
	if (Attribute.AttributeName == FString("Health"))
	{
		NewValue = FMath::Clamp(NewValue, 0.f, MaxHealth.GetCurrentValue());
	}
}

void UHomeworkCharacterAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UHomeworkCharacterAttributeSet, Stamina);
	DOREPLIFETIME(UHomeworkCharacterAttributeSet, MaxStamina);
	DOREPLIFETIME(UHomeworkCharacterAttributeSet, Health);
	DOREPLIFETIME(UHomeworkCharacterAttributeSet, MaxHealth);
	DOREPLIFETIME(UHomeworkCharacterAttributeSet, Defence);
}

