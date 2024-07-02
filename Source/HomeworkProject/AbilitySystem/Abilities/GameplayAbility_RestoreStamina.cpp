// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GameplayAbility_RestoreStamina.h"

#include "AbilitySystemComponent.h"
#include "Characters/BaseCharacter.h"
#include "GameFramework/MovementComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitAttributeChange.h"
#include "AbilitySystem/AttributeSets/HomeworkCharacterAttributeSet.h"
#include "Components/MovemenyComponents/BaseCharacterMovementComponent.h"

bool UGameplayAbility_RestoreStamina::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!ActorInfo->MovementComponent.IsValid() || !ActorInfo->MovementComponent->IsA<UBaseCharacterMovementComponent>())
	{
		return false;
	}
	return  Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UGameplayAbility_RestoreStamina::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	Character = StaticCast<ABaseCharacter*>(ActorInfo->AvatarActor.Get());
	MovementComponent = StaticCast<UBaseCharacterMovementComponent*>(ActorInfo->MovementComponent.Get());

	UAbilityTask_WaitAttributeChange* AttributeChangeTask = UAbilityTask_WaitAttributeChange::WaitForAttributeChange(this, StaminaAttribute, FGameplayTag(), FGameplayTag(), false);
	AttributeChangeTask->OnChange.AddDynamic(this, &UGameplayAbility_RestoreStamina::OnStaminaChanged);
	AttributeChangeTask->Activate();
}

void UGameplayAbility_RestoreStamina::OnStaminaChanged()
{
	const UHomeworkCharacterAttributeSet* AttributeSet = Character->GetCharacterAttributeSet();
	float Stamina = AttributeSet->Stamina.GetCurrentValue();
	float MaxStamina = AttributeSet->MaxStamina.GetCurrentValue();

	if (Stamina == 0.f)
	{
		Character->GetAbilitySystemComponent()->AddLooseGameplayTag(OutOfStaminaTag);
		MovementComponent->SetIsOutOfStamina(true);
	} else if (Stamina == MaxStamina && MovementComponent->IsOutOfStamina())
	{
		MovementComponent->SetIsOutOfStamina(false);
		Character->GetAbilitySystemComponent()->RemoveLooseGameplayTag(OutOfStaminaTag);
	}
}
