// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"

#include "AIController.h"
#include "Actors/Environment/PlatformTrigger.h"
#include "Actors/Equipment/Weapons/MeleeWeaponItem.h"
#include "Components/CapsuleComponent.h"
#include "Components/CharacterComponents/CharacterEquipmentComponent.h"
#include "Curves/CurveVector.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PhysicsVolume.h"
#include "HomeworkProject/LedgeDetectorComponent.h"
#include "HomeworkProject/Components/MovemenyComponents/BaseCharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "HomeworkProject/Actors/Interactive/Enviroment/Ladder.h"
#include "HomeworkProject/Actors/Interactive/Enviroment/Zipline.h"
#include "HomeworkProject/Components/CharacterComponents/CharacterAttributeComponent.h"
#include "HomeworkProject/HomeworkProjectTypes.h"
#include "Net/UnrealNetwork.h"


ABaseCharacter::ABaseCharacter(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer.SetDefaultSubobjectClass<UBaseCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	BaseCharacterMovementComponent = StaticCast<UBaseCharacterMovementComponent*>(GetCharacterMovement());
	IKTraceDistance = GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	CurrentStamina = MaxStamina;
	BaseCharacterMovementComponent->OnSprintStart.BindUObject(this, &ABaseCharacter::OnSprintChanged);
	
	LedgeDetectorComponent = CreateDefaultSubobject<ULedgeDetectorComponent>(TEXT("LedgeDetector"));

	CharacterEquipmentComponent = CreateDefaultSubobject<UCharacterEquipmentComponent>(TEXT("CharacterEquipment"));


	GetMesh()->CastShadow = true;
	GetMesh()->bCastDynamicShadow = true;

	CharacterAttributesComponent = CreateDefaultSubobject<UCharacterAttributeComponent>(TEXT("CharaterAttributes"));

	CharacterAttributesComponent->OnDeathEvent.AddUObject(this, &ABaseCharacter::OnDeath);
	CharacterAttributesComponent->OnOutOfStamina.AddUObject(this, &ABaseCharacter::OnOutOfStamina);
	CharacterAttributesComponent->OnOutOfOxigen.AddUObject(this, &ABaseCharacter::OnOutOfOxigen);
}

void ABaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	AAIController* AIController = Cast<AAIController>(NewController);
	if (AIController)
	{
		FGenericTeamId TeamId((uint8)Team);
		AIController->SetGenericTeamId(TeamId);
	}
}

void ABaseCharacter::ChangeCrouchState()
{
	if (BaseCharacterMovementComponent->IsSliding())
	{
		return;
	}
	if (!BaseCharacterMovementComponent->IsProning() && !BaseCharacterMovementComponent->IsCrouching())
	{
		Crouch();
	}
}

void ABaseCharacter::StartSprint()
{
	bIsSprintRequested = true;
	if (bIsCrouched)
	{
		UnCrouch();
	}
}

void ABaseCharacter::StopSprint()
{
	bIsSprintRequested = false;
}

void ABaseCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	TryChangeSprintState(DeltaSeconds);
	FVector IKLeftFootOuthitLocation = GetIKOutHitLocationForASocket(LeftFootSocketName);
	FVector IKRightFootOuthitLocation = GetIKOutHitLocationForASocket(RightFootSocketName);
	if (FMath::Abs(IKRightFootOuthitLocation.Z - IKLeftFootOuthitLocation.Z) < 50)
	{
		IKHipOffset = FMath::FInterpTo(IKHipOffset, FMath::Abs(IKRightFootOuthitLocation.Z - IKLeftFootOuthitLocation.Z) * -1, DeltaSeconds, IKInterpSpeed);
	}
	IKRightFootOffset = FMath::IsNearlyZero(IKRightFootOuthitLocation.Z) ? 0 : FMath::FInterpTo(IKRightFootOffset, IKRightFootOuthitLocation.Z - GetMesh()->GetComponentLocation().Z - IKHipOffset, DeltaSeconds, IKInterpSpeed);

	IKLeftFootOffset = FMath::IsNearlyZero(IKLeftFootOuthitLocation.Z) ? 0 : FMath::FInterpTo(IKLeftFootOffset, IKLeftFootOuthitLocation.Z - GetMesh()->GetComponentLocation().Z - IKHipOffset, DeltaSeconds, IKInterpSpeed);

	CharacterAttributesComponent->UpdateStaminaValue(DeltaSeconds, BaseCharacterMovementComponent->IsSprinting());
	CharacterAttributesComponent->UpdateOxygenValue(DeltaSeconds, IsSwimmingUnderWater());
}

