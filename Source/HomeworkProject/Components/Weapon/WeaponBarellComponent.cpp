// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Weapon/WeaponBarellComponent.h"

#include "DrawDebugHelpers.h"
#include "HomeworkProjectTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystems/DebugSubsystem.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Actors/Projectiles/ArrowProjectile.h"
#include "Actors/Projectiles/Projectile.h"
#include "Components/DecalComponent.h"
<<<<<<< Updated upstream
=======
#include "Net/UnrealNetwork.h"

UWeaponBarellComponent::UWeaponBarellComponent()
{
	SetIsReplicatedByDefault(true);
}

void UWeaponBarellComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	FDoRepLifetimeParams RepLifetimeParams;
	RepLifetimeParams.Condition = COND_SimulatedOnly;
	RepLifetimeParams.RepNotifyCondition = REPNOTIFY_Always;
	DOREPLIFETIME_WITH_PARAMS(UWeaponBarellComponent, LastShotsInfo, RepLifetimeParams);
	DOREPLIFETIME(UWeaponBarellComponent, ProjectilePool);
	DOREPLIFETIME(UWeaponBarellComponent, CurrentProjectileIndex);
	DOREPLIFETIME(UWeaponBarellComponent, LoadedProjectile);
}

void UWeaponBarellComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UWeaponBarellComponent::CreateProjectilePool()
{
	if (!FireInfo.NeedCreatePoolProjectile)
	{
		return;
	}
	if (GetOwnerRole() < ROLE_Authority)
	{
		return;
	}

	if (!FireInfo.ProjectileClass)
	{
		return;
	}

	ProjectilePool.Reserve(ProjectilePoolSize);

	for (int32 i = 0; i < ProjectilePoolSize; ++i)
	{
		AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(FireInfo.ProjectileClass, ProjectilePoolLocation, FRotator::ZeroRotator);
		Projectile->SetOwner(GetOwningPawn());
		Projectile->SetProjectileActive(false);
		ProjectilePool.Add(Projectile);
	}
}



>>>>>>> Stashed changes

bool UWeaponBarellComponent::HitScan(FVector ShotStart, FVector ShotDirection, FVector& ShotEnd)
{
	FHitResult ShotResult;
	bool bHasHit = GetWorld()->LineTraceSingleByChannel(ShotResult, ShotStart, ShotEnd, ECC_Bullet);
	if (bHasHit)
	{
		ShotEnd = ShotResult.ImpactPoint;
		ProcessHit(ShotResult, ShotDirection);
	}
	return bHasHit;
}

void UWeaponBarellComponent::Shot(FVector ShotStart, FVector ShotDirection, float SpreadAngle)
{
	FVector MuzzleLocation = GetComponentLocation();
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), MuzzleFlashFX, MuzzleLocation, GetComponentRotation());
	for (int i = 0; i < FireInfo.BulletsPerShot; i++)
	{
		ShotDirection += GetBulletSpreadOffset(FMath::RandRange(0.f, SpreadAngle), ShotDirection.ToOrientationRotator());
		FVector ShotEnd = ShotStart + FireInfo.FiringRange * ShotDirection;

#if ENABLE_DRAW_DEBUG
		UDebugSubsystem* DebugSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UDebugSubsystem>();
		bool bIsDebugEnabled = DebugSubsystem->IsCategoryEnabled(DebugCategoryRangeWeapon);

#else
		bool bIsDebugEnabled = false;
#endif

		switch (FireInfo.HitRegistration)
		{
			case EHitRegistrationType::HitScan:
			{
				bool bHasHit = HitScan(ShotStart, ShotDirection, ShotEnd);
				if (bIsDebugEnabled && bHasHit) {
					DrawDebugSphere(GetWorld(), ShotEnd, 10.0f, 24, FColor::Red, false, 1.f);
				}
				break;
			}
			case EHitRegistrationType::Projectile:
			{
				LaunchProjectile(ShotStart, ShotDirection);
				break;
			}
		}


		UNiagaraComponent* TraceFXComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), TraceFX, MuzzleLocation, GetComponentRotation());
		TraceFXComponent->SetVectorParameter(FXParamTraceEnd, ShotEnd);

		if (bIsDebugEnabled) {
			DrawDebugLine(GetWorld(), MuzzleLocation, ShotEnd, FColor::Red, false, 1.f, 0, 3.f);
		}
	}
}


