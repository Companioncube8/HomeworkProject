// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "Components/TimelineComponent.h"
#include "PlayerCharacter.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class HOMEWORKPROJECT_API APlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void MoveForward(float Value) override;
	virtual void MoveRight(float Value) override;
	virtual void Turn(float Value) override;
	virtual void LookUp(float Value) override;
	virtual void TurnAtRate(float Value) override;
	virtual void LookUpAtRate(float Value) override;

	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual bool CanJumpInternal_Implementation() const override;
	virtual void OnJumped_Implementation() override;

	virtual void OnSprintEnd_Implementation() override;
	virtual void OnSprintStart_Implementation() override;
	virtual void BeginPlay() override;

	virtual void OnStartProne_Implementation(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndProne_Implementation(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;


	virtual void SwimForward(float Value) override;
	virtual void SwimRight(float Value) override;
	virtual void SwimUp(float Value) override;

	void UpdateCameraOffset(float Offset, bool bIsIncreased);


protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Camera")
	class UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Camera")
	class USpringArmComponent* SpringArmComponent;

	UTimelineComponent* Timeline;

	UPROPERTY(EditAnywhere, Category = "Sprint | Timeline")
	UCurveFloat* FloatCurve;

	UFUNCTION()
	void TimelineProgress(float Value);

	float DefaultArmLength = 0;

	UPROPERTY(EditAnywhere, Category = "Sprint | Timeline")
	float SprintArmLength = 500;

	virtual void OnStartAimingInternal() override;
	virtual void OnStopAimingInternal() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | FOV")
	UTimelineComponent* FOVTimeline;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | FOV")
	UCurveFloat* FOVTimelineCurve;

	UFUNCTION()
	void FOVTimelineProgress(float Value);
private:
	float GetAimTurnModifier();
	float GetAimLookUpModifier();
};
