// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Equipment/Throwables/ThrowableItem.h"

#include "Actors/Projectiles/Projectile.h"
#include "Characters/BaseCharacter.h"
#include "Net/UnrealNetwork.h"

AThrowableItem::AThrowableItem()
{
	SetReplicates(true);
}

void AThrowableItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AThrowableItem, ProjectilePool);

	FDoRepLifetimeParams RepLifetimeParams;
	RepLifetimeParams.Condition = COND_SimulatedOnly;
	RepLifetimeParams.RepNotifyCondition = REPNOTIFY_Always;
	DOREPLIFETIME_WITH_PARAMS(AThrowableItem, LastThrowInfo, RepLifetimeParams);
}



void AThrowableItem::Throw()
{
	ABaseCharacter* CharacterOwner = GetCharacterOwner();
	if (!CharacterOwner)
	{
		return;
	}
	APlayerController* Controller = CharacterOwner->GetController<APlayerController>();
	if (!IsValid(Controller))
	{
		return;
	}

	FVector PlayerViewPoint;
	FRotator PlayerViewRotation;

	Controller->GetPlayerViewPoint(PlayerViewPoint, PlayerViewRotation);

	FTransform PlayerViewTransform(PlayerViewRotation, PlayerViewPoint);

	FVector ViewDirection = PlayerViewRotation.RotateVector(FVector::ForwardVector);
	FVector ViewUpVector = PlayerViewRotation.RotateVector(FVector::UpVector);

	FVector LaunchDirection = ViewDirection + FMath::Tan(FMath::DegreesToRadians(ThrowAngle)) * ViewUpVector;

	FVector ThrowableSocketLocation = CharacterOwner->GetMesh()->GetSocketLocation(SocketCharacterThrowable);
	FVector SocketInViewSpace = PlayerViewTransform.InverseTransformPosition(ThrowableSocketLocation);

	FVector SpawnLocation = PlayerViewPoint + ViewDirection * SocketInViewSpace.X;

	FThrowInfo ThrowInfo = FThrowInfo(SpawnLocation, LaunchDirection);

	if (CharacterOwner->GetLocalRole() == ROLE_AutonomousProxy)
	{
		Server_Throw(ThrowInfo);
	}
	SetProjectile(ThrowInfo);
}

void AThrowableItem::CreateProjectilePool()
{
	if (!GetOwner())
	{
		return;
	}

	if (GetOwner()->GetLocalRole() < ROLE_Authority)
	{
		return;
	}

	ProjectilePool.Reserve(ThrowablePoolSize);

	for (int32 i = 0; i < ThrowablePoolSize; ++i)
	{
		AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, ProjectilePoolLocation, FRotator::ZeroRotator);
		Projectile->SetOwner(GetOwner());
		ProjectilePool.Add(Projectile);
	}
}

void AThrowableItem::SetProjectile(const FThrowInfo& ThrowInfo)
{
	if (GetCharacterOwner()->GetLocalRole() == ROLE_Authority)
	{
		LastThrowInfo = ThrowInfo;
	}
	AProjectile* Projectile = ProjectilePool[CurrentProjectileIndex];
	++CurrentProjectileIndex;
	if (CurrentProjectileIndex == ProjectilePool.Num())
	{
		CurrentProjectileIndex = 0;
	}
	Projectile->SetOwner(GetOwner());
	Projectile->SetActorLocation(ThrowInfo.GetLocation());
	Projectile->SetActorRotation(ThrowInfo.GetDirection().ToOrientationRotator());
	Projectile->LaunchProjectile(ThrowInfo.GetDirection().GetSafeNormal());
	Projectile->SetProjectileActive(true);
}

void AThrowableItem::Server_Throw_Implementation(const FThrowInfo& ThrowInfo)
{
	SetProjectile(ThrowInfo);
}

void AThrowableItem::OnRep_LastThrowInfo()
{
	SetProjectile(LastThrowInfo);
}