void ABaseCharacter::OnSprintChanged(bool isSprint)
{
	if (isSprint)
	{
		OnSprintStart();
	}
	else
	{
		OnSprintEnd();
	}
}

bool ABaseCharacter::CanSprint()
{
	return !BaseCharacterMovementComponent->IsOutOfStamina();
}


void ABaseCharacter::Jump()
{
	UBaseCharacterMovementComponent* MovementComponent = GetBaseCharacterMovementComponent();
	if (!MovementComponent)
	{
		return;
	}
	if (MovementComponent->IsWallRun())
	{
		FVector JumpDirection = FVector::ZeroVector;
		if (MovementComponent->GetWallRunSide() == EWallRunSide::Right)
		{
			JumpDirection = FVector::CrossProduct(MovementComponent->GetWallRunDirection(), FVector::UpVector).GetSafeNormal();
		}
		else
		{
			JumpDirection = FVector::CrossProduct(FVector::UpVector, MovementComponent->GetWallRunDirection()).GetSafeNormal();
		}

		JumpDirection += FVector::UpVector;

		LaunchCharacter(MovementComponent->JumpZVelocity * JumpDirection.GetSafeNormal(), false, true);
		MovementComponent->StopWallRun();
	}
	if (IsProne())
	{
		UnProne(true);
	}
	else
	{
		Super::Jump();
	}
}

bool ABaseCharacter::CanJumpInternal_Implementation() const
{
	return !GetBaseCharacterMovementComponent()->IsMantling() && !BaseCharacterMovementComponent->IsOutOfStamina() && !BaseCharacterMovementComponent->IsSliding() && Super::CanJumpInternal_Implementation();
}

void ABaseCharacter::TryChangeSprintState(float DeltaTime)
{
	if (bIsSprintRequested && !BaseCharacterMovementComponent->IsSprinting() && CanSprint())
	{
		BaseCharacterMovementComponent->StartSprint();
	}

	if (!bIsSprintRequested && BaseCharacterMovementComponent->IsSprinting())
	{
		BaseCharacterMovementComponent->StopSprint();
	}

	if (BaseCharacterMovementComponent->IsSprinting())
	{
		CurrentStamina -= SprintStaminaConsumptionVelocity * DeltaTime;
		CurrentStamina = FMath::Clamp(CurrentStamina, 0.0f, MaxStamina);
	}
}

void ABaseCharacter::OnSprintEnd_Implementation() {}

void ABaseCharacter::OnSprintStart_Implementation() {}

FVector ABaseCharacter::GetIKOutHitLocationForASocket(const FName& SocketName)
{
	FVector Result = FVector::ZeroVector;

	if (USkeletalMeshComponent* SkeletalMesh = GetMesh()) {
		FVector SocketLocation = SkeletalMesh->GetSocketLocation(SocketName);
		FVector TraceStart(SocketLocation.X, SocketLocation.Y, GetActorLocation().Z);
		FVector TraceEnd(SocketLocation.X, SocketLocation.Y, SocketLocation.Z - IKTraceDistance);

		FHitResult HitResult;
		ETraceTypeQuery TraceType = UEngineTypes::ConvertToTraceType(ECC_Visibility);
		if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), TraceStart, TraceEnd, TraceType, true, TArray<AActor*>(), EDrawDebugTrace::ForOneFrame, HitResult, true))
		{
			Result = HitResult.Location;
		}
	}

	return Result;
}


void ABaseCharacter::ChangeProneState()
{
	if (!BaseCharacterMovementComponent->IsCrouching() && BaseCharacterMovementComponent->IsProning())
	{
		UnProne(false);
	}
	else if (BaseCharacterMovementComponent->IsCrouching() && !BaseCharacterMovementComponent->IsProning())
	{
		Prone();
	}
}

void ABaseCharacter::SetIsProne(bool bIsProne_In)
{
	bIsProne = bIsProne_In;
}

bool ABaseCharacter::CanProne() const
{
	return BaseCharacterMovementComponent && !bIsProne && GetRootComponent() && !GetRootComponent()->IsSimulatingPhysics();
}

