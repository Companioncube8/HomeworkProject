// Fill out your copyright notice in the Description page of Project Settings.


#include "RangeWeaponItem.h"

#include "HomeworkProjectTypes.h"
#include "Characters/BaseCharacter.h"
#include "Components/Weapon/WeaponBarellComponent.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"

ARangeWeaponItem::ARangeWeaponItem()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponRoot"));

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(RootComponent);

	WeaponBarell = CreateDefaultSubobject<UWeaponBarellComponent>(TEXT("WeaponBarell"));
	WeaponBarell->SetupAttachment(WeaponMesh, SocketWeaponMuzzle);

	ReticleType = EReticleType::Default;

	EquippedSocketName = SocketCharacterWeapon;
	SetReplicates(true);
}

void ARangeWeaponItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	DOREPLIFETIME_WITH_PARAMS(ARangeWeaponItem, Ammo, Params);
}


void ARangeWeaponItem::BeginPlay()
{
	Super::BeginPlay();

	Ammo.AddZeroed(FireModes.Num());
	for (int i = 0; i < FireModes.Num(); i++)
	{
		Ammo[i] = FireModes[i].MaxAmmo;
	}
	WeaponBarell->SetFireInfo(CurrentFireMode().FireInfo);
	WeaponBarell->CreateProjectilePool();
}

void ARangeWeaponItem::StartFire()
{
	if (GetWorld()->GetTimerManager().IsTimerActive(ShotTimer))
	{
		return;
	}

	bIsFiring = true;
	MakeShot();
}
void ARangeWeaponItem::StopFire()
{
	bIsFiring = false;
}

void ARangeWeaponItem::StopAnimMontage(UAnimMontage* AnimMontage, float BlendOutTime)
{
	UAnimInstance* WeaponAnimInstance = WeaponMesh->GetAnimInstance();
	if (IsValid(WeaponAnimInstance))
	{
		WeaponAnimInstance->Montage_Stop(BlendOutTime, AnimMontage);
	}
}

float ARangeWeaponItem::PlayAnimMontage(UAnimMontage* AnimMontage)
{
	UAnimInstance* WeaponAnimInstance = WeaponMesh->GetAnimInstance();
	float Result = 0.f;
	if (IsValid(WeaponAnimInstance))
	{
		Result = WeaponAnimInstance->Montage_Play(AnimMontage);
	}
	return Result;
}

FTransform ARangeWeaponItem::GetForeGripTransform() const
{
	return WeaponMesh->GetSocketTransform(SocketWeaponForeGrip);
}

float ARangeWeaponItem::GetShotTimerInterval() const
{
	return  60.f / CurrentFireMode().RateOfFire;
}

void ARangeWeaponItem::MakeShot()
{
	ABaseCharacter* CharacterOwner = GetCharacterOwner();
	if (!CharacterOwner)
	{
		return;
	}

	if (!CanShoot())
	{
		if (bIsReloading)
		{
			return;
		}
		StopFire();
		if (Ammo[IndexCurrentFireMode] == 0 && CurrentFireMode().bAutoReload)
		{
			CharacterOwner->Reload();
		}
		return;
	}

	EndReload(false);

	CharacterOwner->PlayAnimMontage(CharacterFireMontage);
	PlayAnimMontage(WeaponFireMontage);


	FVector ShotLocation;
	FRotator ShotRotation;
	if (CharacterOwner->IsPlayerControlled())
	{
		APlayerController* Controller = CharacterOwner->GetController<APlayerController>();
		if (!IsValid(Controller))
		{
			return;
		}

		Controller->GetPlayerViewPoint(ShotLocation, ShotRotation);
	}
	else
	{
		ShotLocation = WeaponBarell->GetComponentLocation();
		ShotRotation = CharacterOwner->GetBaseAimRotation();
	}

	FVector ShotDirection = ShotRotation.RotateVector(FVector::ForwardVector);

	WeaponBarell->Shot(ShotLocation, ShotDirection, GetCurrentBulletSpreadAngle());
	SetAmmo(Ammo[IndexCurrentFireMode] - 1);

	GetWorld()->GetTimerManager().SetTimer(ShotTimer, this, &ARangeWeaponItem::OnShotTimerElapsed, GetShotTimerInterval(), false);
}

void ARangeWeaponItem::StartAim()
{
	bIsAiming = true;
}

void ARangeWeaponItem::StopAim()
{
	bIsAiming = false;
}

float ARangeWeaponItem::GetAimFOV() const
{
	return CurrentFireMode().AimFOV;
}

float ARangeWeaponItem::GetAimMovmentMaxSpeed() const
{
	return AimMovementMaxSpeed;
}

float ARangeWeaponItem::GetCurrentBulletSpreadAngle() const
{
	float AngleInDegrees = bIsAiming ? CurrentFireMode().AimSpreadAngle : CurrentFireMode().SpreadAngle;
	return FMath::DegreesToRadians(AngleInDegrees);
}

float ARangeWeaponItem::GetAimLookUpModifier() const
{
	return AimLookUpModifier;
}

float ARangeWeaponItem::GetAimTurnModifier() const
{
	return AimTurnModifier;
}


int32 ARangeWeaponItem::GetAmmo() const
{
	return Ammo[IndexCurrentFireMode];
}

