// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterAttributeComponent.h"

#include "DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"
#include "HomeworkProject/HomeworkProjectTypes.h"
#include "HomeworkProject/Characters/BaseCharacter.h"
#include "HomeworkProject/Subsystems/DebugSubsystem.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UCharacterAttributeComponent::UCharacterAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UCharacterAttributeComponent::BeginPlay()
{
	Super::BeginPlay();

	checkf(MaxHealth > 0.f, TEXT("UCharacterAttributeComponent::BeginPlay() MaxHealth cannot be equal 0"));
	checkf(GetOwner()->IsA<ABaseCharacter>(), TEXT("UCharacterAttributeComponent::BeginPlay() UCharacterAttributeComponent can be used only with ABaseCharacter"));
	CachedBaseCharacterOwner = StaticCast<ABaseCharacter*>(GetOwner());

	CurrentStamina = MaxStamina;
	Health = MaxHealth;
	Oxygen = MaxOxygen;

	CachedBaseCharacterOwner->OnTakeAnyDamage.AddDynamic(this, &UCharacterAttributeComponent::OnTakeAnyDamage);
}


// Called every frame
void UCharacterAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	DebugDrawAttributes();
#endif
}

void UCharacterAttributeComponent::OnTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (!IsAlive())
	{
		return;
	}

	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);
	UE_LOG(LogDamage, Warning, TEXT("UCharacterAttributeComponent::OnTakeAnyDamage %s receive %.2f amount if damage from %s"), *CachedBaseCharacterOwner->GetName(), Damage, *DamageCauser->GetName())

		OnHealthPercentChanged.ExecuteIfBound(Health / MaxHealth);

	if (Health <= 0.f)
	{
		UE_LOG(LogDamage, Warning, TEXT("UCharacterAttributeComponent::OnTakeAnyDamage character %s is killed by an actor %s"), *CachedBaseCharacterOwner->GetName(), *DamageCauser->GetName())
			if (OnDeathEvent.IsBound())
			{
				OnDeathEvent.Broadcast();
			}
	}
}

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
void UCharacterAttributeComponent::DebugDrawAttributes()
{
	UDebugSubsystem* DebugSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UDebugSubsystem>();
	if (!DebugSubsystem->IsCategoryEnabled(DebugCategoryCharacterAttributes))
	{
		return;
	}

	FVector TextLocation = CachedBaseCharacterOwner->GetActorLocation() + (CachedBaseCharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 5.f) * FVector::UpVector;
	DrawDebugString(GetWorld(), TextLocation, FString::Printf(TEXT("Health: %2.f"), Health), nullptr, FColor::Green, 0.f, true);


	TextLocation.Z -= 10;

	DrawDebugString(GetWorld(), TextLocation, FString::Printf(TEXT("Stamina: %2.f"), CurrentStamina), nullptr, FColor::Blue, 0.f, true);

	TextLocation.Z -= 10;
	DrawDebugString(GetWorld(), TextLocation, FString::Printf(TEXT("Oxigen: %2.f"), Oxygen), nullptr, FColor::Cyan, 0.f, true);
}
#endif


void UCharacterAttributeComponent::UpdateStaminaValue(float DeltaTime, bool bIsSprinting)
{
	if (bIsSprinting) {
		CurrentStamina -= SprintStaminaConsumptionVelocity * DeltaTime;
	}
	else
	{
		if (FMath::IsNearlyEqual(MaxStamina, CurrentStamina))
		{
			return;
		}
		CurrentStamina += StaminaRestoreVelocity * DeltaTime;
	}
	CurrentStamina = FMath::Clamp(CurrentStamina, 0.0f, MaxStamina);

	OnStaminaPercentChanged.ExecuteIfBound(CurrentStamina / MaxStamina);

	if (FMath::IsNearlyZero(CurrentStamina)) {
		OnOutOfStamina.Broadcast(true);
	}
	else if (FMath::IsNearlyEqual(MaxStamina, CurrentStamina))
	{
		OnOutOfStamina.Broadcast(false);
	}
}

void UCharacterAttributeComponent::UpdateOxygenValue(float DeltaTime, bool bIsUnderWater)
{
	if (bIsUnderWater) {
		Oxygen -= SwimOxygenConsumptionVelocity * DeltaTime;
	}
	else
	{
		if (FMath::IsNearlyEqual(Oxygen, MaxOxygen))
		{
			return;
		}
		Oxygen += OxygenRestoreVelocity * DeltaTime;
	}
	Oxygen = FMath::Clamp(Oxygen, 0.0f, MaxOxygen);

	OnOxigenPercentChanged.ExecuteIfBound(Oxygen / MaxOxygen);

	OnOutOfOxigen.Broadcast(FMath::IsNearlyZero(Oxygen));
}

float UCharacterAttributeComponent::GetHealthPercent() const
{
	return Health / MaxHealth;
}