FVector UWeaponBarellComponent::GetBulletSpreadOffset(float Angle, FRotator ShotRotation) const
{
	float SpreadSize = FMath::Tan(Angle);
	float RotationAngle = FMath::RandRange(0.f, 2 * PI);

	float SpreadY = FMath::Cos(RotationAngle);
	float SpreadZ = FMath::Sin(RotationAngle);

	FVector Result = (ShotRotation.RotateVector(FVector::UpVector) * SpreadZ + ShotRotation.RotateVector(FVector::RightVector) * SpreadY) * SpreadSize;

	return Result;
}

APawn* UWeaponBarellComponent::GetOwningPawn() const
{
	APawn* PawnOwner = Cast<APawn>(GetOwner());
	if (IsValid(PawnOwner))
	{
		PawnOwner = Cast<APawn>(GetOwner()->GetOwner());
	}
	return PawnOwner;
}

AController* UWeaponBarellComponent::GetController() const
{
	APawn* PawnOwner = GetOwningPawn();
	return IsValid(PawnOwner) ? PawnOwner->GetController() : nullptr;
}

AProjectile* UWeaponBarellComponent::GetProjectile()
{
	if (LoadedProjectile)
	{
		return LoadedProjectile;
	}

	if (!FireInfo.ProjectileClass)
	{
		return nullptr;
	}

	if (!FireInfo.NeedCreatePoolProjectile)
	{
		if (GetOwner()->GetLocalRole() < ROLE_Authority) {
			return nullptr;
		}
		AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(FireInfo.ProjectileClass);
		LoadedProjectile = Projectile;
		return Projectile;
	}
	else
	{
		return ProjectilePool[CurrentProjectileIndex];
	}
}


void UWeaponBarellComponent::LaunchProjectile(const FVector& LaunchStart, const FVector& LaunchDirection)
{
<<<<<<< Updated upstream
	if (AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(FireInfo.ProjectileClass, LaunchStart, LaunchDirection.ToOrientationRotator()))
	{
		Projectile->SetOwner(GetOwningPawn());
		Projectile->OnProjectileHit.AddDynamic(this, &UWeaponBarellComponent::ProcessHit);
		Projectile->LaunchProjectile(LaunchDirection.GetSafeNormal());
=======

	if (!FireInfo.NeedCreatePoolProjectile)
	{
		if (IsValid(LoadedProjectile))
		{
			LoadedProjectile->SetActorLocation(LaunchStart);
			LoadedProjectile->SetActorRotation(LaunchDirection.ToOrientationRotator());
			LoadedProjectile->SetProjectileActive(true);
			LoadedProjectile->SetCollision(ECollisionEnabled::QueryOnly);
			LoadedProjectile->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			LoadedProjectile->OnProjectileHit.AddDynamic(this, &UWeaponBarellComponent::ProcessProjectileHit);
			LoadedProjectile->LaunchProjectile(LaunchDirection.GetSafeNormal());
			LoadedProjectile = nullptr;
		} 
	} else
	{
		AProjectile* Projectile = ProjectilePool[CurrentProjectileIndex];
		Projectile->SetActorLocation(LaunchStart);
		Projectile->SetActorRotation(LaunchDirection.ToOrientationRotator());
		Projectile->SetProjectileActive(true);
		Projectile->OnProjectileHit.AddDynamic(this, &UWeaponBarellComponent::ProcessProjectileHit);
		Projectile->LaunchProjectile(LaunchDirection.GetSafeNormal());
		++CurrentProjectileIndex;
		if (CurrentProjectileIndex == ProjectilePool.Num())
		{
			CurrentProjectileIndex = 0;
		}
>>>>>>> Stashed changes
	}
}

void UWeaponBarellComponent::ProcessHit(const FHitResult& HitResult, const FVector& Direction)
{
	AActor* HitActor = HitResult.GetActor();
	if (IsValid(HitActor))
	{
		float DamageCoef = 1;
		if (FalloffDiagram != nullptr) {
			DamageCoef = FalloffDiagram->GetFloatValue(HitResult.Distance);
		}
		FPointDamageEvent DamageEvent;
		DamageEvent.HitInfo = HitResult;
		DamageEvent.ShotDirection = Direction;
		DamageEvent.DamageTypeClass = DamageTypeClass;
		HitActor->TakeDamage( FireDamage * DamageCoef, DamageEvent, GetController(), GetOwner());
	}

	UDecalComponent* DecalComponent = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), DefaultDecalInfo.DecalMaterial, DefaultDecalInfo.DecalSize, HitResult.ImpactPoint, HitResult.ImpactNormal.ToOrientationRotator());
	if (IsValid(DecalComponent))
	{
		DecalComponent->SetFadeOut(DefaultDecalInfo.DecalLifeTime, DefaultDecalInfo.DecalFadeOutTime);
		DecalComponent->SetFadeScreenSize(0.0001f);
	}
}
<<<<<<< Updated upstream
=======

