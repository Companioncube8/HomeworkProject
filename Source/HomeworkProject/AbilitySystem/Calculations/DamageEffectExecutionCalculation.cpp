// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Calculations/DamageEffectExecutionCalculation.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/AttributeSets/HomeworkCharacterAttributeSet.h"

struct GCDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Health);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Defence);

	GCDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UHomeworkCharacterAttributeSet, Health, Target, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UHomeworkCharacterAttributeSet, Defence, Target, true);
	}
};

static const GCDamageStatics& DamageStatics()
{
	static GCDamageStatics Statics;
	return Statics;
}

UDamageEffectExecutionCalculation::UDamageEffectExecutionCalculation()
{
	RelevantAttributesToCapture.Add(DamageStatics().HealthDef);
	RelevantAttributesToCapture.Add(DamageStatics().DefenceDef);
}

void UDamageEffectExecutionCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	//UAbilitySystemComponent* TargetAbilitySystemComponent = ExecutionParams.GetTargetAbilitySystemComponent();
	//UAbilitySystemComponent* SourceAbilitySystemComponent = ExecutionParams.GetSourceAbilitySystemComponent();

	//AActor* SourceActor = SourceAbilitySystemComponent ? SourceAbilitySystemComponent->GetAvatarActor() : nullptr;
	//AActor* TargetActor = TargetAbilitySystemComponent ? TargetAbilitySystemComponent->GetAvatarActor() : nullptr;

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	FGameplayTagContainer AssetTags;
	Spec.GetAllAssetTags(AssetTags);

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float Defence = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DefenceDef, EvaluateParameters, Defence);

	float Damage = Spec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Abilities.Attributes.Health")), false, 0.f);

	float TotalDamage = FMath::Max<float>(0.f, Damage - Defence);

	if (TotalDamage > 0.f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageStatics().HealthProperty, EGameplayModOp::Additive, -TotalDamage));
	}
}

