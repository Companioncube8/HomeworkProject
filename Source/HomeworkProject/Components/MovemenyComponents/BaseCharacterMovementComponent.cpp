// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacterMovementComponent.h"

#include "Components/CapsuleComponent.h"
#include "Curves/CurveVector.h"
#include "GameFramework/Character.h"
#include "HomeworkProject/HomeworkProjectTypes.h"
#include "HomeworkProject/Actors/Interactive/Enviroment/Ladder.h"
#include "HomeworkProject/Actors/Interactive/Enviroment/Zipline.h"
#include "HomeworkProject/Characters/BaseCharacter.h"
#include "HomeworkProject/Characters/PlayerCharacter.h"

float UBaseCharacterMovementComponent::GetMaxSpeed() const
{
	float Result = Super::GetMaxSpeed();
	if (bIsSprinting)
	{
		Result = SprintSpeed;
	}
	if (bIsOutOfStamina)
	{
		Result = OutOfStaminaSpeed;
	}
	if (IsProning())
	{
		Result = MaxProneSpeed;
	}
	if (IsOnLadder())
	{
		Result = ClimbingOnLadderMaxSpeed;
	}
	if (IsWallRun())
	{
		Result = MaxWallRunSpeed;
	}
	if (IsSliding())
	{
		Result = SlideSpeed;
	}
	if (GetBaseCharacterOwner()->IsAiming())
	{
		Result = GetBaseCharacterOwner()->GetAimingMovementSpeed();
	}
	return Result;
}

void UBaseCharacterMovementComponent::BeginPlay() {
	Super::BeginPlay();
	CharacterOwner->GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &UBaseCharacterMovementComponent::OnPlayerCapsuleHit);
}

void UBaseCharacterMovementComponent::StartSprint()
{
	bIsSprinting = true;
	bForceMaxAccel = 1;
	OnSprintStart.ExecuteIfBound(true);
}

void UBaseCharacterMovementComponent::StopSprint()
{
	bIsSprinting = false;
	if (IsSliding())
	{
		return;
	}
	bForceMaxAccel = 0;
	OnSprintStart.ExecuteIfBound(false);
}

void UBaseCharacterMovementComponent::SetIsOutOfStamina(bool bIsOutOfStamina_In)
{
	if (bIsOutOfStamina != bIsOutOfStamina_In) 
	{
		bIsOutOfStamina = bIsOutOfStamina_In;
		if (bIsOutOfStamina)
		{
			StopSprint();
			Velocity = GetMaxSpeed() * Velocity.GetSafeNormal();
		}
		else 
		{
			Velocity = GetMaxSpeed() * Velocity.GetSafeNormal();
		}
	}
}

bool UBaseCharacterMovementComponent::IsProning() const
{
	checkf(GetCharacterOwner()->IsA<ABaseCharacter>(), TEXT("Prone can be use only with ABaseCharacter"));
	ABaseCharacter* CurrentCharacter = StaticCast<ABaseCharacter*>(GetCharacterOwner());

	return CurrentCharacter && CurrentCharacter->IsProne();
}

void UBaseCharacterMovementComponent::UpdateCharacterStateAfterMovement(float DeltaSeconds)
{
	Super::UpdateCharacterStateAfterMovement(DeltaSeconds);
	if (IsProning() && !CanProneInCurrentState())
	{
		UnProne();
	}
}

void UBaseCharacterMovementComponent::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
	Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);
	const bool bIsProning = IsProning();
	if (IsProning() && (!WantsToProne() || !CanProneInCurrentState()))
	{
		UnProne();
	}
	else if (!bIsProning && WantsToProne() && CanProneInCurrentState())
	{
		Prone();
	}
}

bool UBaseCharacterMovementComponent::CanProneInCurrentState() const
{
	return (IsFalling() || IsMovingOnGround()) && UpdatedComponent && !UpdatedComponent->IsSimulatingPhysics();
}

void UBaseCharacterMovementComponent::SetWantsToProne(bool bWantsToProne_In)
{
	bWantsToProne = bWantsToProne_In;
}

void UBaseCharacterMovementComponent::SetIsFullHeighte(bool bIsFullHeighte_In)
{
	bIsFullHeight = bIsFullHeighte_In;
}


