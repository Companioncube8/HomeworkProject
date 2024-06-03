// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Equipment/EquipableItem.h"
#include "ThrowableItem.generated.h"

USTRUCT(BlueprintType)
struct FThrowInfo
{
	GENERATED_BODY()

	FThrowInfo() : Location_Mul_10(FVector_NetQuantize100::ZeroVector), Direction(FVector_NetQuantizeNormal::ZeroVector) {};
	FThrowInfo(FVector Location, FVector Direction) : Location_Mul_10(Location * 10), Direction(Direction) {};

	UPROPERTY()
	FVector_NetQuantize100 Location_Mul_10;

	UPROPERTY()
	FVector_NetQuantizeNormal Direction;

	FVector GetLocation() const { return Location_Mul_10 * 0.1f; }
	FVector GetDirection() const { return Direction; }
};

/**
 * 
 */
UCLASS(Blueprintable)
class HOMEWORKPROJECT_API AThrowableItem : public AEquipableItem
{
	GENERATED_BODY()

public:
	AThrowableItem();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void Throw();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Throwables", meta = (UIMin = -90.f, UIMax = 90.f, ClampMin = -90.f, ClampMax = 90.f))
	float ThrowAngle = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Throwables", meta = (UIMin = 1, ClampMin = 1))
	int ThrowablePoolSize = 10;

	void CreateProjectilePool();
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Throwables")
	TSubclassOf<class AProjectile> ProjectileClass;


	UFUNCTION(Server, Reliable)
	void Server_Throw(const FThrowInfo& ThrowInfo);


	UPROPERTY(Replicated)
	TArray<AProjectile*>ProjectilePool;

	const FVector ProjectilePoolLocation = FVector(0.f, 0.f, -100.f);

	UPROPERTY()
	int32 CurrentProjectileIndex;

	UPROPERTY(ReplicatedUsing = OnRep_LastThrowInfo)
	FThrowInfo LastThrowInfo;

	UFUNCTION()
	void OnRep_LastThrowInfo();

	void SetProjectile(const FThrowInfo& ThrowInfo);
};
