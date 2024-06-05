// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Equipment/EquipableItem.h"
#include "Components/Weapon/WeaponBarellComponent.h"
#include "RangeWeaponItem.generated.h"

/**
 * 
 */
class UAnimMontage;

DECLARE_MULTICAST_DELEGATE(FOnReloadComplete)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnAmmoChanged, int32)

UENUM(BlueprintType)
enum class EWeaponFireMode: uint8
{
	Single,
	FullAuto
};

UENUM(BlueprintType)
enum class EReloadType : uint8
{
	FullClip,
	ByBullet
};
USTRUCT(BlueprintType)
struct FFireModeInfo
{
	GENERATED_BODY();
	// FullClip reload type adds ammo only when the whole reload animation is successfully played
	// ByBullet reload type requires sections "ReloadEnd" in character reload animation
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations | Weapon")
	EReloadType ReloadType = EReloadType::FullClip;

	// Rate of fire in rounds per minutes
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters", meta = (ClampMin = 1.0f, UIMin = 1.f))
	float RateOfFire = 600.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters")
	EWeaponFireMode WeaponFireMode = EWeaponFireMode::Single;

	//Bullet spread half angle in degrees
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters", meta = (ClampMin = 0.f, UIMin = 0.f, ClampMax = 2.f, UIMax = 2.f))
	float SpreadAngle = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters| Aiming", meta = (ClampMin = 0.f, UIMin = 0.f, ClampMax = 2.f, UIMax = 2.f))
	float AimSpreadAngle = 0.25f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters| Aiming", meta = (ClampMin = 0.f, UIMin = 0.f, ClampMax = 120.f, UIMax = 120.f))
	float AimFOV = 60.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters| Ammo", meta = (ClampMin = 1, UIMin = 1))
	int32 MaxAmmo = 30;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters| Ammo", meta = (ClampMin = 1, UIMin = 1))
	EAmunitionType AmmoType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters| Ammo")
	bool bAutoReload = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters| Ammo")
	FFireInfo FireInfo;
};

UCLASS(Blueprintable)
class HOMEWORKPROJECT_API ARangeWeaponItem : public AEquipableItem
{
	GENERATED_BODY()
public:
	ARangeWeaponItem();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void StartFire();
	void StopFire();

	bool IsFiring() const { return bIsFiring; };
	bool IsReloading() const { return bIsReloading; };

	bool CanUseAmmo() const { return Ammo.Num() > 0; }

	void StartAim();
	void StopAim();

	FTransform GetForeGripTransform() const;

	void StartReload();
	void EndReload(bool bIsSuccess);

	float GetAimFOV() const;
	float GetAimMovmentMaxSpeed() const;

	float GetAimLookUpModifier() const;
	float GetAimTurnModifier() const;

	int32 GetAmmo() const;
	int32 GetMaxAmmo() const;

	void SetAmmo(int32 NewAmmo);

	bool CanShoot() const;

	EAmunitionType GetAmmoType() const;

	FOnAmmoChanged OnAmmoChanged;
	FOnReloadComplete OnReloadComplete;

	void CreateFireModes();

	virtual EReticleType GetReticleType() const override;

	void ChangeFireMode();
protected:
	virtual void BeginPlay() override;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UWeaponBarellComponent* WeaponBarell;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations | Weapon")
	UAnimMontage* WeaponFireMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations | Weapon")
	UAnimMontage* WeaponReloadMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations | Character")
	UAnimMontage* CharacterFireMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations | Character")
	UAnimMontage* CharacterReloadMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters| Aiming", meta = (ClampMin = 0.f, UIMin = 0.f))
	float AimMovementMaxSpeed = 200.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters| Aiming", meta = (ClampMin = 0.f, UIMin = 0.f, ClampMax = 1.f, UIMax = 1.f))
	float AimTurnModifier = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters| Aiming", meta = (ClampMin = 0.f, UIMin = 0.f, ClampMax = 1.f, UIMax = 1.f))
	float AimLookUpModifier = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Reticle")
	EReticleType AimReticleType = EReticleType::Default;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon | Fire mode")
	TArray<FFireModeInfo> FireModes;
private:

	UPROPERTY(Replicated)
	TArray<int32> Ammo;

	bool bIsFiring = false;
	bool bIsReloading = false;

	void MakeShot();

	float GetShotTimerInterval() const;

	float PlayAnimMontage(UAnimMontage* AnimMontage);
	void StopAnimMontage(UAnimMontage* AnimMontage, float BlendOutTime = 0.f);


	FTimerHandle ShotTimer;

	FTimerHandle ReloadTimer;

	bool bIsAiming;

	float GetCurrentBulletSpreadAngle() const;

	void OnShotTimerElapsed();

	FFireModeInfo CurrentFireMode() const { return FireModes[IndexCurrentFireMode]; };

	UPROPERTY(Replicated)
	int32 IndexCurrentFireMode = 0;

	UFUNCTION(Server, Reliable)
	void Server_Reload();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Reload();

	void ReloadAmmo();

	UFUNCTION(Server, Reliable)
	void Server_ChangeAmmo(int32 Index, int32 Value);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ChangeAmmo(int32 Index, int32 Value);

	UFUNCTION(Server, Reliable)
	void Server_EndReload(bool bIsSuccess);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_EndReload(bool bIsSuccess);

	void EndReloadReplicated(bool bIsSuccess);
};
