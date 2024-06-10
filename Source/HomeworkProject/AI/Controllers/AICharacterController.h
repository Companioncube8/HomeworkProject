// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/Characters/AICharacter.h"
#include "AI/Controllers/GCAIController.h"
#include "AICharacterController.generated.h"

/**
 * 
 */
UCLASS()
class HOMEWORKPROJECT_API AAICharacterController : public AGCAIController
{
	GENERATED_BODY()
public:
	virtual void SetPawn(APawn* InPawn) override;

	virtual void ActorsPerceptionUpdated(const TArray<AActor*>& UpdatedActors) override;

	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float TargetReachRadius = 100.f;

private:
	void TryMoveToNextTarget();

	bool IsTargetReached(FVector TargetLocation) const;

	TWeakObjectPtr<AAICharacter> CachedAICharacter;

	bool bIsPatroling = false;

	void SetupPatrolling();
};
