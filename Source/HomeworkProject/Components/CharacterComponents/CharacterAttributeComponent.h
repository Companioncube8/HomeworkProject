// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterAttributeComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnOutOfStamina, bool)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnOutOfOxigen, bool)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHealthChanged, float)

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HOMEWORKPROJECT_API UCharacterAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCharacterAttributeComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	TMulticastDelegate<void()> OnDeathEvent;
	FOnOutOfStamina OnOutOfStamina;
	FOnOutOfOxigen OnOutOfOxigen;
	FOnHealthChanged OnHealthChangedEvent;

	TDelegate<void(float)>OnOxigenPercentChanged;
	TDelegate<void(float)>OnStaminaPercentChanged;

	bool IsAlive() { return  Health > 0.f; }

	void UpdateStaminaValue(float DeltaTime, bool bIsSprinting);

	void UpdateOxygenValue(float DeltaTime, bool bIsUnderWater);

	bool IsOutOfOxigen() { return FMath::IsNearlyZero(Oxygen); }

	float GetHealthPercent() const;

	void AddHealth(float HealthToAdd);

	void RestoreFullStamina();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health", meta = (UIMin = 0.f))
	float MaxHealth = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Stamina")
	float MaxStamina = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Stamina")
	float StaminaRestoreVelocity = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Stamina")
	float SprintStaminaConsumptionVelocity = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Oxigen", meta = (UIMin = 0.f))
	float MaxOxygen = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Oxigen", meta = (UIMin = 0.f))
	float OxygenRestoreVelocity = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Oxigen", meta = (UIMin = 0.f))
	float SwimOxygenConsumptionVelocity = 2.0f;

private:
	UPROPERTY(ReplicatedUsing=OnRep_Health)
	float Health = 0.f;

	UFUNCTION()
	void OnRep_Health();

	void OnHealthChanged();

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	void DebugDrawAttributes();
#endif

	UFUNCTION()
	void OnTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	TWeakObjectPtr<class ABaseCharacter> CachedBaseCharacterOwner;

	float CurrentStamina = 0.f;
	bool CanSprint = true;

	float Oxygen = 0.f;
};