void UWeaponBarellComponent::ShotInternal(const TArray<FShotInfo>& ShotsInfos)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(TEXT("UWeaponBarellComponent::ShotInternal"));
	if (GetOwner()->HasAuthority())
	{
		LastShotsInfo = ShotsInfos;
	}
	FVector MuzzleLocation = GetComponentLocation();
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), MuzzleFlashFX, MuzzleLocation, GetComponentRotation());
	for (const FShotInfo& ShotInfo : ShotsInfos)
	{
		FVector ShotStart = ShotInfo.GetLocation();
		FVector ShotDirection = ShotInfo.GetDirection();
		FVector ShotEnd = ShotStart + FireInfo.FiringRange * ShotDirection;

#if ENABLE_DRAW_DEBUG
		UDebugSubsystem* DebugSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UDebugSubsystem>();
		bool bIsDebugEnabled = DebugSubsystem->IsCategoryEnabled(DebugCategoryRangeWeapon);

#else
		bool bIsDebugEnabled = false;
#endif

		switch (FireInfo.HitRegistration)
		{
		case EHitRegistrationType::HitScan:
		{
			bool bHasHit = HitScan(ShotStart, ShotDirection, ShotEnd);
			if (bIsDebugEnabled && bHasHit) {
				DrawDebugSphere(GetWorld(), ShotEnd, 10.0f, 24, FColor::Red, false, 1.f);
			}
			break;
		}
		case EHitRegistrationType::Projectile:
		{
			LaunchProjectile(ShotStart, ShotDirection);
			break;
		}
		}


		UNiagaraComponent* TraceFXComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), TraceFX, MuzzleLocation, GetComponentRotation());
		if (TraceFXComponent)
		{
			TraceFXComponent->SetVectorParameter(FXParamTraceEnd, ShotEnd);
		}

		if (bIsDebugEnabled) {
			DrawDebugLine(GetWorld(), MuzzleLocation, ShotEnd, FColor::Red, false, 1.f, 0, 3.f);
		}
	}
}

void UWeaponBarellComponent::Server_Shot_Implementation(const TArray<FShotInfo>& ShotsInfos)
{
	ShotInternal(ShotsInfos);
}

void UWeaponBarellComponent::OnRep_LoadedProjectile(AProjectile* OldProjectile)
{
}

void UWeaponBarellComponent::OnRep_LastShotsInfo()
{
	ShotInternal(LastShotsInfo);
}

void UWeaponBarellComponent::ProcessProjectileHit(AProjectile* Projectile, const FHitResult& HitResult, const FVector& Direction)
{
	if (FireInfo.NeedCreatePoolProjectile) {
		Projectile->SetProjectileActive(false);
		Projectile->SetActorLocation(ProjectilePoolLocation);
		Projectile->SetActorRotation(FRotator::ZeroRotator);
		Projectile->OnProjectileHit.RemoveAll(this);
	}
	if (AActor* Actor = HitResult.GetActor()) {
		Projectile->AttachToActor(Actor, FAttachmentTransformRules::KeepWorldTransform);
	}
	ProcessHit(HitResult, Direction);
}


void UWeaponBarellComponent::RemoveLoadedProjectile()
{
	if (LoadedProjectile) {
		LoadedProjectile->Destroy();
		LoadedProjectile = nullptr;
	}
}
>>>>>>> Stashed changes