void ABaseCharacter::Prone()
{
	if (UBaseCharacterMovementComponent* BaseMovementComponent = BaseCharacterMovementComponent)
	{
		if (CanProne())
		{
			BaseMovementComponent->SetWantsToProne(true);
		}
	}
}

void ABaseCharacter::UnProne(bool bIsFullHeight)
{
	if (UBaseCharacterMovementComponent* BaseMovementComponent = BaseCharacterMovementComponent)
	{
		BaseMovementComponent->SetIsFullHeighte(bIsFullHeight);
		BaseMovementComponent->SetWantsToProne(false);
	}
}


void ABaseCharacter::OnEndProne_Implementation(float HeightAdjust, float ScaledHeightAdjust)
{
	float ZOffset = BaseCharacterMovementComponent->IsFullHeight() ? 0 : BaseCharacterMovementComponent->CrouchedHalfHeight - BaseCharacterMovementComponent->ProneCapsuleHalfHeight;
	ChangeMeshOffset(ZOffset);
}

void ABaseCharacter::ChangeMeshOffset(float Offset)
{
	RecalculateBaseEyeHeight();

	const ACharacter* DefaultChar = GetDefault<ACharacter>(GetClass());
	UMeshComponent* CurrentMesh = GetMesh();
	if (CurrentMesh && DefaultChar->GetMesh())
	{
		FVector& MeshRelativeLocation = CurrentMesh->GetRelativeLocation_DirectMutable();
		MeshRelativeLocation.Z = DefaultChar->GetMesh()->GetRelativeLocation().Z + Offset;
		BaseTranslationOffset.Z = MeshRelativeLocation.Z;
	}
	else
	{
		BaseTranslationOffset.Z = DefaultChar->GetBaseTranslationOffset().Z + Offset;
	}
}

void ABaseCharacter::OnStartProne_Implementation(float HeightAdjust, float ScaledHeightAdjust)
{
	const ACharacter* DefaultChar = GetDefault<ACharacter>(GetClass());
	ChangeMeshOffset(DefaultChar->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() - BaseCharacterMovementComponent->ProneCapsuleHalfHeight);
}

