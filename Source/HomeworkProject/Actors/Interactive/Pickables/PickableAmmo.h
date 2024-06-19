// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Interactive/Pickables/PickableItem.h"
#include "PickableAmmo.generated.h"

class ABaseCharacter;
/**
 * 
 */
UCLASS(Blueprintable)
class HOMEWORKPROJECT_API APickableAmmo : public APickableItem
{
	GENERATED_BODY()
public:
	APickableAmmo();
	virtual void Interact(ABaseCharacter* Character) override;
	virtual FName GetActionEventName() const override;
	void SetCount(int32 NewCount);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* WeaponMesh;

	UPROPERTY(EditAnywhere)
	int32 Count;
};