void UBaseCharacterMovementComponent::Prone()
{
	if (!HasValidData())
	{
		return;
	}

	if (!CanProneInCurrentState())
	{
		return;
	}
	ACharacter* DefaultCharacter = GetCharacterOwner()->GetClass()->GetDefaultObject<ACharacter>();

	checkf(GetCharacterOwner()->IsA<ABaseCharacter>(), TEXT("Prone can be use only with ABaseCharacter"));
	ABaseCharacter* CurrentCharacter = StaticCast<ABaseCharacter*>(GetCharacterOwner());

	if (FMath::IsNearlyEqual(CurrentCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight(), ProneCapsuleHalfHeight))
	{
		CurrentCharacter->SetIsProne(true);
		bWantsToProne = false;
		CurrentCharacter->bIsCrouched = false;
		CurrentCharacter->OnStartProne(0.f, 0.f);
		return;
	}

	const float ComponentScale = CurrentCharacter->GetCapsuleComponent()->GetShapeScale();
	const float OldUnscaledHalfHeight = DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	const float OldUnscaledRadius = DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius();
	const float ClampedPronedHalfHeight = FMath::Max3(0.f, OldUnscaledRadius, ProneCapsuleHalfHeight);

	CurrentCharacter->GetCapsuleComponent()->SetCapsuleSize(ProneCapsuleRadius, ClampedPronedHalfHeight);
	float HalfHeightAdjust = (OldUnscaledHalfHeight - ClampedPronedHalfHeight);
	float ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;

	if (ClampedPronedHalfHeight > OldUnscaledHalfHeight)
	{
		FCollisionQueryParams CapsuleParams(SCENE_QUERY_STAT(ProneTrace), false, CurrentCharacter);
		FCollisionResponseParams ResponseParam;
		InitCollisionParams(CapsuleParams, ResponseParam);
		const bool bEncroached = GetWorld()->OverlapBlockingTestByChannel(UpdatedComponent->GetComponentLocation() - FVector(0.f, 0.f, ScaledHalfHeightAdjust), FQuat::Identity,
			UpdatedComponent->GetCollisionObjectType(), GetPawnCapsuleCollisionShape(SHRINK_None), CapsuleParams, ResponseParam);

		if (bEncroached)
		{
			CurrentCharacter->GetCapsuleComponent()->SetCapsuleSize(OldUnscaledRadius, OldUnscaledHalfHeight);
			return;
		}
	}

	UpdatedComponent->MoveComponent(FVector(0.f, 0.f, -ScaledHalfHeightAdjust), UpdatedComponent->GetComponentQuat(), true, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);

	CurrentCharacter->SetIsProne(true);
	bWantsToCrouch = false;
	CurrentCharacter->bIsCrouched = false;

	bForceNextFloorCheck = true;

	const float MeshAdjust = ScaledHalfHeightAdjust;
	HalfHeightAdjust = CrouchedHalfHeight - ClampedPronedHalfHeight;
	ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;
	CurrentCharacter->OnStartProne(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

bool UBaseCharacterMovementComponent::CanUncrouch(const float CurrentPronedHalfHeight, float ScaledHalfHeightAdjust, const FVector PawnLocation)
{
	const UWorld* MyWorld = GetWorld();
	const float SweepInflation = KINDA_SMALL_NUMBER * 10.f;
	FCollisionQueryParams CapsuleParams(SCENE_QUERY_STAT(ProneTrace), false, CharacterOwner);
	FCollisionResponseParams ResponseParam;
	InitCollisionParams(CapsuleParams, ResponseParam);

	const FCollisionShape StandingCapsuleShape = GetPawnCapsuleCollisionShape(SHRINK_HeightCustom, -SweepInflation - ScaledHalfHeightAdjust);
	const ECollisionChannel CollisionChannel = UpdatedComponent->GetCollisionObjectType();
	bool bEncroached = true;

	FVector StandingLocation = PawnLocation + FVector(0.f, 0.f, StandingCapsuleShape.GetCapsuleHalfHeight() - CurrentPronedHalfHeight);
	bEncroached = MyWorld->OverlapBlockingTestByChannel(StandingLocation, FQuat::Identity, CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParam);

	if (bEncroached)
	{
		if (IsMovingOnGround())
		{
			const float MinFloorDist = KINDA_SMALL_NUMBER * 10.f;
			if (CurrentFloor.bBlockingHit && CurrentFloor.FloorDist > MinFloorDist)
			{
				StandingLocation.Z -= CurrentFloor.FloorDist - MinFloorDist;
				bEncroached = MyWorld->OverlapBlockingTestByChannel(StandingLocation, FQuat::Identity, CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParam);
			}
		}
	}

	if (!bEncroached)
	{
		if (IsCrouching())
		{
			UpdatedComponent->MoveComponent(StandingLocation - PawnLocation, UpdatedComponent->GetComponentQuat(), false, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
		}
		bForceNextFloorCheck = true;
	}
	else
	{
		return false;
	}

	return true;
}

void UBaseCharacterMovementComponent::UnProne()
{
	if (!HasValidData())
	{
		return;
	}

	checkf(CharacterOwner->IsA<ABaseCharacter>(), TEXT("Prone can be use only with ABaseCharacter"));
	ABaseCharacter* CurrentCharacter = StaticCast<ABaseCharacter*>(CharacterOwner);

	ACharacter* DefaultCharacter = GetCharacterOwner()->GetClass()->GetDefaultObject<ACharacter>();
	float NewHalfHeight = IsFullHeight() ? DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() : CrouchedHalfHeight;

	if (FMath::IsNearlyEqual(CurrentCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight(), NewHalfHeight))
	{
		CurrentCharacter->SetIsProne(false);
		SetWantsToProne(false);
		CurrentCharacter->OnEndProne(0.f, 0.f);
		return;
	}

	const float CurrentPronedHalfHeight = CurrentCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	const float ComponentScale = CurrentCharacter->GetCapsuleComponent()->GetShapeScale();
	const float OldUnscaledHalfHeight = CurrentCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();

	float HalfHeightAdjust = NewHalfHeight - OldUnscaledHalfHeight;
	float ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;
	const FVector PawnLocation = UpdatedComponent->GetComponentLocation();

	check(CurrentCharacter->GetCapsuleComponent());

	if (!CanUncrouch(CurrentPronedHalfHeight, ScaledHalfHeightAdjust, PawnLocation))
	{
		return;
	};

	CurrentCharacter->SetIsProne(false);
	SetWantsToProne(false);
	if (!IsFullHeight())
	{
		CurrentCharacter->bIsCrouched = true;
		bWantsToCrouch = true;
	}
	CurrentCharacter->GetCapsuleComponent()->SetCapsuleSize(DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius(), NewHalfHeight, true);

	const float MeshAdjust = ScaledHalfHeightAdjust;
	AdjustProxyCapsuleSize();

	CurrentCharacter->OnEndProne(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

void UBaseCharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
	if (MovementMode == MOVE_Swimming)
	{
		CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(SwimmingCapsuleRadius, SwimmingCapsuleHalfHeight, true);
	}
	else if (PreviousMovementMode == MOVE_Swimming)
	{
		ACharacter* DefaultCharacter = CharacterOwner->GetClass()->GetDefaultObject<ACharacter>();
		CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius(), DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight(), true);
	}

	if (MovementMode == MOVE_Custom)
	{
		switch (CustomMovementMode)
		{
			case (uint8)ECustomMovementMode::Mantling:
			{

				GetWorld()->GetTimerManager().SetTimer(MantlingTimer, this, &UBaseCharacterMovementComponent::EndMantle, CurrentMantlingParameters.Duration, false);
				break;
			}
			case (uint8)ECustomMovementMode::Zipline:
			{
				float time = FVector::Distance(CurrentZipline->GetTopPoint(), CurrentZipline->GetBottomPoint()) / ZiplineMoveSpeed;
				GetWorld()->GetTimerManager().SetTimer(ZiplineTimer, this, &UBaseCharacterMovementComponent::EndZipline, time, false);
				break;
			}
			default:
				break;
		}
	}

	if (PreviousMovementMode == MOVE_Custom && PreviousCustomMode == (uint8)ECustomMovementMode::Ladder)
	{
		CurrentLadder = nullptr;
	}

	if (PreviousMovementMode == MOVE_Custom && PreviousCustomMode == (uint8)ECustomMovementMode::Zipline)
	{
		CurrentZipline = nullptr;
	}

	if (PreviousMovementMode == MOVE_Falling)
	{
		PreviousWallRunSide = EWallRunSide::None;
	}
}

void UBaseCharacterMovementComponent::StartMantle(const FMantlingMovementParameters& MantlingParameters)
{
	CurrentMantlingParameters = MantlingParameters;
	SetMovementMode(EMovementMode::MOVE_Custom, (uint8)ECustomMovementMode::Mantling);
}

void UBaseCharacterMovementComponent::EndMantle()
{
	SetMovementMode(MOVE_Walking);
}

void UBaseCharacterMovementComponent::EndZipline()
{
	DetachFromZipline();
}

bool UBaseCharacterMovementComponent::IsSliding() const
{
	return UpdatedComponent && MovementMode == MOVE_Custom && CustomMovementMode == (uint8)ECustomMovementMode::Slide;
}


bool UBaseCharacterMovementComponent::IsMantling() const
{
	return UpdatedComponent && MovementMode == MOVE_Custom && CustomMovementMode == (uint8)ECustomMovementMode::Mantling;
}

bool UBaseCharacterMovementComponent::IsWallRun() const
{
	return UpdatedComponent && MovementMode == MOVE_Custom && CustomMovementMode == (uint8)ECustomMovementMode::WallRun;
}

bool UBaseCharacterMovementComponent::IsOnLadder() const
{
	return UpdatedComponent && MovementMode == MOVE_Custom && CustomMovementMode == (uint8)ECustomMovementMode::Ladder;
}


bool UBaseCharacterMovementComponent::IsOnZipline() const
{
	return UpdatedComponent && MovementMode == MOVE_Custom && CustomMovementMode == (uint8)ECustomMovementMode::Zipline;
}

float UBaseCharacterMovementComponent::GetActorToCurrentLadderProjection(const FVector& Location) const
{

	checkf(IsValid(CurrentLadder), TEXT("UBaseCharacterMovementComponent::GetActorToCurrentLadderProjection() cannot be invoked when current ladder is null"));
	FVector LadderUpVector = CurrentLadder->GetActorUpVector();
	FVector LadderToCharacterDistance = Location - CurrentLadder->GetActorLocation();
	return FVector::DotProduct(LadderUpVector, LadderToCharacterDistance);
}

void UBaseCharacterMovementComponent::AttachToLadder(const class ALadder* Ladder)
{
	CurrentLadder = Ladder;

	FRotator TargetOrientationRotation = CurrentLadder->GetActorForwardVector().ToOrientationRotator();
	TargetOrientationRotation.Yaw += 180;
	TargetOrientationRotation.Normalize();

	FVector LadderUpVector = CurrentLadder->GetActorUpVector();
	FVector LadderForwardVector = CurrentLadder->GetActorForwardVector();
	float Projection = GetActorToCurrentLadderProjection(GetActorLocation());

	FVector NewCharacterLocation = CurrentLadder->GetActorLocation() + Projection * LadderUpVector + LadderToCharacterOffset * LadderForwardVector;
	if (CurrentLadder->GetIsOnTop())
	{
		NewCharacterLocation = CurrentLadder->GetAttachFromTopAnimMontageStartingLocation();
	}

	GetOwner()->SetActorLocation(NewCharacterLocation);
	GetOwner()->SetActorRotation(TargetOrientationRotation);

	SetMovementMode(MOVE_Custom, (uint8)ECustomMovementMode::Ladder);
}
void UBaseCharacterMovementComponent::DetachFromLadder(EDetachFromLadderMethod DetachFromLadderMethod)
{
	switch (DetachFromLadderMethod)
	{
	case EDetachFromLadderMethod::ReachingTheBottom:
	{
		SetMovementMode(MOVE_Walking);
		break;
	}
	case EDetachFromLadderMethod::ReachingTheTop:
	{
		GetBaseCharacterOwner()->Mantle(true);
		break;
	}
	case EDetachFromLadderMethod::Jump:
	{
		FVector JumpDirection = CurrentLadder->GetActorForwardVector();
		SetMovementMode(MOVE_Falling);

		FVector JumpVelocity = JumpDirection * JumpOffFromLadderSpeed;

		ForceTargetRotation = JumpDirection.ToOrientationRotator();
		bForceRotation = true;
		Launch(JumpVelocity);

		break;
	}
	case EDetachFromLadderMethod::Fall:
	default:
	{
		SetMovementMode(MOVE_Falling);
		break;
	}
	}
}

void UBaseCharacterMovementComponent::PhysLadder(float DeltaTime, int32 Iterations)
{
	CalcVelocity(DeltaTime, 1.0f, false, ClimbingOnLadderBrakingDeceleration);
	FVector Delta = Velocity * DeltaTime;

	if (HasAnimRootMotion())
	{
		FHitResult HitResult;
		SafeMoveUpdatedComponent(Delta, GetOwner()->GetActorRotation(), false, HitResult);
		return;
	}

	FVector NewPos = GetActorLocation() + Delta;
	float NewPosProjection = GetActorToCurrentLadderProjection(NewPos);

	if (NewPosProjection < MinLadderBottomOffset)
	{
		DetachFromLadder(EDetachFromLadderMethod::ReachingTheBottom);
		return;
	}
	else if (NewPosProjection > (CurrentLadder->GetLadderHeight() - MaxLadderTopOffset))
	{
		DetachFromLadder(EDetachFromLadderMethod::ReachingTheTop);
		return;
	}
	FHitResult HitResult;

	SafeMoveUpdatedComponent(Delta, GetOwner()->GetActorRotation(), false, HitResult);
}


void UBaseCharacterMovementComponent::PhysZipline(float DeltaTime, int32 Iterations)
{
	float ElapsedTime = GetWorld()->GetTimerManager().GetTimerElapsed(ZiplineTimer);
	float RateTime = GetWorld()->GetTimerManager().GetTimerRate(ZiplineTimer);

	FVector NewLocation = FMath::Lerp(CurrentZipline->GetTopPoint(), CurrentZipline->GetBottomPoint(), ElapsedTime / RateTime);

	FVector Delta = NewLocation - FVector(1.f, 1.f, GetCharacterOwner()->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() + OffsetFromTopZipline) - GetActorLocation();
	FHitResult HitResult;

	SafeMoveUpdatedComponent(Delta, GetOwner()->GetActorRotation(), false, HitResult);
}

void UBaseCharacterMovementComponent::PhysMantling(float DeltaTime, int32 Iterations)
{
	float ElapsedTime = GetWorld()->GetTimerManager().GetTimerElapsed(MantlingTimer) + CurrentMantlingParameters.StartTime;
	FVector MantlingCurveValue = CurrentMantlingParameters.MantlingCurve->GetVectorValue(ElapsedTime);
	float PositionAlpha = MantlingCurveValue.X;
	float XYCorrectionAlpha = MantlingCurveValue.Y;
	float ZCorrectionAlpha = MantlingCurveValue.Z;

	FVector CorrectedInitialLocation = FMath::Lerp(CurrentMantlingParameters.InitialLocation, CurrentMantlingParameters.InitialAnimationLocation, XYCorrectionAlpha);
	CorrectedInitialLocation.Z = FMath::Lerp(CurrentMantlingParameters.InitialLocation.Z, CurrentMantlingParameters.InitialAnimationLocation.Z, ZCorrectionAlpha);

	FVector NewLocation = FMath::Lerp(CorrectedInitialLocation, CurrentMantlingParameters.TargetLocation, PositionAlpha);
	FRotator NewRotation = FMath::Lerp(CurrentMantlingParameters.InitialRotation, CurrentMantlingParameters.TargetRotation, PositionAlpha);

	NewLocation += CurrentMantlingParameters.LedgeActor->GetRelativeLocation() - CurrentMantlingParameters.StartActorPosition;
	FVector Delta = NewLocation - GetActorLocation();
	FHitResult HitResult;

	SafeMoveUpdatedComponent(Delta, NewRotation, false, HitResult);
}

void UBaseCharacterMovementComponent::PhysWallRun(float DeltaTime, int32 iterations)
{
	if (!IsWallRun())
	{
		return;
	}
	UpdateWallRun();
	float ElapsedTime = GetWorld()->GetTimerManager().GetTimerElapsed(WallRunTimer);

	FVector Delta = Velocity * DeltaTime;
	Delta.Z = StartWallRunZPosition + (WallRunCurve->GetFloatValue(ElapsedTime / MaxWallRunTime) * MaxWallRunZOffset) - GetActorLocation().Z;
	FHitResult HitResult;

	SafeMoveUpdatedComponent(Delta, CurrentWallRunDirection.ToOrientationRotator(), false, HitResult);
}

void UBaseCharacterMovementComponent::PhysCustom(float DeltaTime, int32 Iterations)
{
	switch (CustomMovementMode)
	{
	case (uint8)ECustomMovementMode::Mantling:
	{
		PhysMantling(DeltaTime, Iterations);
		break;
	}
	case (uint8)ECustomMovementMode::Ladder:
	{
		PhysLadder(DeltaTime, Iterations);
		break;
	}
	case (uint8)ECustomMovementMode::Zipline:
	{
		PhysZipline(DeltaTime, Iterations);
		break;
	}
	case (uint8)ECustomMovementMode::WallRun:
	{
		PhysWallRun(DeltaTime, Iterations);
		break;
	}
	case(uint8)ECustomMovementMode::Slide:
	{
		PhysSliding(DeltaTime, Iterations);
		break;
	}
	default:
		break;
	}

	Super::PhysCustom(DeltaTime, Iterations);
}

const ALadder* UBaseCharacterMovementComponent::GetCurrentLadder()
{
	return CurrentLadder;
}

void UBaseCharacterMovementComponent::PhysicsRotation(float DeltaTime)
{
	if(bForceRotation)
	{
		FRotator CurrentRotation = UpdatedComponent->GetComponentRotation(); // Normalized
		CurrentRotation.DiagnosticCheckNaN(TEXT("CharacterMovementComponent::PhysicsRotation(): CurrentRotation"));

		FRotator DeltaRot = GetDeltaRotation(DeltaTime);
		DeltaRot.DiagnosticCheckNaN(TEXT("CharacterMovementComponent::PhysicsRotation(): GetDeltaRotation"));

		const float AngleTolerance = 1e-3f;

		if (!CurrentRotation.Equals(ForceTargetRotation, AngleTolerance))
		{
			FRotator DesiredRotation = ForceTargetRotation;
			// PITCH
			if (!FMath::IsNearlyEqual(CurrentRotation.Pitch, DesiredRotation.Pitch, AngleTolerance))
			{
				DesiredRotation.Pitch = FMath::FixedTurn(CurrentRotation.Pitch, DesiredRotation.Pitch, DeltaRot.Pitch);
			}

			// YAW
			if (!FMath::IsNearlyEqual(CurrentRotation.Yaw, DesiredRotation.Yaw, AngleTolerance))
			{
				DesiredRotation.Yaw = FMath::FixedTurn(CurrentRotation.Yaw, DesiredRotation.Yaw, DeltaRot.Yaw);
			}

			// ROLL
			if (!FMath::IsNearlyEqual(CurrentRotation.Roll, DesiredRotation.Roll, AngleTolerance))
			{
				DesiredRotation.Roll = FMath::FixedTurn(CurrentRotation.Roll, DesiredRotation.Roll, DeltaRot.Roll);
			}

			// Set the new rotation.
			DesiredRotation.DiagnosticCheckNaN(TEXT("CharacterMovementComponent::PhysicsRotation(): DesiredRotation"));
			MoveUpdatedComponent(FVector::ZeroVector, DesiredRotation, /*bSweep*/ false);
		}
		else
		{
			ForceTargetRotation = FRotator::ZeroRotator;
			bForceRotation = false;
		}
		return;
	}

	if (IsOnLadder())
	{
		return;
	}
	Super::PhysicsRotation(DeltaTime);
}

ABaseCharacter* UBaseCharacterMovementComponent::GetBaseCharacterOwner() const
{
	return StaticCast<ABaseCharacter*>(CharacterOwner);
}

float UBaseCharacterMovementComponent::GetLadderSpeedRation() const
{
	checkf(IsValid(CurrentLadder), TEXT("UBaseCharacterMovementComponent::GetLadderSpeedRation() cannot be invoked when current ladder is null"));

	FVector LadderUpVector = CurrentLadder->GetActorUpVector();
	return FVector::DotProduct(LadderUpVector, Velocity) / ClimbingOnLadderMaxSpeed;
}

void UBaseCharacterMovementComponent::AttachToZipline(const class AZipline* Zipline)
{
	CurrentZipline = Zipline;

	FRotator TargetOrientationRotation = Zipline->GetCabelOrientation();

	FVector PawnLocation = GetCharacterOwner()->GetActorLocation();

	GetOwner()->SetActorRotation(TargetOrientationRotation);

	SetMovementMode(MOVE_Custom, (uint8)ECustomMovementMode::Zipline);
}

void UBaseCharacterMovementComponent::DetachFromZipline()
{
	SetMovementMode(MOVE_Falling);
}

void UBaseCharacterMovementComponent::GetWallRunSideAndDirection(const FVector& HitNormal, EWallRunSide& OutSide, FVector& OutVectorDirection) const
{
	if (FVector::DotProduct(HitNormal, GetOwner()->GetActorRightVector()) > 0)
	{
		OutVectorDirection = FVector::CrossProduct(HitNormal, FVector::UpVector).GetSafeNormal();
		OutSide = EWallRunSide::Left;
	} else
	{
		OutVectorDirection = FVector::CrossProduct(FVector::UpVector, HitNormal).GetSafeNormal();
		OutSide = EWallRunSide::Right;
	}
}

void UBaseCharacterMovementComponent::OnPlayerCapsuleHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
 	FVector HitNormal = Hit.ImpactNormal;

	if (IsWallRun())
	{
		return;
	}

	if (!IsSurfaceWallRunnable(HitNormal))
	{
		return;
	}

	if (!IsFalling())
	{
		return;
	}

	EWallRunSide Side = EWallRunSide::None;
	FVector VectorDirection = FVector::ZeroVector;

	GetWallRunSideAndDirection(HitNormal, Side, VectorDirection);

	if (Side == PreviousWallRunSide)
	{
		return;
	}

	if (!AreRequiredKeysDown(Side))
	{
		return;
	}

	StartWallRun(Side, VectorDirection);
}

bool UBaseCharacterMovementComponent::IsSurfaceWallRunnable(const FVector& SurfaceNormal) const
{
	if (SurfaceNormal.Z > GetWalkableFloorZ() || SurfaceNormal.Z < -0.001f)
	{
		return false;
	}
	return true;
}

bool UBaseCharacterMovementComponent::AreRequiredKeysDown(EWallRunSide Side) const
{
	ABaseCharacter* Character = GetBaseCharacterOwner();

	if (Character->GetForwardAxisValue() < 0.1f)
	{
		return false;
	}

	if (Side == EWallRunSide::Right && Character->GetRightAxisValue() < -0.1f)
	{
		return  false;
	}
	if (Side == EWallRunSide::Left && Character->GetRightAxisValue() > 0.1f)
	{
		return  false;
	}

	return true;
}


void UBaseCharacterMovementComponent::StartWallRun(EWallRunSide Side, FVector& Direction)
{
	SetMovementMode(EMovementMode::MOVE_Custom, (uint8)ECustomMovementMode::WallRun);
	bOrientRotationToMovement = false;
	CurrentWallRunDirection = Direction;
	CurrentWallRunSide = Side;
	StartWallRunZPosition = GetActorLocation().Z;

	GetWorld()->GetTimerManager().ClearTimer(WallRunTimer);
	GetWorld()->GetTimerManager().SetTimer(WallRunTimer, this, &UBaseCharacterMovementComponent::StopWallRun, MaxWallRunTime);
}

void UBaseCharacterMovementComponent::StopWallRun()
{
	SetMovementMode(MOVE_Falling);
	bOrientRotationToMovement = true;
	CurrentWallRunDirection = FVector::ZeroVector;
	PreviousWallRunSide = CurrentWallRunSide;
	CurrentWallRunSide = EWallRunSide::None;
	StartWallRunZPosition = 0;
	GetWorld()->GetTimerManager().ClearTimer(WallRunTimer);
}

void UBaseCharacterMovementComponent::UpdateWallRun()
{
	if (!AreRequiredKeysDown(CurrentWallRunSide))
	{
		StopWallRun();
		return;
	}

	FHitResult HitResult;

	FVector StartPosition = GetActorLocation();
	FVector LineTraceDirection = CurrentWallRunSide == EWallRunSide::Right ? GetOwner()->GetActorRightVector() : -GetOwner()->GetActorRightVector();
	float LineTraceLenght = 200.f;
	FVector EndPosition = StartPosition + LineTraceDirection * LineTraceLenght;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());

	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartPosition, EndPosition, ECC_WallRunnable, QueryParams))
	{
		FVector HitNormal = HitResult.ImpactNormal;
		EWallRunSide Side = EWallRunSide::None;
		FVector VectorDirection = FVector::ZeroVector;

		GetWallRunSideAndDirection(HitNormal, Side, VectorDirection);

		if (Side != CurrentWallRunSide)
		{
			StopWallRun();
		} else
		{
			CurrentWallRunDirection = VectorDirection;
			Velocity = GetMaxSpeed() * CurrentWallRunDirection;
		}
	}
	else
	{
		StopWallRun();
	}
}


