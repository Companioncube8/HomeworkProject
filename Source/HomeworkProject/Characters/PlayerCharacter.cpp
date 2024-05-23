// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CharacterComponents/CharacterEquipmentComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "HomeworkProject/Components/MovemenyComponents/BaseCharacterMovementComponent.h"

APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring arm"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->bUsePawnControlRotation = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->bUsePawnControlRotation = false;

	Timeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("Timeline"));

	FOVTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("FOVTimeline"));

	GetCharacterMovement()->bOrientRotationToMovement = 1;
	GetCharacterMovement()->RotationRate = FRotator(0, 540, 0);

	DefaultArmLength = SpringArmComponent->TargetArmLength;

	Team = ETeams::Player;
}

void APlayerCharacter::BeginPlay() {
	Super::BeginPlay();
	if (FloatCurve)
	{
		FOnTimelineFloat ProgressFunction;
		ProgressFunction.BindUFunction(this, FName("TimelineProgress"));

		Timeline->AddInterpFloat(FloatCurve, ProgressFunction);
	}

	if (FOVTimelineCurve)
	{
		FOnTimelineFloat FOVTimelineUpdate;
		FOVTimelineUpdate.BindDynamic(this, &APlayerCharacter::FOVTimelineProgress);

		FOVTimeline->AddInterpFloat(FOVTimelineCurve, FOVTimelineUpdate);
	}
}
void APlayerCharacter::MoveForward(float Value)
{
	if (BaseCharacterMovementComponent->IsSliding())
	{
		return;
	}
	ForwardAxis = Value;
	if ((GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling() || BaseCharacterMovementComponent->IsWallRun()) && !FMath::IsNearlyZero(Value, 1e-6f))
	{
		FRotator YawRotator(0.0f, GetControlRotation().Yaw, 0.0f);
		FVector ForwardVector = YawRotator.RotateVector(FVector::ForwardVector);
		AddMovementInput(ForwardVector, Value);
	}
}

void APlayerCharacter::MoveRight(float Value)
{
	if (BaseCharacterMovementComponent->IsSliding())
	{
		return;
	}
	RightAxis = Value;
	if ((GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling() || BaseCharacterMovementComponent->IsWallRun()) && !FMath::IsNearlyZero(Value, 1e-6f))
	{
		FRotator YawRotator(0.0f, GetControlRotation().Yaw, 0.0f);
		FVector RightVector = YawRotator.RotateVector(FVector::RightVector);
		AddMovementInput(RightVector, Value);
	}
}

void APlayerCharacter::Turn(float Value)
{
	AddControllerYawInput(Value * GetAimTurnModifier());
}

void APlayerCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value * GetAimLookUpModifier());
}

float APlayerCharacter::GetAimTurnModifier()
{
	float Result = 1.f;
	if (IsAiming()) {
		if (ARangeWeaponItem* RangeWeapon = GetCharacterEquipmentComponent()->GetCurrentRangeWeapon())
		{
			Result = RangeWeapon->GetAimTurnModifier();
		}
	}
	return Result;
}

float APlayerCharacter::GetAimLookUpModifier()
{
	float Result = 1.f;
	if (IsAiming()) {
		if (ARangeWeaponItem* RangeWeapon = GetCharacterEquipmentComponent()->GetCurrentRangeWeapon())
		{
			Result = RangeWeapon->GetAimLookUpModifier();
		}
	}
	return Result;
}

void APlayerCharacter::TurnAtRate(float Value)
{
	AddControllerYawInput(Value * BaseTurnRate * GetWorld()->GetDeltaSeconds() * GetAimTurnModifier());
}

void APlayerCharacter::LookUpAtRate(float Value)
{
	AddControllerPitchInput(Value * BaseLookUpRate * GetWorld()->GetDeltaSeconds() * GetAimLookUpModifier());
}

void APlayerCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	UpdateCameraOffset(HalfHeightAdjust, true);
}

void APlayerCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	UpdateCameraOffset(HalfHeightAdjust, false);
}

void APlayerCharacter::OnStartProne_Implementation(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartProne_Implementation(HalfHeightAdjust, ScaledHalfHeightAdjust);
	UpdateCameraOffset(HalfHeightAdjust, true);
}

