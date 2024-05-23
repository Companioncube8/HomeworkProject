// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerCharacter.h"
#include "FPPlayerCharacter.generated.h"

/**
 *
 */
UCLASS()
class HOMEWORKPROJECT_API AFPPlayerCharacter : public APlayerCharacter
{
	GENERATED_BODY()
public:
	AFPPlayerCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void PossessedBy(AController* NewController) override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	virtual FRotator GetViewRotation() const override;

	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;


	virtual void Falling() override;
	virtual void Landed(const FHitResult& Hit) override;
	virtual void NotifyJumpApex() override;

	UFUNCTION(BlueprintCallable)
	void EndHardLanded();

	UFUNCTION(BlueprintCallable)
	void StartHardLanded();
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | First person")
	class USkeletalMeshComponent* FirstPersonMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | First person")
	class UCameraComponent* FirstPersonCameraComponent;

	virtual void OnMantle(const FMantlingSettings& MantlingSettings, float MantlingAnimationStartTime) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | First person | Camera | Ladder | Pitch", meta = (UIMin = -89.f, UIMax = 89.f))
	float LadderCameraMinPitch = -60.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | First person | Camera | Ladder | Pitch", meta = (UIMin = -89.f, UIMax = 89.f))
	float LadderCameraMaxPitch = 80.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | First person | Camera | Ladder | Yaw", meta = (UIMin = -89.f, UIMax = 89.f))
	float LadderCameraMinYaw = -85.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | First person | Camera | Ladder | Yaw", meta = (UIMin = -89.f, UIMax = 89.f))
	float LadderCameraMaxYaw = 85.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | First person | Camera | Zipline | Pitch", meta = (UIMin = -89.f, UIMax = 89.f))
	float ZiplineCameraMinPitch = -89.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | First person | Camera | Zipline | Pitch", meta = (UIMin = -89.f, UIMax = 89.f))
	float ZiplineCameraMaxPitch = 89.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | First person | Camera | Zipline | Yaw", meta = (UIMin = -89.f, UIMax = 89.f))
	float ZiplineCameraMinYaw = -90.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | First person | Camera | Zipline | Yaw", meta = (UIMin = -89.f, UIMax = 89.f))
	float ZiplineCameraMaxYaw = 90.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | First person | Camera | Zipline | Yaw", meta = (UIMin = -89.f, UIMax = 89.f))
	float HardFallingMinimumHeight = 1000.f;
private:
	FTimerHandle FPMontageTimer;

	void OnFPMontageTimerElapsed();

	TWeakObjectPtr<class ABasePlayerController> PlayerController;
	bool IsFPMontagePlaying() const;
	void SetCameraRestrictions(bool bIsNeedViewRestrictions, float ViewPitchMin, float ViewPitchMax, float ViewYawMin, float ViewYawMax);
	void OnLadderStopped();
	void OnZiplineStarted();
	void OnZiplineStoped();
	void OnLadderStarted();


	float StartFallingZ;
};

