// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "HomeworkCharacterAttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class HOMEWORKPROJECT_API UHomeworkCharacterAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	float GetStaminaPercent() const { return Stamina.GetCurrentValue() / MaxStamina.GetCurrentValue(); }

	float GetHealthPercent() const { return Health.GetCurrentValue() / MaxHealth.GetCurrentValue(); }

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, ReplicatedUsing=OnRep_Stamina, Category = "Attributes | Stamina")
	FGameplayAttributeData Stamina = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, ReplicatedUsing = OnRep_MaxStamina, Category = "Attributes | Stamina")
	FGameplayAttributeData MaxStamina = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Attributes | Stamina")
	FGameplayAttributeData Health = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth,Category = "Attributes | Stamina")
	FGameplayAttributeData MaxHealth = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, ReplicatedUsing = OnRep_Defence, Category = "Attributes | Stamina")
	FGameplayAttributeData Defence = 5.0f;

	UFUNCTION()
	void OnRep_Stamina(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxStamina(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Defence(const FGameplayAttributeData& OldValue);
};
