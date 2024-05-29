// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HomeworkProject/Characters/BaseCharacter.h"
#include "BaseCharacterMovementComponent.generated.h"

/**
 * 
 */

struct FMantlingMovementParameters
{
	FVector InitialLocation = FVector::ZeroVector;
	FRotator InitialRotation = FRotator::ZeroRotator;

	FVector TargetLocation = FVector::ZeroVector;
	FRotator TargetRotation = FRotator::ZeroRotator;

	FVector InitialAnimationLocation = FVector::ZeroVector;

	float Duration = 1.0f;
	float StartTime = 0.f;

	UPROPERTY(BlueprintReadOnly)
	UCurveVector* MantlingCurve;

	UPROPERTY(BlueprintReadOnly)
	UPrimitiveComponent* LedgeActor;
	FVector StartActorPosition;
};

UENUM(BlueprintType)
enum class ECustomMovementMode : uint8
{
	None = 0 UMETA(DisplayName = "None"),
	Mantling UMETA(DisplayName = "Mantling"),
	Ladder UMETA(DisplayName = "Ladder"),
	Zipline UMETA(DisplayName = "Zipline"),
	WallRun UMETA(DisplayName = "Wallrun"),
	Slide UMETA(DisplayName = "Slide"),
	Max UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EWallRunSide : uint8
{
	Left,
	Right,
	None
};

UENUM(BlueprintType)
enum class EDetachFromLadderMethod: uint8
{
	Fall = 0,
	ReachingTheTop,
	ReachingTheBottom,
	Jump
};

UCLASS()
class HOMEWORKPROJECT_API UBaseCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

		friend class FSavedMove_GC;

public:
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;

	virtual void BeginPlay() override;
	virtual void PhysicsRotation(float DeltaTime) override;

	TDelegate<void(bool)> OnSprintStart;
	FORCEINLINE bool IsSprinting() { return bIsSprinting; }
	virtual float GetMaxSpeed() const override;

	void StartSprint();
	void StopSprint();

	void StartMantle(const FMantlingMovementParameters& MantlingParameters);
	void EndMantle();
	void EndZipline();
	bool IsMantling() const;

	FORCEINLINE bool IsOutOfStamina() const { return bIsOutOfStamina; }
	void SetIsOutOfStamina(bool bIsOutOfStamina_In);

	bool WantsToProne() const { return bWantsToProne; }
	void SetWantsToProne(bool bWantsToProne_In);

	bool IsFullHeight() const { return bIsFullHeight; }
	void SetIsFullHeighte(bool bIsFullHeighte_In);

	virtual void UpdateCharacterStateAfterMovement(float DeltaSeconds) override;
	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;

	bool IsProning() const;

	void AttachToLadder(const class ALadder* Ladder);
	void DetachFromLadder(EDetachFromLadderMethod DetachFromLadderMethod = EDetachFromLadderMethod::Fall);
	void PhysMantling(float DeltaTime, int32 Iterations);

	void PhysLadder(float DeltaTime, int32 Iterations);

	bool IsOnLadder() const;
	bool IsOnZipline() const;
	float GetActorToCurrentLadderProjection(const FVector& Location) const;
	float GetLadderSpeedRation() const;
	void GetWallRunSideAndDirection(const FVector& HitNormal, EWallRunSide& OutSide, FVector& OutVectorDirection) const;
	const class ALadder* GetCurrentLadder();

	void PhysZipline(float DeltaTime, int32 Iterations);
	void AttachToZipline(const class AZipline* Zipline);
	void DetachFromZipline();

	void PhysWallRun(float DeltaTime, int32 Iterations);
	void PhysSliding(float DeltaTime, int32 Iterations);
	bool IsWallRun() const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement: prone", meta = (ClampMin = 0.f, UIMin = 0.f))
	float ProneCapsuleHalfHeight = 40.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement: prone", meta = (ClampMin = 0.f, UIMin = 0.f))
	float ProneCapsuleRadius = 40.f;

	void StopWallRun();
	EWallRunSide GetWallRunSide() { return CurrentWallRunSide; }
	FVector GetWallRunDirection() { return CurrentWallRunDirection; }

	void StartSlide();
	void StopSlide();

	bool IsSliding() const;

	void SetIsHardFalling(bool bIsHardFalling_In);
	bool IsHardFalling() const;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement: sprint", meta = (ClampMin = 0.f, UIMin = 0.f))
	float SprintSpeed = 1200.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement: stamina", meta = (ClampMin = 0.f, UIMin = 0.f))
	float OutOfStaminaSpeed = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement: prone", meta = (ClampMin = 0.f, UIMin = 0.f))
	float MaxProneSpeed = 100.0f;
	