void UBaseCharacterMovementComponent::StartSlide()
{
	SetMovementMode(EMovementMode::MOVE_Custom, (uint8)ECustomMovementMode::Slide);

	APlayerCharacter* Character = StaticCast<APlayerCharacter*>(CharacterOwner);
	Character->ChangeMeshOffset(Character->GetDefaultHalfHeight() - SlideCaspsuleHalfHeight);
	Character->GetCapsuleComponent()->SetCapsuleHalfHeight(SlideCaspsuleHalfHeight);

	UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
	float Duration = AnimInstance->Montage_Play(SlideAnimMontage, SlideAnimMontage->CalculateSequenceLength() / SlideMaxTime, EMontagePlayReturnType::Duration, 0);
	Character->UpdateCameraOffset(Character->GetDefaultHalfHeight() - SlideCaspsuleHalfHeight, true);
	GetWorld()->GetTimerManager().SetTimer(SlidingTimer, this, &UBaseCharacterMovementComponent::StopSlide, SlideMaxTime, false);
}

void UBaseCharacterMovementComponent::StopSlide()
{
	GetWorld()->GetTimerManager().ClearTimer(SlidingTimer);
	SetMovementMode(MOVE_Walking);
	if (!bIsSprinting)
	{
		StopSprint();
	}

	APlayerCharacter* Character = StaticCast<APlayerCharacter*>(CharacterOwner);
	const FVector PawnLocation = UpdatedComponent->GetComponentLocation();
	ACharacter* DefaultCharacter = GetCharacterOwner()->GetClass()->GetDefaultObject<ACharacter>();
	Character->GetCapsuleComponent()->SetCapsuleHalfHeight(DefaultCharacter->GetDefaultHalfHeight());
	Character->ChangeMeshOffset(0);

	Character->UpdateCameraOffset(Character->GetDefaultHalfHeight() - SlideCaspsuleHalfHeight, false);

	if (!CanUncrouch(SlideCaspsuleHalfHeight, CrouchedHalfHeight, PawnLocation))
	{
		Crouch();
	}
}

void UBaseCharacterMovementComponent::PhysSliding(float DeltaTime, int32 Iterations)
{
	FVector Delta = Velocity * DeltaTime;

	FHitResult HitResult;
	SafeMoveUpdatedComponent(Delta, GetOwner()->GetActorRotation(), true, HitResult);
}

bool UBaseCharacterMovementComponent::IsHardFalling() const
{
	return bIsHardFalling;
}

void UBaseCharacterMovementComponent::SetIsHardFalling(bool bIsHardFalling_In)
{
	bIsHardFalling = bIsHardFalling_In;
}
