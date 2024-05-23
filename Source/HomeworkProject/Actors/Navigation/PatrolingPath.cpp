// Fill out your copyright notice in the Description page of Project Settings.


#include "PatrolingPath.h"

const TArray<FVector>& APatrolingPath::GetWayPoints() const
{
	return WayPoints;
}
