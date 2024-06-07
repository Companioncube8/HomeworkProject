// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/MainMenu/JoinSessionWidget.h"
#include "HomeworkGameInstance.h"
#include "Kismet/GameplayStatics.h"

void UJoinSessionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld());
	check(GameInstance->IsA<UHomeworkGameInstance>());
	BaseGameInstance = StaticCast<UHomeworkGameInstance*>(GetGameInstance());
}

void UJoinSessionWidget::FindOnlineSession()
{
	BaseGameInstance->OnMatchFound.AddUObject(this, &UJoinSessionWidget::OnMatchFound);
	BaseGameInstance->FindMatch(bIsLan);
	SearchingSessionState = ESearchingSessionState::Searching;
}

void UJoinSessionWidget::JoinOnlineSession()
{
	BaseGameInstance->JoinOnlineGame();
}

void UJoinSessionWidget::OnMatchFound_Implementation(bool bIsSuccessful)
{
	SearchingSessionState = bIsSuccessful ? ESearchingSessionState::SessionIsFound : ESearchingSessionState::None;
	BaseGameInstance->OnMatchFound.RemoveAll(this);
}

void UJoinSessionWidget::CloseWidget()
{
	BaseGameInstance->OnMatchFound.RemoveAll(this);
	Super::CloseWidget();
}



