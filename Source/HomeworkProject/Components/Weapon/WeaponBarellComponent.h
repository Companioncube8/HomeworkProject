// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "WeaponBarellComponent.generated.h"

UENUM(BlueprintType)
enum class EHitRegistrationType: uint8
{
	HitScan,
	Projectile
};

UENUM(BlueprintType)
enum class EFireMode : uint8
{
	Default,
	RifleGrenades
};

USTRUCT(BlueprintType)
struct FDecalInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Decal info")
	UMaterialInterface* DecalMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Decal info")
	FVector DecalSize = FVector(5.f, 5.f, 5.f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Decal info")
	float DecalLifeTime = 10.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Decal info")
	float DecalFadeOutTime = 5.f;
};

USTRUCT(BlueprintType)
struct FFireInfo
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell attributes", meta = (ClampMin = 1, UIMin = 1))
	int32 BulletsPerShot = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barrel attributes | Hit registration")
	EHitRegistrationType HitRegistration = EHitRegistrationType::HitScan;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barrel attributes | Hit registration", meta = (EditCondition = "HitRegistration == EHitRegistrationType::Projectile"))
	TSubclassOf<class AProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barrel attributes | Hit registration", meta = (EditCondition = "HitRegistration == EHitRegistrationType::Projectile"))
	bool NeedCreatePoolProjectile = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell attributes")
	float FiringRange = 5000.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell attributes | Damage")
	float DamageAmount = 20.f;
};

class UNiagaraSystem;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HOMEWORKPROJECT_API UWeaponBarellComponent : public USceneComponent
{
	GENERATED_BODY()
public:
	void Shot(FVector ShotStart, FVector ShotDirection, float SpreadAngle);
	void SetFireInfo(FFireInfo NewFireInfo) {
		FireInfo = NewFireInfo;
		FireDamage = FireInfo.DamageAmount;
	}

<<<<<<< Updated upstream
=======
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void CreateProjectilePool();

	AProjectile* GetProjectile();
	void RemoveLoadedProjectile();

	void SetFireDamage(float FireDamage_In) { FireDamage = FireDamage_In; }

>>>>>>> Stashed changes
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell attributes | Damage")
	UNiagaraSystem* MuzzleFlashFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell attributes | Damage")
	UNiagaraSystem* TraceFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell attributes | Damage")
	UCurveFloat* FalloffDiagram;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell attributes | Damage")
	TSubclassOf<class UDamageType> DamageTypeClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell attributes | Decals")
	FDecalInfo DefaultDecalInfo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell attributes | Fire mode")
	FFireInfo FireInfo;

private:
	FVector GetBulletSpreadOffset(float Angle, FRotator ShotRotation) const;

	bool HitScan(FVector ShotStart, FVector ShotDirection, FVector& ShotEnd);
	void LaunchProjectile(const FVector& LaunchStart, const FVector& LaunchDirection);

	APawn* GetOwningPawn() const;
	AController* GetController() const;

	UFUNCTION()
	void ProcessHit(const FHitResult& HitResult, const FVector& Direction);
<<<<<<< Updated upstream
=======

	UFUNCTION(Server, Reliable)
	void Server_Shot(const TArray<FShotInfo>& ShotsInfos);

	UPROPERTY(ReplicatedUsing = OnRep_LoadedProjectile)
	AProjectile* LoadedProjectile;

	UFUNCTION()
	void OnRep_LoadedProjectile(AProjectile* OldProjectile);

	UPROPERTY(ReplicatedUsing = OnRep_LastShotsInfo)
	TArray<FShotInfo> LastShotsInfo;

	UPROPERTY(Replicated)
	TArray<AProjectile*>ProjectilePool;

	UPROPERTY(Replicated)
	int32 CurrentProjectileIndex;
	
	UFUNCTION()
	void OnRep_LastShotsInfo();

	const FVector ProjectilePoolLocation = FVector(0.f, 0.f, -100.f);

	UPROPERTY()
	float FireDamage = 0;
>>>>>>> Stashed changes
};