void ABaseCharacter::Mantle(bool bForce)
{
	if (!(CanMantle() || bForce))
	{
		return;
	}
	if (BaseCharacterMovementComponent->IsSliding())
	{
		return;
	}
	if (BaseCharacterMovementComponent->IsOnZipline())
	{
		return;
	}
	FLedgeDescription LedgeDescription;
	if (GetBaseCharacterMovementComponent()->IsMantling())
	{
		return;
	}
	if (LedgeDetectorComponent->DetectLedge(LedgeDescription))
	{
		bIsMantling = true;

		const ACharacter* DefaultChar = GetDefault<ACharacter>(GetClass());

		FMantlingMovementParameters MantlingMovementParameters;
		MantlingMovementParameters.InitialLocation = bIsCrouched ? GetActorLocation() + FVector::UpVector * BaseCharacterMovementComponent->CrouchedHalfHeight : GetActorLocation();
		if (bIsCrouched)
		{
			UnCrouch();
		}
		MantlingMovementParameters.InitialRotation = GetActorRotation();
		MantlingMovementParameters.TargetLocation = LedgeDescription.Location;
		MantlingMovementParameters.TargetRotation = LedgeDescription.Rotator;
		MantlingMovementParameters.LedgeActor = LedgeDescription.LedgePrimitiveComponent;
		MantlingMovementParameters.StartActorPosition = LedgeDescription.StartLedgePosition;

		float MantlingHeight = LedgeDescription.Height - (MantlingMovementParameters.InitialLocation.Z - GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
		const FMantlingSettings& MantlingSettings = GetMantlingSettings(MantlingHeight);

		float MinRange;
		float MaxRange;
		MantlingSettings.MantlingCurve->GetTimeRange(MinRange, MaxRange);

		MantlingMovementParameters.Duration = MaxRange - MinRange;


		MantlingMovementParameters.MantlingCurve = MantlingSettings.MantlingCurve;

		FVector2D SourceRange(MantlingSettings.MinHeight, MantlingSettings.MaxHeight);
		FVector2D TargetRange(MantlingSettings.MinHeightStartTime, MantlingSettings.MaxHeightStartTime);
		MantlingMovementParameters.StartTime = FMath::GetMappedRangeValueClamped(SourceRange, TargetRange, MantlingHeight);

		MantlingMovementParameters.InitialAnimationLocation = MantlingMovementParameters.TargetLocation - MantlingSettings.AnimationCorrectionZ * FVector::UpVector + MantlingSettings.AnimationCorrectionXY * LedgeDescription.LedgeNormal;

		if (IsLocallyControlled() || GetLocalRole() == ROLE_Authority)
		{
			GetBaseCharacterMovementComponent()->StartMantle(MantlingMovementParameters);
		}

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		AnimInstance->Montage_Play(MantlingSettings.MantlingMontage, 1.f, EMontagePlayReturnType::Duration, MantlingMovementParameters.StartTime);
		OnMantle(MantlingSettings, MantlingMovementParameters.StartTime);
	}
}


const FMantlingSettings& ABaseCharacter::GetMantlingSettings(float LedgeHeight) const
{
	return LedgeHeight > LowMantleMaxHeight ? HighMantlingSettings : LowMantlingSettings;
}

void ABaseCharacter::RegisterInteractiveActor(AInteractiveActor* InteractiveActor)
{
	AvailibleInteractiveActors.AddUnique(InteractiveActor);
}

void ABaseCharacter::UnregisterInteractiveActor(AInteractiveActor* InteractiveActor)
{
	AvailibleInteractiveActors.RemoveSingleSwap(InteractiveActor);
}

void ABaseCharacter::ClimbLadderUp(float Value)
{
	UBaseCharacterMovementComponent* MovementComponent = GetBaseCharacterMovementComponent();
	if (!MovementComponent)
	{
		return;
	}
	if (MovementComponent->IsOnLadder() && !FMath::IsNearlyZero(Value))
	{
		FVector LadderUpVector = MovementComponent->GetCurrentLadder()->GetActorUpVector();
		AddMovementInput(LadderUpVector, Value);

	}
}

void ABaseCharacter::InteractWithLadder()
{
	if (BaseCharacterMovementComponent->IsSliding())
	{
		return;
	}
	UBaseCharacterMovementComponent* MovementComponent = GetBaseCharacterMovementComponent();
	if (!MovementComponent)
	{
		return;
	}
	if (BaseCharacterMovementComponent->IsSliding())
	{
		return;
	}
	if (MovementComponent->IsOnLadder())
	{
		MovementComponent->DetachFromLadder(EDetachFromLadderMethod::Jump);
	}
	else
	{
		const ALadder* AvailableLadder = GetAvailableLadder();
		if (IsValid(AvailableLadder))
		{
			if (AvailableLadder->GetIsOnTop())
			{
				PlayAnimMontage(AvailableLadder->GetAttachFromTopAnimMontage());
			}
			MovementComponent->AttachToLadder(AvailableLadder);
		}
	}
}


void ABaseCharacter::InteractWithZipline()
{
	if (BaseCharacterMovementComponent->IsSliding())
	{
		return;
	}
	UBaseCharacterMovementComponent* MovementComponent = GetBaseCharacterMovementComponent();
	if (!MovementComponent)
	{
		return;
	}

	if (BaseCharacterMovementComponent->IsSliding())
	{
		return;
	}

	if (MovementComponent->IsOnZipline())
	{
		MovementComponent->DetachFromZipline();
	}
	else
	{
		const AZipline* AvailableZipline = GetAvailableZipline();
		if (IsValid(AvailableZipline) && FVector::Distance(AvailableZipline->GetTopPoint(), GetActorLocation()) < MaxDistanceBetweenZiplineTopAndPawn)
		{
			MovementComponent->AttachToZipline(AvailableZipline);
		}
	}
}



const ALadder* ABaseCharacter::GetAvailableLadder()
{
	const ALadder* Result = nullptr;
	for (const AInteractiveActor* InteractiveActor: AvailibleInteractiveActors)
	{
		if (InteractiveActor->IsA<ALadder>())
		{
			Result = StaticCast<const ALadder*>(InteractiveActor);
			break;
		}
	}
	return Result;
}

const AZipline* ABaseCharacter::GetAvailableZipline()
{
	const AZipline* Result = nullptr;
	for (const AInteractiveActor* InteractiveActor : AvailibleInteractiveActors)
	{
		if (InteractiveActor->IsA<AZipline>())
		{
			Result = StaticCast<const AZipline*>(InteractiveActor);
			break;
		}
	}
	return Result;
}


bool ABaseCharacter::CanMantle() const
{
	return !GetBaseCharacterMovementComponent()->IsOnLadder();
}

void ABaseCharacter::Slide()
{
	if (BaseCharacterMovementComponent->IsSwimming())
	{
		return;
	}
	if (!BaseCharacterMovementComponent->IsSprinting())
	{
		return;
	}
	if (BaseCharacterMovementComponent->IsSliding())
	{
		return;
	}
	BaseCharacterMovementComponent->StartSlide();
}


void ABaseCharacter::OnMantle(const FMantlingSettings& MantlingSettings, float MantlingAnimationStartTime)
{

}

void ABaseCharacter::OnDeath()
{
	float Duration = PlayAnimMontage(OnDeathAnimMontage);
	if (Duration == 0.f)
	{
		EnableRagdoll();
	}
	GetCharacterMovement()->DisableMovement();
}

void ABaseCharacter::EnableRagdoll()
{
	GetMesh()->SetCollisionProfileName(CollisionProfileRagdoll);
	GetMesh()->SetSimulatePhysics(true);
}


void ABaseCharacter::NotifyJumpApex()
{
	Super::NotifyJumpApex();
	CurrentFallApex = GetActorLocation();

}

void ABaseCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	float FallingHeight = (CurrentFallApex - GetActorLocation()).Z * 0.01f;
	if (IsValid(FallDamageCurve))
	{
		float DamageAmount = FallDamageCurve->GetFloatValue(FallingHeight);
		TakeDamage(DamageAmount, FDamageEvent(), GetController(), Hit.Actor.Get());
	}
}

