// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayerController.h"

#include "../BaseCharacter.h"
#include "Blueprint/UserWidget.h"
#include "Components/CharacterComponents/CharacterEquipmentComponent.h"
#include "GameFramework/PlayerInput.h"
#include "UI/Widget/PlayerHUDWidget.h"

void ABasePlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
	CachedBaseCharacter = Cast<ABaseCharacter>(InPawn);
	if (CachedBaseCharacter.IsValid() && IsLocalController())
	{
		CreateAndInitializeWidgets();
		CachedBaseCharacter->OnInteractableObjectFound.BindUObject(this, &ABasePlayerController::OnInteractableObjectFound);
	}
}


void ABasePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAxis("MoveForward", this, &ABasePlayerController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ABasePlayerController::MoveRight);
	InputComponent->BindAxis("Turn", this, &ABasePlayerController::Turn);
	InputComponent->BindAxis("LookUp", this, &ABasePlayerController::LookUp);
	InputComponent->BindAction("InteractWithLadder", EInputEvent::IE_Pressed, this, &ABasePlayerController::InteractWithLadder);
	InputComponent->BindAction("InteractWithZipline", EInputEvent::IE_Pressed, this, &ABasePlayerController::InteractWithZipline);
	InputComponent->BindAction("Mantle", EInputEvent::IE_Pressed, this, &ABasePlayerController::Mantle);
	InputComponent->BindAction("Jump", IE_Pressed, this, &ABasePlayerController::Jump);
	InputComponent->BindAxis("TurnAtRate", this, &ABasePlayerController::TurnAtRate);
	InputComponent->BindAxis("LookUpAtRate", this, &ABasePlayerController::LookUpAtRate);
	InputComponent->BindAction("Slide", IE_Pressed, this, &ABasePlayerController::Slide);
	InputComponent->BindAction("Prone", IE_Pressed, this, &ABasePlayerController::ChangeProneState);
	InputComponent->BindAction("Crouch", IE_Pressed, this, &ABasePlayerController::ChangeCrouchState);
	InputComponent->BindAction("Sprint", IE_Pressed, this, &ABasePlayerController::StartSprint);
	InputComponent->BindAction("Sprint", IE_Released, this, &ABasePlayerController::StopSprint);
	InputComponent->BindAxis("SwimForward", this, &ABasePlayerController::SwimForward);
	InputComponent->BindAxis("SwimRight", this, &ABasePlayerController::SwimRight);
	InputComponent->BindAxis("SwimUp", this, &ABasePlayerController::SwimUp);
	InputComponent->BindAxis("ClimbLadderUp", this, &ABasePlayerController::ClimbLadderUp);
	InputComponent->BindAction("Fire", EInputEvent::IE_Pressed, this, &ABasePlayerController::PlayerStartFire);
	InputComponent->BindAction("Fire", EInputEvent::IE_Released, this, &ABasePlayerController::PlayerStopFire);
	InputComponent->BindAction("Aim", EInputEvent::IE_Pressed, this, &ABasePlayerController::StartAiming);
	InputComponent->BindAction("Aim", EInputEvent::IE_Released, this, &ABasePlayerController::StopAiming);
	InputComponent->BindAction("Reload", EInputEvent::IE_Pressed, this, &ABasePlayerController::Reload);
	InputComponent->BindAction("NextItem", EInputEvent::IE_Pressed, this, &ABasePlayerController::NextItem);
	InputComponent->BindAction("PreviousItem", EInputEvent::IE_Pressed, this, &ABasePlayerController::PreviousItem);
	InputComponent->BindAction("EquipPrimaryItem", EInputEvent::IE_Pressed, this, &ABasePlayerController::EquipPrimaryItem);
	InputComponent->BindAction("AlternativeFire", EInputEvent::IE_Pressed, this, &ABasePlayerController::AlternativeFire);
	InputComponent->BindAction("PrimaryMeleeAttack", EInputEvent::IE_Pressed, this, &ABasePlayerController::PrimaryMeleeAttack);
	InputComponent->BindAction("SecondaryMeleeAttack", EInputEvent::IE_Pressed, this, &ABasePlayerController::SecondaryMeleeAttack);
	InputComponent->BindAction(ActionInteract, EInputEvent::IE_Pressed, this, &ABasePlayerController::Interact);
	FInputActionBinding& ToggleMainBinding = InputComponent->BindAction("ToggleMainMenu", EInputEvent::IE_Pressed, this, &ABasePlayerController::ToggleMainMenu);
	ToggleMainBinding.bExecuteWhenPaused = true;
}

bool ABasePlayerController::GetIgnoreCameraPitch() const
{
	return bIgnoreCameraPitch;
}

