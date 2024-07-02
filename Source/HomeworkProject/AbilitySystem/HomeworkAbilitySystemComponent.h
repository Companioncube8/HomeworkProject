// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "HomeworkAbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class HOMEWORKPROJECT_API UHomeworkAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	bool TryActivateAbilityWithTag(FGameplayTag GameplayTag, bool bIsAllowRemoteActivation = true);

	bool TryCancelAbilityWithTag(FGameplayTag GameplayTag);

	bool IsAbilityActive(FGameplayTag GameplayTag) const;
};
