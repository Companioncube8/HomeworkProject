// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Interactive/Interface/Interactive.h"
#include "Actors/Projectiles/Projectile.h"
#include "ArrowProjectile.generated.h"

/**
 * 
 */
UCLASS()
class HOMEWORKPROJECT_API AArrowProjectile : public AProjectile, public IInteractable
{
	GENERATED_BODY()
public:
	AArrowProjectile();

	virtual void Interact(ABaseCharacter* Character) override;
	virtual FName GetActionEventName() const override;
private:
	UPROPERTY(EditAnywhere)
	FName ItemID = NAME_None;
	
};
