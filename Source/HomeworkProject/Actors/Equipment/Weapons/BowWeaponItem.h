// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Equipment/Weapons/RangeWeaponItem.h"
#include "Components/TimelineComponent.h"
#include "BowWeaponItem.generated.h"

/**
 * 
 */


UCLASS()
class HOMEWORKPROJECT_API ABowWeaponItem : public ARangeWeaponItem
{
	GENERATED_BODY()
protected:
	ABowWeaponItem();

	virtual void Tick(float DeltaSeconds) override;
	
	virtual void StopAim() override;

	virtual void EndReload(bool bIsSuccess) override;

	virtual float GetCurrentBulletSpreadAngle() const override;

	virtual void StartFire() override;

	void StartStringTense();
	void StopStringTense();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters | Bow")
	float FullStringTensionTime = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters | Bow")
	float StringTensionStartTime = 0.2f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters | Bow")
	float ArrowMaxSpeed = 5000.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters | Bow")
	float ArrowMinSpeed = 1000.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters | Bow")
	UCurveFloat* ArrowSpeedCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters | Bow")
	UCurveFloat* SpreadAngleCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters | Bow")
	UCurveFloat* SpreadAngleUnstableCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters | Bow")
	UCurveFloat* ArrowDamageCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters | Bow")
	float FullStringTensionMaxTime = 10.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters | Bow")
	float ArrowMaxDamage = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters | Bow")
	float ArrowMinDamage = 20.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters | Bow")
	float FullStringTensionMaxStableTime = 8.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters | Bow")
	float UnstableSpreadAngleChange = 0.1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters | Bow")
	float UnstableSpreadAngleTimeChange = 0.1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters | Bow")
	float UnstableSpreadAngleAutoShot = 0.5;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters | Bow")
	float TensionProgress = 0.0f;
private:
	UPROPERTY()
	FTimerHandle TensionTimer;

	UPROPERTY()
	FTimerHandle UnstableTensionTimer;

	void UnstableAngleChange();

	void UpdateTensionValue();

	void AutoShot();
};