void ABasePlayerController::SetIgnoreCameraPitch(bool bIgnoreCameraPitch_In)
{
	bIgnoreCameraPitch = bIgnoreCameraPitch_In;
}

void ABasePlayerController::MoveForward(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->MoveForward(Value);
	}
}

void ABasePlayerController::MoveRight(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->MoveRight(Value);
	}
}

void ABasePlayerController::Turn(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Turn(Value);
	}
}


void ABasePlayerController::LookUp(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->LookUp(Value);
	}
}

void ABasePlayerController::Jump()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Jump();
	}
}

void ABasePlayerController::TurnAtRate(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->TurnAtRate(Value);
	}
}

void ABasePlayerController::LookUpAtRate(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->LookUpAtRate(Value);
	}
}


void ABasePlayerController::ChangeCrouchState()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->ChangeCrouchState();
	}
}


void ABasePlayerController::ChangeProneState()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->ChangeProneState();
	}
}

void ABasePlayerController::StartSprint()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StartSprint();
	}
}

void ABasePlayerController::StopSprint()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StopSprint();
	}
}

void ABasePlayerController::SwimForward(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->SwimForward(Value);
	}
}

void ABasePlayerController::SwimRight(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->SwimRight(Value);
	}
}

void ABasePlayerController::SwimUp(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->SwimUp(Value);
	}
}

void ABasePlayerController::Mantle()
{

	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Mantle();
	}
}

void ABasePlayerController::ClimbLadderUp(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->ClimbLadderUp(Value);
	}
}

void ABasePlayerController::InteractWithLadder()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->InteractWithLadder();
	}
}

void ABasePlayerController::InteractWithZipline()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->InteractWithZipline();
	}
}

void ABasePlayerController::Slide()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Slide();
	}
}

void ABasePlayerController::PlayerStartFire()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->PlayerStartFire();
	}
}

void ABasePlayerController::PlayerStopFire()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->PlayerStopFire();
	}
	
}

void ABasePlayerController::StartAiming()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StartAiming();
	}
}

void ABasePlayerController::StopAiming()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StopAiming();
	}
}

void ABasePlayerController::Reload()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Reload();
	}
}

void ABasePlayerController::NextItem()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->NextItem();
	}
}

void ABasePlayerController::PreviousItem()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->PreviousItem();
	}
}

void ABasePlayerController::EquipPrimaryItem()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->EquipPrimaryItem();
	}
}

void ABasePlayerController::AlternativeFire()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->AlternativeFire();
	}
}

void ABasePlayerController::PrimaryMeleeAttack()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->PrimaryMeleeAttack();
	}
}

void ABasePlayerController::SecondaryMeleeAttack()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->SecondaryMeleeAttack();
	}
}

void ABasePlayerController::Interact()
{

	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Interact();
	}
}


void ABasePlayerController::ToggleMainMenu()
{
	if (!MainMenuWidget || !PlayerHUDWidget)
	{
		return;
	}

	if (MainMenuWidget->IsVisible())
	{
		MainMenuWidget->RemoveFromParent();
		PlayerHUDWidget->AddToViewport();
		SetInputMode(FInputModeGameOnly{});
		SetPause(false);
		bShowMouseCursor = false;
	}
	else
	{
		MainMenuWidget->AddToViewport();
		PlayerHUDWidget->RemoveFromParent();
		SetInputMode(FInputModeGameAndUI{});
		SetPause(true);
		bShowMouseCursor = true;
	}
}


void ABasePlayerController::CreateAndInitializeWidgets()
{
	if (!IsValid(PlayerHUDWidget)) {
		PlayerHUDWidget = CreateWidget<UPlayerHUDWidget>(GetWorld(), PlayerHudWidgetClass);
		if (IsValid(PlayerHUDWidget))
		{
			PlayerHUDWidget->AddToViewport();
		}
	}

	if(!MainMenuWidget)
	{
		MainMenuWidget = CreateWidget<UUserWidget>(GetWorld(), MainMenuWidgetClass);
	}

	SetInputMode(FInputModeGameOnly{});
	bShowMouseCursor = false;
}

void ABasePlayerController::OnInteractableObjectFound(FName ActionName)
{
	if (!IsValid(PlayerHUDWidget))
	{
		return;
	}

	TArray<FInputActionKeyMapping> ActionKeys = PlayerInput->GetKeysForAction(ActionName);
	const bool HasAnyKeys = ActionKeys.Num() != 0;
	if (HasAnyKeys)
	{
		FName ActionKey = ActionKeys[0].Key.GetFName();
		PlayerHUDWidget->SetHighlightInteractableActionText(ActionKey);
	}
	PlayerHUDWidget->SetHighlightInteractableVisibility(HasAnyKeys);
}
