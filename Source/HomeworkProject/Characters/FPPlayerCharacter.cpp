// Fill out your copyright notice in the Description page of Project Settings.


#include "FPPlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "HomeworkProject/HomeworkProjectTypes.h"
#include "HomeworkProject/Components/MovemenyComponents/BaseCharacterMovementComponent.h"
#include "PlayersControllers/BasePlayerController.h"


AFPPlayerCharacter::AFPPlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FirstPersonMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	FirstPersonMeshComponent->SetupAttachment(RootComponent);
	FirstPersonMeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -86.0f));
	FirstPersonMeshComponent->CastShadow = false;
	FirstPersonMeshComponent->bCastDynamicShadow = false;
	FirstPersonMeshComponent->SetOnlyOwnerSee(true);

	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FIrstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(FirstPersonMeshComponent, SocketFPCamera);
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->bCastHiddenShadow = true;

	CameraComponent->bAutoActivate = false;

	SpringArmComponent->bAutoActivate = false;
	SpringArmComponent->bUsePawnControlRotation = false;

	GetCharacterMovement()->bOrientRotationToMovement = false;

	bUseControllerRotationYaw = true;
}

void AFPPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	PlayerController = Cast<ABasePlayerController>(NewController);
}

void AFPPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (IsFPMontagePlaying() && PlayerController.IsValid())
	{
		FRotator TargetControlRotation = PlayerController->GetControlRotation();
		TargetControlRotation.Pitch = 0.0f;
		float BlendSpeed = 300.0f;
		TargetControlRotation = FMath::RInterpTo(PlayerController->GetControlRotation(), TargetControlRotation, DeltaTime, BlendSpeed);
		PlayerController->SetControlRotation(TargetControlRotation);
	}
}

void AFPPlayerCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	const AFPPlayerCharacter* DefaultCharacter = GetDefault<AFPPlayerCharacter>(GetClass());
	FVector& FirstPersonMeshRelativeLocation = FirstPersonMeshComponent->GetRelativeLocation_DirectMutable();
	FirstPersonMeshRelativeLocation.Z = DefaultCharacter->FirstPersonMeshComponent->GetRelativeLocation().Z + HalfHeightAdjust;
}

void AFPPlayerCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	const AFPPlayerCharacter* DefaultCharacter = GetDefault<AFPPlayerCharacter>(GetClass());
	FVector& FirstPersonMeshRelativeLocation = FirstPersonMeshComponent->GetRelativeLocation_DirectMutable();
	FirstPersonMeshRelativeLocation.Z = DefaultCharacter->FirstPersonMeshComponent->GetRelativeLocation().Z;
}

FRotator AFPPlayerCharacter::GetViewRotation() const
{
	FRotator Result = Super::GetViewRotation();

	if (IsFPMontagePlaying() || GetBaseCharacterMovementComponent()->IsHardFalling())
	{
		FRotator SocketRotation = FirstPersonMeshComponent->GetSocketRotation(SocketFPCamera);
		Result.Pitch += SocketRotation.Pitch;
		Result.Yaw = SocketRotation.Yaw;
		Result.Roll = SocketRotation.Roll;
	}

	return Result;
}



void AFPPlayerCharacter::OnLadderStarted()
{
	SetCameraRestrictions(true, LadderCameraMinPitch, LadderCameraMaxPitch, FirstPersonCameraComponent->GetComponentRotation().Yaw + LadderCameraMinYaw, FirstPersonCameraComponent->GetComponentRotation().Yaw + LadderCameraMaxYaw);
}

void AFPPlayerCharacter::SetCameraRestrictions(bool bIsNeedViewRestrictions, float ViewPitchMin, float ViewPitchMax, float ViewYawMin, float ViewYawMax)
{
	if (PlayerController.IsValid())
	{
		APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager;
		PlayerController->SetIgnoreCameraPitch(bIsNeedViewRestrictions);
		bUseControllerRotationYaw = !bIsNeedViewRestrictions;

		CameraManager->ViewPitchMin = ViewPitchMin;
		CameraManager->ViewPitchMax = ViewPitchMax;
		CameraManager->ViewYawMin = ViewYawMin;
		CameraManager->ViewYawMax = ViewYawMax;
	}
}

void AFPPlayerCharacter::OnLadderStopped()
{
	APlayerCameraManager* DefaultCameraManager = PlayerController->PlayerCameraManager->GetClass()->GetDefaultObject<APlayerCameraManager>();
	SetCameraRestrictions(false, DefaultCameraManager->ViewPitchMin, DefaultCameraManager->ViewPitchMax, DefaultCameraManager->ViewYawMin, DefaultCameraManager->ViewYawMax);
}

void AFPPlayerCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);
	if (GetBaseCharacterMovementComponent()->IsOnLadder())
	{
		OnLadderStarted();
	}
	else if (PreviousCustomMode == (uint8)ECustomMovementMode::Ladder)
	{
		OnLadderStopped();
	}

	if (GetBaseCharacterMovementComponent()->IsOnZipline())
	{
		OnZiplineStarted();
	}
	else if (PreviousCustomMode == (uint8)ECustomMovementMode::Zipline)
	{
		OnZiplineStoped();
	}
}

void AFPPlayerCharacter::OnZiplineStarted()
{
	SetCameraRestrictions(true, ZiplineCameraMinPitch, ZiplineCameraMaxPitch, FirstPersonCameraComponent->GetComponentRotation().Yaw + ZiplineCameraMinYaw, FirstPersonCameraComponent->GetComponentRotation().Yaw + ZiplineCameraMaxYaw);
}

void AFPPlayerCharacter::OnZiplineStoped()
{
	APlayerCameraManager* DefaultCameraManager = PlayerController->PlayerCameraManager->GetClass()->GetDefaultObject<APlayerCameraManager>();
	SetCameraRestrictions(false, DefaultCameraManager->ViewPitchMin, DefaultCameraManager->ViewPitchMax, DefaultCameraManager->ViewYawMin, DefaultCameraManager->ViewYawMax);
}


void AFPPlayerCharacter::OnMantle(const FMantlingSettings& MantlingSettings, float MantlingAnimationStartTime)
{
	Super::OnMantle(MantlingSettings, MantlingAnimationStartTime);
	UAnimInstance* FPAnimInstance = FirstPersonMeshComponent->GetAnimInstance();
	if (IsValid(FPAnimInstance) && MantlingSettings.FPMantlingMontage)
	{
		if (PlayerController.IsValid())
		{
			PlayerController->SetIgnoreLookInput(true);
			PlayerController->SetIgnoreMoveInput(true);
		}
		float MontageDuration = FPAnimInstance->Montage_Play(MantlingSettings.FPMantlingMontage, 1.0f, EMontagePlayReturnType::Duration, MantlingAnimationStartTime);
		GetWorld()->GetTimerManager().SetTimer(FPMontageTimer, this, &AFPPlayerCharacter::OnFPMontageTimerElapsed, MontageDuration, false);
	}
}

void AFPPlayerCharacter::OnFPMontageTimerElapsed()
{
	if (PlayerController.IsValid())
	{
		PlayerController->SetIgnoreLookInput(false);
		PlayerController->SetIgnoreMoveInput(false);
	}
}

bool AFPPlayerCharacter::IsFPMontagePlaying() const
{
	UAnimInstance* FPAnimInstance = FirstPersonMeshComponent->GetAnimInstance();
	return IsValid(FPAnimInstance) && FPAnimInstance->IsAnyMontagePlaying();
}


void AFPPlayerCharacter::Falling()
{
	Super::Falling();
	UBaseCharacterMovementComponent* BaseCharacterMovement = GetBaseCharacterMovementComponent();
	if (!BaseCharacterMovement)
	{
		return;
	}
	BaseCharacterMovement->bNotifyApex = true;
}

void AFPPlayerCharacter::Landed(const FHitResult& Hit)
{

	float FallingHeight = StartFallingZ - GetActorLocation().Z;
	if (FallingHeight > HardFallingMinimumHeight && FallingHeight > 0)
	{
		StartHardLanded();
	}
	Super::Landed(Hit);
}

void AFPPlayerCharacter::NotifyJumpApex()
{
	Super::NotifyJumpApex();
	StartFallingZ = GetActorLocation().Z;
}

void AFPPlayerCharacter::StartHardLanded()
{
	UBaseCharacterMovementComponent* BaseCharacterMovement = GetBaseCharacterMovementComponent();
	if (BaseCharacterMovement)
	{
		BaseCharacterMovement->SetIsHardFalling(true);
	}
	if (PlayerController.IsValid())
	{
		PlayerController->SetIgnoreLookInput(true);
		PlayerController->SetIgnoreMoveInput(true);
	}
}

void AFPPlayerCharacter::EndHardLanded()
{
	UBaseCharacterMovementComponent* BaseCharacterMovement = GetBaseCharacterMovementComponent();
	if (BaseCharacterMovement)
	{
		BaseCharacterMovement->SetIsHardFalling(false);
	}
	if (PlayerController.IsValid())
	{
		PlayerController->SetIgnoreLookInput(false);
		PlayerController->SetIgnoreMoveInput(false);
	}
}