void APlayerCharacter::OnEndProne_Implementation(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndProne_Implementation(HalfHeightAdjust, ScaledHalfHeightAdjust);
	UpdateCameraOffset(HalfHeightAdjust, false);
}

void APlayerCharacter::UpdateCameraOffset(float Offset, bool bIsIncreased)
{
	if (bIsIncreased)
	{
		SpringArmComponent->TargetOffset += FVector(0, 0, Offset);
	}
	else
	{
		SpringArmComponent->TargetOffset -= FVector(0, 0, Offset);
	}
}

void APlayerCharacter::OnJumped_Implementation()
{
	if (bIsCrouched)
	{
		UnCrouch();
	}
}

bool APlayerCharacter::CanJumpInternal_Implementation() const
{
	return (bIsCrouched && !GetBaseCharacterMovementComponent()->IsMantling() && !GetBaseCharacterMovementComponent()->IsOutOfStamina()) || Super::CanJumpInternal_Implementation();
}

void APlayerCharacter::OnSprintEnd_Implementation()
{
	Timeline->Reverse();
}

void APlayerCharacter::OnSprintStart_Implementation()
{
	Timeline->PlayFromStart();
}

void APlayerCharacter::TimelineProgress(float Value)
{
	float NewValue = FMath::Lerp(DefaultArmLength, SprintArmLength, Value);
	SpringArmComponent->TargetArmLength = NewValue;
}

void APlayerCharacter::FOVTimelineProgress(const float Alpha)
{
	APlayerController* PlayerController = GetController<APlayerController>();
	APlayerCameraManager * CameraManager = PlayerController->PlayerCameraManager;
	if (IsValid(CameraManager))
	{
		if (ARangeWeaponItem* RangeWeapon = GetCharacterEquipmentComponent()->GetCurrentRangeWeapon())
		{
			float NewFOVValue = FMath::Lerp(CameraManager->DefaultFOV, RangeWeapon->GetAimFOV(), Alpha);
			CameraManager->SetFOV(NewFOVValue);
		}
	}
}

void APlayerCharacter::SwimForward(float Value)
{
	if (GetCharacterMovement()->IsSwimming() && !FMath::IsNearlyZero(Value, 1e-6f))
	{
		FRotator PitchYawRotator(GetControlRotation().Pitch, GetControlRotation().Yaw, 0.0f);
		FVector ForwardVector = PitchYawRotator.RotateVector(FVector::ForwardVector);
		AddMovementInput(ForwardVector, Value);
	}
}

void APlayerCharacter::SwimRight(float Value)
{
	if (GetCharacterMovement()->IsSwimming() && !FMath::IsNearlyZero(Value, 1e-6f))
	{
		FRotator YawRotator(0.0f, GetControlRotation().Yaw, 0.0f);
		FVector RightVector = YawRotator.RotateVector(FVector::RightVector);
		AddMovementInput(RightVector, Value);
	}
}

void APlayerCharacter::SwimUp(float Value)
{
	if (GetCharacterMovement()->IsSwimming() && !FMath::IsNearlyZero(Value, 1e-6f))
	{
		AddMovementInput(FVector::UpVector, Value);
	}
}

void APlayerCharacter::OnStopAimingInternal()
{
	Super::OnStopAimingInternal();
	APlayerController* PlayerController = GetController<APlayerController>();
	if (!IsValid(PlayerController))
	{
		return;
	}
	if (FOVTimelineCurve)
	{
		FOVTimeline->Reverse();
	} else if (APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager)
	{
		ARangeWeaponItem* RangeWeapon = GetCharacterEquipmentComponent()->GetCurrentRangeWeapon();
		CameraManager->UnlockFOV();
	}
}

void APlayerCharacter::OnStartAimingInternal()
{
	Super::OnStartAimingInternal();
	APlayerController* PlayerController = GetController<APlayerController>();
	if (!IsValid(PlayerController))
	{
		return;
	}

	if (FOVTimelineCurve)
	{
		FOVTimeline->Play();
	} else if (APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager)
	{
		ARangeWeaponItem* RangeWeapon = GetCharacterEquipmentComponent()->GetCurrentRangeWeapon();
		CameraManager->SetFOV(RangeWeapon->GetAimFOV());
	}
}

