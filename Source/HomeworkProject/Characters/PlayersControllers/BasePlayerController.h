// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BasePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class HOMEWORKPROJECT_API ABasePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void SetPawn(APawn* InPawn) override;

	bool GetIgnoreCameraPitch() const;
	void SetIgnoreCameraPitch(bool bIgnoreCameraPitch_In);

protected:
	virtual void SetupInputComponent() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
	TSubclassOf<class UPlayerHUDWidget> PlayerHudWidgetClass;

private:
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void TurnAtRate(float Value);
	void LookUpAtRate(float Value);
	void ChangeCrouchState();
	void Jump();
	void StopSprint();
	void StartSprint();
	void ChangeProneState();
	void Mantle();
	void Slide();

	void SwimForward(float Value);
	void SwimRight(float Value);
	void SwimUp(float Value);

	void ClimbLadderUp(float Value);
	void InteractWithLadder();
	void InteractWithZipline();

	void PlayerStartFire();
	void PlayerStopFire();

	void StartAiming();
	void StopAiming();

	void NextItem();
	void PreviousItem();

	void Reload();

	void EquipPrimaryItem();

	void AlternativeFire();

	void PrimaryMeleeAttack();
	void SecondaryMeleeAttack();
	
	TSoftObjectPtr<class ABaseCharacter> CachedBaseCharacter;

private:
	bool bIgnoreCameraPitch = false;

	UPlayerHUDWidget* PlayerHUDWidget = nullptr;

	void CreateAndInitializeWidgets();
};
