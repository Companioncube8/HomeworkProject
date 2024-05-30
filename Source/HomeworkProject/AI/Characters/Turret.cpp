// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Characters/Turret.h"

#include <Kismet/GameplayStatics.h>
#include <Perception/AISense_Damage.h>

#include "AIController.h"
#include "Components/Weapon/WeaponBarellComponent.h"
#include "Net/UnrealNetwork.h"
// Sets default values
ATurret::ATurret()
{
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* TurretRoot = CreateDefaultSubobject<USceneComponent>(TEXT("TurretRoot"));
	SetRootComponent(TurretRoot);

	TurretBaseComponent = CreateDefaultSubobject<USceneComponent>(TEXT("TurretBase"));
	TurretBaseComponent->SetupAttachment(TurretRoot);

	TurretBarellComponent = CreateDefaultSubobject<USceneComponent>(TEXT("TurretBarrel"));
	TurretBarellComponent->SetupAttachment(TurretBaseComponent);
	
	WeaponBarrel = CreateDefaultSubobject<UWeaponBarellComponent>(TEXT("WeaponBarell"));
	WeaponBarrel->SetupAttachment(TurretBarellComponent);

	Health = MaxHealth;

	this->OnTakeAnyDamage.AddDynamic(this, &ATurret::OnTakeAnyDamageEvent);
	SetReplicates(true);
}

void ATurret::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATurret, CurrentTarget);
}


void ATurret::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	AAIController* AIController = Cast<AAIController>(NewController);
	if (AIController)
	{
		FGenericTeamId TeamId((uint8)Team);
		AIController->SetGenericTeamId(TeamId);
	}
}

// Called every frame
void ATurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (CurrentTurretState)
	{
		case ETurretState::Searching:
		{
			SearchingMovement(DeltaTime);
			break;
		}
		case ETurretState::Firing:
		{
			FiringMovement(DeltaTime);
			break;
		}
	}

}

void ATurret::OnCurrentTargetSet()
{
	ETurretState NewState = CurrentTarget ? ETurretState::Firing : ETurretState::Searching;
	SetCurrentTurretState(NewState);
}

void ATurret::SetCurrentTurretState(ETurretState NewState)
{
	bool bIsStateChanged = NewState != CurrentTurretState;
	CurrentTurretState = NewState;
	if (!bIsStateChanged)
	{
		return;
	}

	switch (CurrentTurretState)
	{
	case ETurretState::Firing:
		GetWorld()->GetTimerManager().SetTimer(ShotTimer, this, &ATurret::MakeShot, GetFireInterval(), true, FireDelayTime);
		break;
	case ETurretState::Searching:
		GetWorld()->GetTimerManager().ClearTimer(ShotTimer);
		break;
	}
}

void ATurret::SearchingMovement(float DeltaTime)
{
	FRotator TurretBaseRotation = TurretBaseComponent->GetRelativeRotation();
	TurretBaseRotation.Yaw += DeltaTime * BaseSearchingRotationRate;
	TurretBaseComponent->SetRelativeRotation(TurretBaseRotation);

	FRotator TurretBarellRotation = TurretBarellComponent->GetRelativeRotation();
	TurretBarellRotation.Pitch = FMath::FInterpTo(TurretBarellRotation.Pitch, 0.f, DeltaTime, BarellPitchRotationRate);
	TurretBarellComponent->SetRelativeRotation(TurretBarellRotation);
}

void ATurret::FiringMovement(float DeltaTime)
{
	FVector BaseLookAtDirection = (CurrentTarget->GetActorLocation() - TurretBaseComponent->GetComponentLocation()).GetSafeNormal2D();
	FQuat LookAtQuat = BaseLookAtDirection.ToOrientationQuat();
	FQuat TargetQuat = FMath::QInterpTo(TurretBaseComponent->GetComponentQuat(), LookAtQuat, DeltaTime, BaseFiringInterpSpeed);
	TurretBaseComponent->SetWorldRotation(TargetQuat);

	FVector BarellLookAtDirection = (CurrentTarget->GetActorLocation() - TurretBarellComponent->GetComponentLocation()).GetSafeNormal();
	float LookAtPitchAngle = BarellLookAtDirection.ToOrientationRotator().Pitch;

	FRotator BarellLocalRotation = TurretBarellComponent->GetRelativeRotation();
	BarellLocalRotation.Pitch = FMath::FInterpTo(BarellLocalRotation.Pitch, LookAtPitchAngle, DeltaTime, BarellPitchRotationRate);

	TurretBarellComponent->SetRelativeRotation(BarellLocalRotation);
}

FVector ATurret::GetPawnViewLocation() const
{
	return WeaponBarrel->GetComponentLocation();
}

FRotator ATurret::GetViewRotation() const
{
	return WeaponBarrel->GetComponentRotation();
}

float ATurret::GetFireInterval() const
{
	return 60.f / RateOfFire;
}

void ATurret::MakeShot()
{
	FVector ShotLocation = WeaponBarrel->GetComponentLocation();
	FVector ShotDirection = WeaponBarrel->GetComponentRotation().RotateVector(FVector::ForwardVector);
	float SpreadAngle = FMath::DegreesToRadians(BulletSpreadAngle);
	WeaponBarrel->Shot(ShotLocation, ShotDirection, SpreadAngle);
}

void ATurret::OnTakeAnyDamageEvent(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);
	UAISense_Damage::ReportDamageEvent(GetWorld(), DamagedActor, DamageCauser->GetOwner(), Damage, DamageCauser->GetOwner()->GetActorLocation(), GetActorLocation());

	if (FMath::IsNearlyZero(Health))
	{
		if (ExplosionVFX)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionVFX, GetActorLocation());
		}
		CurrentTurretState = ETurretState::Searching;

		if (OnExplode.IsBound())
		{
			OnExplode.Broadcast();
		}
	}
}

void ATurret::OnRep_CurrentTarget()
{
	OnCurrentTargetSet();
}
