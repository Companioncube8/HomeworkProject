// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Controllers/GCAIController.h"

#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Damage.h"

AGCAIController::AGCAIController()
{
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
}

AActor* AGCAIController::GetClosestSensedActor(TSubclassOf<UAISense> SenseClass) const
{
	if (!GetPawn())
	{
		return nullptr;
	}

	TArray<AActor*> SensedActors;
	PerceptionComponent->GetCurrentlyPerceivedActors(SenseClass, SensedActors);

	AActor* ClosestActor = nullptr;
	float MinSquaredDistance = FLT_MAX;

	FVector PawnLocation = GetPawn()->GetActorLocation();

	for (AActor* Actor : SensedActors)
	{
		float CurrentSquaredDistance = (PawnLocation - Actor->GetActorLocation()).SizeSquared();
		if (CurrentSquaredDistance < MinSquaredDistance)
		{
			MinSquaredDistance = CurrentSquaredDistance;
			ClosestActor = Actor;
		}
	}
	return ClosestActor;
}