// Fill out your copyright notice in the Description page of Project Settings.


#include "AIPatrolingComponent.h"
#include "Actors/Navigation/PatrolingPath.h"

bool UAIPatrollingComponent::CanPatrol() const
{
	return PatrolingPath && PatrolingPath->GetWayPoints().Num() > 0;
}

FVector UAIPatrollingComponent::SelectClosestWaypoint()
{
	FVector OwnerLocation = GetOwner()->GetActorLocation();
	const TArray<FVector> WayPoints = PatrolingPath->GetWayPoints();
	FTransform PathTransform = PatrolingPath->GetActorTransform();

	FVector ClosestWayPoint;
	float MinSqDistance = FLT_MAX;
	for (int32 i = 0; i < WayPoints.Num(); ++i)
	{
		FVector WayPointWorld = PathTransform.TransformPosition(WayPoints[i]);
		float CurrentSqDistance = (OwnerLocation - WayPointWorld).SizeSquared();
		if (CurrentSqDistance < MinSqDistance)
		{
			MinSqDistance = CurrentSqDistance;
			ClosestWayPoint = WayPointWorld;
			CurrentWayPointIndex = i;
		}
	}
	return ClosestWayPoint;
}

FVector UAIPatrollingComponent::SelectNextWaypoint()
{
	++CurrentWayPointIndex;

	const TArray<FVector> WayPoints = PatrolingPath->GetWayPoints();
	if (CurrentWayPointIndex == WayPoints.Num())
	{
		CurrentWayPointIndex = 0;
	}
	FTransform PathTransform = PatrolingPath->GetActorTransform();
	FVector WayPoint = PathTransform.TransformPosition(WayPoints[CurrentWayPointIndex]);
	return WayPoint;
}
