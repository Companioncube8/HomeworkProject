// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AIPatrolingComponent.generated.h"

class APatrolingPath;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HOMEWORKPROJECT_API UAIPatrollingComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	bool CanPatrol() const;
	FVector SelectClosestWaypoint();
	FVector SelectNextWaypoint();

protected:
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Path")
	APatrolingPath* PatrolingPath;

private:
	int32 CurrentWayPointIndex = -1;

};
