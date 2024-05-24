// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AIPatrolingComponent.generated.h"

class APatrolingPath;

UENUM(BlueprintType)
enum class EPatrolingPathType : uint8
{
	PingPong,
	Circle
};

USTRUCT(BlueprintType)
struct FPatrolingPathInfo
{
	GENERATED_BODY()

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Path")
	EPatrolingPathType Type = EPatrolingPathType::Circle;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Path")
	APatrolingPath* PatrolingPath;
};

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
	FPatrolingPathInfo PatrolingPathInfo;
private:
	int32 CurrentWayPointIndex = -1;
	bool bIsNeedGoBack = false;

};
