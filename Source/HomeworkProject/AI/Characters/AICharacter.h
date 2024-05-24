// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "AICharacter.generated.h"

class UAIPatrollingComponent;
class UBehaviorTree;
/**
 * 
 */
UCLASS(Blueprintable)
class HOMEWORKPROJECT_API AAICharacter : public ABaseCharacter
{
	GENERATED_BODY()
public:
	AAICharacter(const FObjectInitializer& ObjectInitializer);

	UAIPatrollingComponent* GetPatrollingComponent() const;

	UBehaviorTree* GetBehaviorTree() const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAIPatrollingComponent* AIPatrolingComponent;

	UPROPERTY(EditAnywhare, BlueprintReadOnly, Category = "AI")
	UBehaviorTree* BehaviorTree;
};
