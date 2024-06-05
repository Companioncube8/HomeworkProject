// Fill out your copyright notice in the Description page of Project Settings.


#include "AIPatrolingComponent.h"
#include "Actors/Navigation/PatrolingPath.h"

bool UAIPatrollingComponent::CanPatrol() const
{
	return PatrolingPathInfo.PatrolingPath && PatrolingPathInfo.PatrolingPath->GetWayPoints().Num() > 0;
}

FVector UAIPatrollingComponent::SelectClosestWaypoint()
{
	FVector OwnerLocation = GetOwner()->GetActorLocation();
	const TArray<FVector>& WayPoints = PatrolingPathInfo.PatrolingPath->GetWayPoints();
	FTransform PathTransform = PatrolingPathInfo.PatrolingPath->GetActorTransform();

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
	const TArray<FVector>& WayPoints = PatrolingPathInfo.PatrolingPath->GetWayPoints();

	if (WayPoints.Num() == 1)
	{
		CurrentWayPointIndex = 0;
		FTransform PathTransform = PatrolingPathInfo.PatrolingPath->GetActorTransform();
		return PathTransform.TransformPosition(WayPoints[CurrentWayPointIndex]);
	}

	CurrentWayPointIndex += bIsNeedGoBack ? -1 : 1;

	switch (PatrolingPathInfo.Type)
	{
	case EPatrolingPathType::Circle:
		if (CurrentWayPointIndex == WayPoints.Num())
		{
			CurrentWayPointIndex = 0;
		}
		break;
	case EPatrolingPathType::PingPong:
		if (CurrentWayPointIndex == WayPoints.Num())
		{
			bIsNeedGoBack = true;
			CurrentWayPointIndex = WayPoints.Num() - 2;
		} else if (CurrentWayPointIndex == -1)
		{
			bIsNeedGoBack = false;
			CurrentWayPointIndex = 1;
		}
		break;
	}
	FTransform PathTransform = PatrolingPathInfo.PatrolingPath->GetActorTransform();
	FVector WayPoint = PathTransform.TransformPosition(WayPoints[CurrentWayPointIndex]);
	return WayPoint;
}
