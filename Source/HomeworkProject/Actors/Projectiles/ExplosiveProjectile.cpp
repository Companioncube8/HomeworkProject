// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Projectiles/ExplosiveProjectile.h"
#include "Components/Weapon/ExplosionComponent.h"

AExplosiveProjectile::AExplosiveProjectile()
{
	ExplosionComponent = CreateDefaultSubobject<UExplosionComponent>("ExplosionComponent");
	ExplosionComponent->SetupAttachment(GetRootComponent());
}

void AExplosiveProjectile::OnProjectileLaunch()
{
	Super::OnProjectileLaunch();
	GetWorld()->GetTimerManager().SetTimer(DetonationTimer, this, &AExplosiveProjectile::OnDetonationTimerElapsed, DetonationTime, false);
}

void AExplosiveProjectile::OnDetonationTimerElapsed()
{
	ExplosionComponent->Explode(GetController());
}

AController* AExplosiveProjectile::GetController()
{
	APawn* PawnOwner = Cast<APawn>(GetOwner());
	return  IsValid(PawnOwner) ? PawnOwner->GetController() : nullptr;
}