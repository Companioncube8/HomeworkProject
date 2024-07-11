// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Equipment/Weapons/BowWeaponItem.h"

#include "Actors/Projectiles/Projectile.h"
#include "Components/TimelineComponent.h"


ABowWeaponItem::ABowWeaponItem()
{
	PrimaryActorTick.bCanEverTick = true;
}


void ABowWeaponItem::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	UpdateTensionValue();
}

void ABowWeaponItem::UpdateTensionValue()
{
	float ElapsedTime = GetWorld()->GetTimerManager().GetTimerElapsed(TensionTimer);
	if (!TensionTimer.IsValid())
	{
		return;
	}
	if (AProjectile* Projectile = WeaponBarell->GetProjectile())
	{
		float Speed = 0;
		float AlphaSpeed = ElapsedTime / FullStringTensionTime;
		if (ArrowSpeedCurve)
		{
			Speed = ArrowSpeedCurve->GetFloatValue(AlphaSpeed);
		} else
		{
			Speed = FMath::Lerp(ArrowMinSpeed, ArrowMaxSpeed, AlphaSpeed);
		}
		Projectile->SetSpeed(Speed);
		
		float AlphaDamage = (ElapsedTime - FullStringTensionTime) / (FullStringTensionMaxTime - FullStringTensionTime);
		float Damage = 0;
		if (ArrowDamageCurve)
		{
			Damage = ArrowDamageCurve->GetFloatValue(AlphaDamage);
		} else
		{
			Damage = FMath::Lerp(ArrowMinDamage, ArrowMaxDamage, AlphaDamage);
		}
		UE_LOG(LogTemp, Warning, TEXT("UpdateTensionValue: %f"), AlphaDamage)
		WeaponBarell->SetFireDamage(Damage);
		if (CachedCharacterOwner->OnUpdateStringTensionPercent.IsBound()) {
			CachedCharacterOwner->OnUpdateStringTensionPercent.Broadcast(Damage / ArrowMaxDamage);
		}
		
	}
}

void ABowWeaponItem::StopAim()
{
	Super::StopAim();

	StopStringTense();
	EndReload(false);
}


void ABowWeaponItem::AutoShot()
{
	StartFire();
}

void ABowWeaponItem::StartStringTense()
{
	if (CachedCharacterOwner->IsPlayerControlled() && CachedCharacterOwner->IsLocallyControlled())
	{
		if (!TensionTimer.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("StartStringTense"));
			GetWorld()->GetTimerManager().SetTimer(TensionTimer, this, &ABowWeaponItem::AutoShot, FullStringTensionMaxTime, false);
		}

		if (!UnstableTensionTimer.IsValid())
		{
			GetWorld()->GetTimerManager().SetTimer(UnstableTensionTimer, this, &ABowWeaponItem::UnstableAngleChange, UnstableSpreadAngleTimeChange, true);
		}

		if (CachedCharacterOwner->OnStartStringTension.IsBound()) {
			CachedCharacterOwner->OnStartStringTension.Broadcast(FullStringTensionTime);
		}
	}
}

void ABowWeaponItem::StopStringTense()
{
	if (!CachedCharacterOwner->IsPlayerControlled())
	{
		return;
	}
	if (!TensionTimer.IsValid())
	{
		return;
	}
	GetWorld()->GetTimerManager().ClearTimer(TensionTimer);
	GetWorld()->GetTimerManager().ClearTimer(UnstableTensionTimer);

	if (CachedCharacterOwner->OnEndStringTension.IsBound()) {
		CachedCharacterOwner->OnEndStringTension.Broadcast();
	}
}



float ABowWeaponItem::GetCurrentBulletSpreadAngle() const
{
	float ElapsedTime = GetWorld()->GetTimerManager().GetTimerElapsed(TensionTimer);

	float SpreadAngle = 0;
	if (ElapsedTime >= FullStringTensionMaxStableTime)
	{
		float Alpha = (ElapsedTime - FullStringTensionMaxStableTime) / (FullStringTensionMaxTime - FullStringTensionMaxStableTime);
		if (SpreadAngleCurve)
		{
			SpreadAngle = SpreadAngleCurve->GetFloatValue(Alpha);
		}
		else
		{
			SpreadAngle = FMath::Lerp(CurrentFireMode().AimSpreadAngle, UnstableSpreadAngleAutoShot, Alpha);
		}
	}
	else {
		float Alpha = ElapsedTime / FullStringTensionMaxStableTime;
		if (SpreadAngleCurve)
		{
			SpreadAngle = SpreadAngleCurve->GetFloatValue(Alpha);
		}
		else
		{
			SpreadAngle = FMath::Lerp(CurrentFireMode().SpreadAngle, CurrentFireMode().AimSpreadAngle, Alpha);
		}
	}
	return FMath::DegreesToRadians(SpreadAngle);
}

void ABowWeaponItem::EndReload(bool bIsSuccess)
{
	Super::EndReload(bIsSuccess);

	if (bIsSuccess) {
		StartStringTense();
	}
}

void ABowWeaponItem::UnstableAngleChange()
{
	float ElapsedTime = GetWorld()->GetTimerManager().GetTimerElapsed(TensionTimer);
	if (ElapsedTime < FullStringTensionMaxStableTime)
	{
		return;
	}

	ABaseCharacter* CharacterOwner = GetCharacterOwner();

	if (!CharacterOwner)
	{
		return;
	}

	float OffsetSize = FMath::Tan(FMath::DegreesToRadians(UnstableSpreadAngleChange));
	float RotationAngle = FMath::RandRange(0.f, 2 * PI);

	float OffsetY = FMath::Cos(RotationAngle);
	float OffsetX = FMath::Sin(RotationAngle);

	FVector Result = (FVector::ForwardVector * OffsetX + FVector::RightVector * OffsetY) * OffsetSize;

	if (CharacterOwner->OnReticalShake.IsBound())
	{
		CharacterOwner->OnReticalShake.Broadcast(Result);
	}
}


void ABowWeaponItem::StartFire()
{
	Super::StartFire();
	StopStringTense();
}
