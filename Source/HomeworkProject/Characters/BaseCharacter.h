// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "HomeworkProjectTypes.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

class UBaseCharacterMovementComponent;

USTRUCT(BlueprintType)
struct FMantlingSettings
{
	GENERATED_BODY();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UAnimMontage* MantlingMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UAnimMontage* FPMantlingMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UCurveVector* MantlingCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin = 0.f, UIMin = 0.f))
	float MaxHeight = 200.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.f, UIMin = 0.f))
	float MinHeight = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.f, UIMin = 0.f))
	float MaxHeightStartTime = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.f, UIMin = 0.f))
	float MinHeightStartTime = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.f, UIMin = 0.f))
	float AnimationCorrectionXY = 65.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.f, UIMin = 0.f))
	float AnimationCorrectionZ = 200.f;

};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnAimingStateChanged, bool)

class AInteractiveActor;
class UCharacterEquipmentComponent;
class UCharacterAttributeComponent;

UCLASS(Abstract, NotBlueprintable)
class HOMEWORKPROJECT_API ABaseCharacter : public ACharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void PossessedBy(AController* NewController) override;

	virtual void MoveForward(float Value) {};
	virtual void MoveRight(float Value) {};
	virtual void LookUp(float Value) {};
	virtual void Turn(float Value) {};
	virtual void LookUpAtRate(float Value) {};
	virtual void TurnAtRate(float Value) {};
	virtual bool CanJumpInternal_Implementation() const override;
	virtual void Jump() override;

	virtual void SwimForward(float Value) {};
	virtual void SwimRight(float Value) {};
	virtual void SwimUp(float Value) {};

	virtual void ChangeCrouchState();
	virtual void ChangeProneState();

	virtual void StartSprint();
	virtual void StopSprint();

	UFUNCTION(BlueprintCallable)
	void Mantle(bool bForce = false);

	virtual void Slide();
	virtual void AlternativeFire();

	virtual void PrimaryMeleeAttack();
	virtual void SecondaryMeleeAttack();

	virtual void Tick(float DeltaSeconds) override;

	void PlayerStartFire();
	void PlayerStopFire();

	void StartAiming();
	void StopAiming();

	void NextItem();
	void PreviousItem();

	void EquipPrimaryItem();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Character")
	void OnStartAiming();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Character")
	void OnStopAiming();

	float GetAimingMovementSpeed() const;

	bool IsAiming() const;

	FOnAimingStateChanged OnAimingStateChanged;

	FORCEINLINE UBaseCharacterMovementComponent* GetBaseCharacterMovementComponent() const { return  BaseCharacterMovementComponent; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetIKLeftFootOffset() const { return IKLeftFootOffset; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetIKRightFootOffset() const { return IKRightFootOffset; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetIKHipOffset() const { return IKHipOffset; }

	FORCEINLINE bool IsProne() const { return bIsProne; }
	void SetIsProne(bool bIsProne_In);


	UFUNCTION(BlueprintNativeEvent, Category = "Character | Movement")
	void OnEndProne(float HalfHeightAdjust, float ScaledHalfHeightAdjust);
	virtual void OnEndProne_Implementation(float HalfHeightAdjust, float ScaledHalfHeightAdjust);
	void ChangeMeshOffset(float Offset);

	UFUNCTION(BlueprintNativeEvent, Category = "Character | Movement")
	void OnStartProne(float HalfHeightAdjust, float ScaledHalfHeightAdjust);
	virtual void OnStartProne_Implementation(float HalfHeightAdjust, float ScaledHalfHeightAdjust);

	void UnProne(bool bIsFullHeight);
	void Prone();

	void RegisterInteractiveActor(AInteractiveActor* InteractiveActor);
	void UnregisterInteractiveActor(AInteractiveActor* InteractiveActor);

	void ClimbLadderUp(float Value);
	void InteractWithLadder();
	void InteractWithZipline();

	const class ALadder* GetAvailableLadder();

	const class AZipline* GetAvailableZipline();
	UFUNCTION()
	float GetForwardAxisValue() const { return ForwardAxis; }

	UFUNCTION()
	float GetRightAxisValue() const { return RightAxis; }

	const UCharacterEquipmentComponent* GetCharacterEquipmentComponent() const;

	UCharacterEquipmentComponent* GetCharacterEquipmentComponent_Mutable() const;

	const UCharacterAttributeComponent* GetCharacterAttributeComponent() const;

	UCharacterAttributeComponent* GetCharacterAttributeComponent_Mutable() const;

	void Reload();

	/** IGenericTeamAgentInterface**/

	virtual FGenericTeamId GetGenericTeamId() const override;

	/** ~IGenericTeamAgentInterface**/
protected:
	UFUNCTION(BlueprintNativeEvent, Category = "Character | Movement")
	void OnSprintStart();
	virtual void OnSprintStart_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "Character | Movement")
	void OnSprintEnd();
	virtual void OnSprintEnd_Implementation();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Controls")
	float BaseTurnRate = 45.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Controls")
	float BaseLookUpRate = 45.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Controls")
	float SprintSpeed = 800.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Stamina")
	float MaxStamina = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Stamina")
	float StaminaRestoreVelocity = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Stamina")
	float SprintStaminaConsumptionVelocity = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Zipline")
	float MaxDistanceBetweenZiplineTopAndPawn = 200.f;
	virtual bool CanSprint();

	bool CanMantle() const;

	virtual void OnMantle(const FMantlingSettings& MantlingSettings, float MantlingAnimationStartTime);

	UBaseCharacterMovementComponent* BaseCharacterMovementComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | IK settings")
	FName LeftFootSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | IK settings")
	FName RightFootSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | IK settings", meta = (ClampMin = 0.f, UIMin = 0.f))
	float IKTraceExtendDistance = 30;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Character | IK settings", meta = (ClampMin = 0.f, UIMin = 0.f))
	float IKInterpSpeed = 30;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Movement")
	class ULedgeDetectorComponent* LedgeDetectorComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Movement | Mantling")
	FMantlingSettings HighMantlingSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Movement | Mantling")
	FMantlingSettings LowMantlingSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Movement | Mantling", meta = (ClampMin = 0.f, UIMin = 0.f))
	float LowMantleMaxHeight = 125.f;

	UPROPERTY()
	float ForwardAxis = 0;

	UPROPERTY()
	float RightAxis = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Components")
	UCharacterAttributeComponent* CharacterAttributesComponent;

	virtual void OnDeath();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Animations")
	class UAnimMontage* OnDeathAnimMontage;

	// Damage depending from fall height (in meters)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Attributes")
	class UCurveFloat* FallDamageCurve;

	virtual void OnOutOfStamina(bool OutOfStamina);

	virtual void Falling() override;
	virtual void Landed(const FHitResult& Hit) override;
	virtual void NotifyJumpApex() override;

	UPROPERTY(Category = "Character Movement: Swimming", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float OutOfOxygenDamageValue = 5.0f;

	UPROPERTY(Category = "Character Movement: Swimming", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float OutOfOxygenDamageInterval = 2.0f;

	virtual void OnOutOfOxigen(bool OutOfOxigen);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Components")
	class UCharacterEquipmentComponent* CharacterEquipmentComponent;

	virtual void OnStartAimingInternal();
	virtual void OnStopAimingInternal();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | Team")
	ETeams Team = ETeams::Enemy;
private:

	FVector GetIKOutHitLocationForASocket(const FName& SocketName);

	bool bIsSprintRequested = false;

	void TryChangeSprintState(float DeltaTime);

	float IKRightFootOffset = 0.f;
	float IKLeftFootOffset = 0.f;
	float IKTraceDistance = 0.f;
	float IKDisplacement = 0.f;
	float IKHipOffset = 0.f;

	float CurrentStamina;

	void OnSprintChanged(bool isSprint);

	bool CanProne() const;

	bool bIsProne = false;

	const FMantlingSettings& GetMantlingSettings(float LedgeHeight) const;

	TArray<AInteractiveActor*> AvailibleInteractiveActors;

	void EnableRagdoll();
	FVector CurrentFallApex;

	bool IsSwimmingUnderWater() const;

	void SwimmingUnderWaterTakeDamage();

	FTimerHandle OutOfOxigenDamageTimer;

	bool bIsAiming = false;

	float CurrentAimingMovementSpeed = 0.f;
};
