// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NetworkWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNetworkWidgetClosed);

/**
 * 
 */
UCLASS()
class HOMEWORKPROJECT_API UNetworkWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FOnNetworkWidgetClosed OnNetworkWidgetClosed;
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Network session");
	bool bIsLan = false;

	UFUNCTION(BlueprintPure)
	FText GetNetworkType() const;

	UFUNCTION(BlueprintCallable)
	void ToggleNetworkType();

	UFUNCTION(BlueprintCallable)
	virtual void CloseWidget();


};
