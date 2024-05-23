// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Characters/AICharacter.h"

#include "Components/CharacterComponents/AIPatrolingComponent.h"

AAICharacter::AAICharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AIPatrolingComponent = CreateDefaultSubobject<UAIPatrollingComponent>(TEXT("AIPatroling"));
}

UAIPatrollingComponent* AAICharacter::GetPatrollingComponent() const
{
	return AIPatrolingComponent;
}
