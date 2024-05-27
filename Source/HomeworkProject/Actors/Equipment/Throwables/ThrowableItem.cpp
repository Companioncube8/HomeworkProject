// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Equipment/Throwables/ThrowableItem.h"

#include "Actors/Projectiles/Projectile.h"
#include "Characters/BaseCharacter.h"

void AThrowableItem::Throw()
{
	ABaseCharacter* CharacterOwner = GetCharacterOwner();
	if (!CharacterOwner)
	{
		return;
	}


	FVector PlayerViewPoint;
	FRotator PlayerViewRotation;

	if (CharacterOwner->IsPlayerControlled())
	{
		APlayerController* Controller = CharacterOwner->GetController<APlayerController>();
		if (!IsValid(Controller))
		{
			return;
		}


		Controller->GetPlayerViewPoint(PlayerViewPoint, PlayerViewRotation);
	} else
	{
		PlayerViewPoint = CharacterOwner->GetMesh()->GetSocketLocation(SocketCharacterThrowable);
		PlayerViewRotation = CharacterOwner->GetBaseAimRotation();
	}

	FTransform PlayerViewTransform(PlayerViewRotation, PlayerViewPoint);

	FVector ViewDirection = PlayerViewRotation.RotateVector(FVector::ForwardVector);
	FVector ViewUpVector = PlayerViewRotation.RotateVector(FVector::UpVector);

	FVector LaunchDirection = ViewDirection + FMath::Tan(FMath::DegreesToRadians(ThrowAngle)) * ViewUpVector;

	FVector ThrowableSocketLocation = CharacterOwner->GetMesh()->GetSocketLocation(SocketCharacterThrowable);
	FVector SocketInViewSpace = PlayerViewTransform.InverseTransformPosition(ThrowableSocketLocation);

	FVector SpawnLocation = PlayerViewPoint + ViewDirection * SocketInViewSpace.X;

	if (AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, SpawnLocation, FRotator::ZeroRotator))
	{
		Projectile->SetOwner(GetOwner());
		Projectile->LaunchProjectile(LaunchDirection.GetSafeNormal());
	}
}