void ABaseCharacter::Falling()
{
	GetCharacterMovement()->bNotifyApex = true;
}

void ABaseCharacter::OnOutOfStamina(bool OutOfStamina)
{
	UBaseCharacterMovementComponent* BaseCharacterMovement = GetBaseCharacterMovementComponent();
	if (!BaseCharacterMovement)
	{
		return;
	}

	BaseCharacterMovement->SetIsOutOfStamina(OutOfStamina);
}

bool ABaseCharacter::IsSwimmingUnderWater() const
{
	if (GetCharacterMovement()->IsSwimming())
	{
		FVector HeadPosition = GetMesh()->GetSocketLocation(SocketHead);
		APhysicsVolume* Volume = GetCharacterMovement()->GetPhysicsVolume();
		float VolumeTopPlane = Volume->GetBounds().GetBoxExtrema(1).Z;
		return HeadPosition.Z < VolumeTopPlane;
	}

	return false;
}

void ABaseCharacter::SwimmingUnderWaterTakeDamage()
{
	if (CharacterAttributesComponent->IsOutOfOxigen())
	{
		TakeDamage(OutOfOxygenDamageValue, FDamageEvent(), GetController(), GetCharacterMovement()->GetPhysicsVolume());
	}
}

void ABaseCharacter::OnOutOfOxigen(bool OutOfOxigen)
{
	if (OutOfOxigen)
	{
		if (!OutOfOxigenDamageTimer.IsValid())
		{
			GetWorld()->GetTimerManager().SetTimer(OutOfOxigenDamageTimer, this, &ABaseCharacter::SwimmingUnderWaterTakeDamage, OutOfOxygenDamageInterval, true);
		}
	}
	else {
		GetWorld()->GetTimerManager().ClearTimer(OutOfOxigenDamageTimer);
	}
}

const UCharacterEquipmentComponent* ABaseCharacter::GetCharacterEquipmentComponent() const
{
	return CharacterEquipmentComponent;
}

void ABaseCharacter::PlayerStartFire()
{
	if (CharacterEquipmentComponent->IsEquipping())
	{
		return;
	}
	ARangeWeaponItem* CurrentRangeWeapon = CharacterEquipmentComponent->GetCurrentRangeWeapon();
	if (IsValid(CurrentRangeWeapon))
	{
		CurrentRangeWeapon->StartFire();
	}
}

