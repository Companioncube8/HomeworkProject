// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GameplayAbility_Life.h"

#include "Abilities/Tasks/AbilityTask_WaitAttributeChange.h"
#include "Characters/BaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/MovementComponent.h"

bool UGameplayAbility_Life::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!ActorInfo->AvatarActor->IsA<ABaseCharacter>())
	{
		return false;
	}

	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UGameplayAbility_Life::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAbilityTask_WaitAttributeChange* OnHealthChangedTask = UAbilityTask_WaitAttributeChange::WaitForAttributeChangeWithComparison(this, HealthAttribute, FGameplayTag(), FGameplayTag(), EWaitAttributeChangeComparison::ExactlyEqualTo, 0.f);
	OnHealthChangedTask->OnChange.AddDynamic(this, &UGameplayAbility_Life::OnHealthElapsed);
	OnHealthChangedTask->Activate();
}

void UGameplayAbility_Life::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	UCharacterMovementComponent* CharacterMovement = Cast<UCharacterMovementComponent>(ActorInfo->MovementComponent.Get());
	if (CharacterMovement)
	{
		CharacterMovement->DisableMovement();
	}

	float Duration = 0.f;
	if (ActorInfo->AnimInstance.IsValid())
	{
		Duration = ActorInfo->AnimInstance->Montage_Play(OnDeathAnimMontage, 1.f, EMontagePlayReturnType::Duration);
	}

	if (Duration == 0.f && ActorInfo->SkeletalMeshComponent.IsValid())
	{
		ActorInfo->SkeletalMeshComponent->SetCollisionProfileName(CollisionProfileRagdoll);
		ActorInfo->SkeletalMeshComponent->SetSimulatePhysics(true);
	}

	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGameplayAbility_Life::OnHealthElapsed()
{
	K2_CancelAbility();
}



