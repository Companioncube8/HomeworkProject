// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/MainMenu/HostSessionWidget.h"

#include "HomeworkGameInstance.h"
#include "Kismet/GameplayStatics.h"

void UHostSessionWidget::CreateSession()
{
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld());
	check(GameInstance->IsA<UHomeworkGameInstance>());
	UHomeworkGameInstance* HomeworkGameInstance = StaticCast<UHomeworkGameInstance*>(GetGameInstance());

	HomeworkGameInstance->LaunchLobby(4, ServerName, bIsLan);
}