void ABaseCharacter::PlayerStopFire()
{
	ARangeWeaponItem* CurrentRangeWeapon = CharacterEquipmentComponent->GetCurrentRangeWeapon();
	if (IsValid(CurrentRangeWeapon))
	{
		CurrentRangeWeapon->StopFire();
	}
	
}


void ABaseCharacter::StopAiming()
{
	if (!bIsAiming)
	{
		return;
	}
	ARangeWeaponItem* CurrentRangeWeapon = GetCharacterEquipmentComponent()->GetCurrentRangeWeapon();
	if (IsValid(CurrentRangeWeapon))
	{
		CurrentRangeWeapon->StopAim();
	}
	CurrentAimingMovementSpeed = 0.f;
	bIsAiming = false;
	OnStopAiming();
}

void ABaseCharacter::StartAiming()
{
	ARangeWeaponItem* CurrentRangeWeapon = GetCharacterEquipmentComponent()->GetCurrentRangeWeapon();
	if(!IsValid(CurrentRangeWeapon))
	{
		return;
	}
	bIsAiming = true;
	CurrentAimingMovementSpeed = CurrentRangeWeapon->GetAimMovmentMaxSpeed();
	CurrentRangeWeapon->StartAim();
	OnStartAiming();
}

bool ABaseCharacter::IsAiming() const
{
	return bIsAiming;
}


float ABaseCharacter::GetAimingMovementSpeed() const
{
	return CurrentAimingMovementSpeed;
}

void ABaseCharacter::OnStopAimingInternal()
{
	if (OnAimingStateChanged.IsBound())
	{
		OnAimingStateChanged.Broadcast(false);
	}
}

void ABaseCharacter::OnStartAimingInternal()
{
	if (OnAimingStateChanged.IsBound())
	{
		OnAimingStateChanged.Broadcast(true);
	}

}

void ABaseCharacter::OnStartAiming_Implementation()
{
	OnStartAimingInternal();
}

void ABaseCharacter::OnStopAiming_Implementation()
{
	OnStopAimingInternal();
}

const UCharacterAttributeComponent* ABaseCharacter::GetCharacterAttributeComponent() const
{
	return  CharacterAttributesComponent;
}

UCharacterAttributeComponent* ABaseCharacter::GetCharacterAttributeComponent_Mutable() const
{
	return  CharacterAttributesComponent;
}

UCharacterEquipmentComponent* ABaseCharacter::GetCharacterEquipmentComponent_Mutable() const
{
	return  CharacterEquipmentComponent;
}

void ABaseCharacter::Reload()
{
	if (IsValid(CharacterEquipmentComponent->GetCurrentRangeWeapon()))
	{
		CharacterEquipmentComponent->ReloadCurrentWeapon();
	}
}

void ABaseCharacter::NextItem()
{
	CharacterEquipmentComponent->EquipNextItem();
}

void ABaseCharacter::PreviousItem()
{
	CharacterEquipmentComponent->EquipPreviousItem();
}

void ABaseCharacter::EquipPrimaryItem()
{
	CharacterEquipmentComponent->EquipItemInSlot(EEquipmentSlots::PrimaryItemSlot);
}


void ABaseCharacter::AlternativeFire()
{
	CharacterEquipmentComponent->ChangeCurrentWeaponFireMode();
}

void ABaseCharacter::PrimaryMeleeAttack()
{
	if (AMeleeWeaponItem* CurrentMeleeWeapon = CharacterEquipmentComponent->GetCurrentMeleeWeapon())
	{
		CurrentMeleeWeapon->StartAttack(EMeleeAttackTypes::PrimaryAttack);
	}
}

void ABaseCharacter::SecondaryMeleeAttack()
{

	if (AMeleeWeaponItem* CurrentMeleeWeapon = CharacterEquipmentComponent->GetCurrentMeleeWeapon())
	{
		CurrentMeleeWeapon->StartAttack(EMeleeAttackTypes::SecondaryAttack);
	}
}

FGenericTeamId ABaseCharacter::GetGenericTeamId() const
{
	return  FGenericTeamId((uint8)Team);
}

void ABaseCharacter::OnRep_IsMantling(bool bWasMantling)
{
	if (GetLocalRole() == ROLE_SimulatedProxy && !bWasMantling && bIsMantling)
	{
		Mantle(true);
	}
}

void ABaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABaseCharacter, bIsMantling);
}