void ARangeWeaponItem::SetAmmo(int32 NewAmmo)
{
	ABaseCharacter* CharacterOwner = GetCharacterOwner();
	if (CharacterOwner->IsLocallyControlled()) {
		Ammo[IndexCurrentFireMode] = NewAmmo;
		Server_ChangeAmmo(IndexCurrentFireMode, NewAmmo);
		if (OnAmmoChanged.IsBound())
		{
			OnAmmoChanged.Broadcast(Ammo[IndexCurrentFireMode]);
		}
	}
}

bool ARangeWeaponItem::CanShoot() const
{
	return Ammo[IndexCurrentFireMode] > 0;
}

EAmunitionType ARangeWeaponItem::GetAmmoType() const
{
	return CurrentFireMode().AmmoType;
}

int32 ARangeWeaponItem::GetMaxAmmo() const
{
	return CurrentFireMode().MaxAmmo;
}

void ARangeWeaponItem::StartReload()
{
	ReloadAmmo();
	Server_Reload();
}

void ARangeWeaponItem::ReloadAmmo()
{
	ABaseCharacter* CharacterOwner = GetCharacterOwner();
	if (!CharacterOwner)
	{
		return;
	}

	bIsReloading = true;
	if (IsValid(CharacterReloadMontage))
	{
		float MontageDuration = CharacterOwner->PlayAnimMontage(CharacterReloadMontage);
		PlayAnimMontage(WeaponReloadMontage);
		if (CurrentFireMode().ReloadType == EReloadType::FullClip)
		{
			GetWorld()->GetTimerManager().SetTimer(ReloadTimer, [this]() { EndReload(true); }, MontageDuration, false);
		}
	}
	else
	{
		EndReload(true);
	}
}

void ARangeWeaponItem::EndReload(bool bIsSuccess)
{
	EndReloadReplicated(bIsSuccess);
	Server_EndReload(bIsSuccess);
}

void ARangeWeaponItem::EndReloadReplicated(bool bIsSuccess)
{
	if (!bIsReloading)
	{
		return;
	}

	ABaseCharacter* CharacterOwner = GetCharacterOwner();

	if (!bIsSuccess)
	{
		if (CharacterOwner)
		{
			CharacterOwner->StopAnimMontage(CharacterReloadMontage);
		}
		StopAnimMontage(WeaponReloadMontage);
	}

	if (CurrentFireMode().ReloadType == EReloadType::ByBullet)
	{
		UAnimInstance* CharacterAnimInstance = CharacterOwner ? CharacterOwner->GetMesh()->GetAnimInstance() : nullptr;
		if (IsValid(CharacterAnimInstance))
		{
			CharacterAnimInstance->Montage_JumpToSection(SectionMontageReloadEnd, CharacterReloadMontage);
		}

		UAnimInstance* WeaponAnimInstance = WeaponMesh->GetAnimInstance();
		if (IsValid(WeaponAnimInstance))
		{
			WeaponAnimInstance->Montage_JumpToSection(SectionMontageReloadEnd, WeaponReloadMontage);
		}
	}

	GetWorld()->GetTimerManager().ClearTimer(ReloadTimer);

	bIsReloading = false;
	if (bIsSuccess && OnReloadComplete.IsBound())
	{
		OnReloadComplete.Broadcast();
	}
}

void ARangeWeaponItem::OnShotTimerElapsed()
{
	if (!bIsFiring)
	{
		return;
	}

	if (CurrentFireMode().AmmoType == EAmunitionType::RifleGrenades)
	{
		StartReload();
	}

	switch (CurrentFireMode().WeaponFireMode)
	{
		case EWeaponFireMode::Single:
		{
			StopFire();
			break;
		}
		case EWeaponFireMode::FullAuto:
		{
			MakeShot();
		}
	}
}

EReticleType ARangeWeaponItem::GetReticleType() const
{
	return  bIsAiming ? AimReticleType : ReticleType;
}


void ARangeWeaponItem::ChangeFireMode()
{
	if (FireModes.Num() == 1)
	{
		return;
	}
	IndexCurrentFireMode += 1;
	if (IndexCurrentFireMode == FireModes.Num())
	{
		IndexCurrentFireMode = 0;
	}
	WeaponBarell->SetFireInfo(CurrentFireMode().FireInfo);
}

void ARangeWeaponItem::Server_Reload_Implementation()
{
	Multicast_Reload();
}

void ARangeWeaponItem::Multicast_Reload_Implementation()
{
	ABaseCharacter* CharacterOwner = GetCharacterOwner();
	if (CharacterOwner->IsLocallyControlled())
	{
		return;
	}
	ReloadAmmo();
}

void ARangeWeaponItem::Server_ChangeAmmo_Implementation(int32 Index, int32 Value)
{
	Ammo[Index] = Value;
	Multicast_ChangeAmmo(Index, Value);
}

void ARangeWeaponItem::Multicast_ChangeAmmo_Implementation(int32 Index, int32 Value)
{
	ABaseCharacter* CharacterOwner = GetCharacterOwner();
	if (CharacterOwner->IsLocallyControlled())
	{
		return;
	}
	Ammo[Index] = Value;
}

void ARangeWeaponItem::Server_EndReload_Implementation(bool bIsSuccess)
{
	Multicast_EndReload(bIsSuccess);
}

void ARangeWeaponItem::Multicast_EndReload_Implementation(bool bIsSuccess)
{
	ABaseCharacter* CharacterOwner = GetCharacterOwner();
	if (CharacterOwner->IsLocallyControlled())
	{
		return;
	}
	EndReloadReplicated(bIsSuccess);
}

