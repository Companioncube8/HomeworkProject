// Fill out your copyright notice in the Description page of Project Settings.


#include "FPPlayerAnimInstance.h"
#include "HomeworkProject/Characters/FPPlayerCharacter.h"
#include "HomeworkProject/Characters/PlayersControllers/BasePlayerController.h"

void UFPPlayerAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	checkf(TryGetPawnOwner()->IsA<AFPPlayerCharacter>(), TEXT("UFPPlayerAnimInstance::NativeBeginPlay() can use only AFPPlayerCharecter"));
	CachedFirstPersonCharacterOwner = StaticCast<AFPPlayerCharacter*>(TryGetPawnOwner());
}

void UFPPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (!CachedFirstPersonCharacterOwner.IsValid())
	{
		return;
	}
	PlayerCameraPitchAngle = CalculateCameraPitchAngle();
}

float UFPPlayerAnimInstance::CalculateCameraPitchAngle() const
{
	float Result = 0.f;

	ABasePlayerController* PlayerController = CachedFirstPersonCharacterOwner->GetController<ABasePlayerController>();
	if (IsValid(PlayerController) && !PlayerController->GetIgnoreCameraPitch())
	{
		Result = PlayerController->GetControlRotation().Pitch;
	}
	return  Result;

}