	UPROPERTY(Category="Character Movement: Swimming", EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="0"))
	float SwimmingCapsuleRadius = 60.f;

	UPROPERTY(Category="Character Movement: Swimming", EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="0"))
	float SwimmingCapsuleHalfHeight = 60.;

	UPROPERTY(Category = "Character Movement: Ladder", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float ClimbingOnLadderMaxSpeed = 200.f;

	UPROPERTY(Category = "Character Movement: Ladder", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float ClimbingOnLadderBrakingDeceleration = 2048.f;

	UPROPERTY(Category = "Character Movement: Ladder", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float LadderToCharacterOffset = 60.f;

	UPROPERTY(Category = "Character Movement: Ladder", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float MaxLadderTopOffset = 90.f;

	UPROPERTY(Category = "Character Movement: Ladder", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float MinLadderBottomOffset = 90.f;

	UPROPERTY(Category = "Character Movement: Ladder", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float JumpOffFromLadderSpeed = 500.f;

	UPROPERTY(Category = "Character Movement: Ladder", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float ZiplineMoveSpeed = 400.f;

	UPROPERTY(Category = "Character Movement: Ladder", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float OffsetFromTopZipline = 30.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character: wall run")
	float MaxWallRunTime = 2.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character: wall run")
	float MaxWallRunSpeed = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character: wall run")
	UCurveFloat* WallRunCurve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character: wall run")
	float MaxWallRunZOffset = 30.f;

	UPROPERTY(Category = "Character Movement: Slide", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float SlideSpeed = 1000.0f;

	UPROPERTY(Category = "Character Movement: Slide", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float SlideCaspsuleHalfHeight = 60.0f;

	UPROPERTY(Category = "Character Movement: Slide", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float SlideMaxTime = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Slide")
	UAnimMontage* SlideAnimMontage;

	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;

	virtual void PhysCustom(float DeltaTime, int32 Iterations) override;

	ABaseCharacter* GetBaseCharacterOwner() const;
private:
	bool bIsSprinting;
	bool bIsOutOfStamina = false;

	float DefaultCapsuleHalfHeight = 0.f;
	float DefaultCapsuleRadius = 0.f;

	bool bWantsToProne = false;
	bool bIsFullHeight = false;

	bool CanProneInCurrentState() const;
	virtual void Prone();
	bool CanUncrouch(float CurrentPronedHalfHeight, float ScaledHalfHeightAdjust, FVector PawnLocation);
	virtual void UnProne();

	FMantlingMovementParameters CurrentMantlingParameters;

	FTimerHandle MantlingTimer;
	FTimerHandle ZiplineTimer;

	const ALadder* CurrentLadder = nullptr;

	FRotator ForceTargetRotation = FRotator::ZeroRotator;
	bool bForceRotation = false;

	FRotator ZiplineMoveOrientation = FRotator::ZeroRotator;

	UFUNCTION()
	void OnPlayerCapsuleHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	bool IsSurfaceWallRunnable(const FVector& SurfaceNormal) const;

	bool AreRequiredKeysDown(EWallRunSide Side) const;

	void StartWallRun(EWallRunSide Side, FVector& Direction);
	void UpdateWallRun();

	EWallRunSide CurrentWallRunSide = EWallRunSide::None;
	EWallRunSide PreviousWallRunSide = EWallRunSide::None;

	FVector CurrentWallRunDirection = FVector::ZeroVector;

	float StartWallRunZPosition = 0.f;

	FTimerHandle WallRunTimer;
	FTimerHandle SlidingTimer;

	UPROPERTY(VisibleAnywhere, Category = "Character Movement: Zipline")
	const AZipline* CurrentZipline = nullptr;

	bool bIsHardFalling = false;
};

class FSavedMove_GC: public FSavedMove_Character
{
	typedef FSavedMove_Character Super;
public:
	virtual void Clear() override;

	virtual uint8 GetCompressedFlags() const override;

	virtual bool CanCombineWith(const FSavedMovePtr& NewMovePtr, ACharacter* InCharacter, float MaxDelta) const override;

	virtual void SetMoveFor(ACharacter* InCharacter, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData) override;

	virtual void PrepMoveFor(ACharacter* Character) override;
private:
	uint8 bSavedIsSprinting : 1;

};

class FNetworkPredictionData_Client_Character_GC : public  FNetworkPredictionData_Client_Character
{
	typedef FNetworkPredictionData_Client_Character Super;

public:
	FNetworkPredictionData_Client_Character_GC(const UCharacterMovementComponent& CharacterMovement);

	virtual FSavedMovePtr AllocateNewMove() override;
};