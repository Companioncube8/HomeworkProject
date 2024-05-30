// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Controllers/AITurretController.h"

#include "AI/Characters/Turret.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Damage.h"

void AAITurretController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
	if (InPawn)
	{
		checkf(InPawn->IsA<ATurret>(), TEXT("AAITurretController::SetPawn AAITurretController can possess only turrets"));
		CachedTurret = StaticCast<ATurret*>(InPawn);
	} else
	{
		CachedTurret = nullptr;
	}
}

void AAITurretController::ActorsPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	Super::ActorsPerceptionUpdated(UpdatedActors);
	if (!CachedTurret.IsValid())
	{
		return;
	}


	AActor* ClosestActor = GetClosestSensedActor(UAISense_Sight::StaticClass());
	if (AActor* DamageActor = GetClosestSensedActor(UAISense_Damage::StaticClass()))
	{
		ClosestActor = DamageActor;
	}
	CachedTurret->CurrentTarget = ClosestActor;
	CachedTurret->OnCurrentTargetSet();
}
