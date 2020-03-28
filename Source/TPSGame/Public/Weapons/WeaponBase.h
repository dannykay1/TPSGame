// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

UCLASS()
class TPSGAME_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	/* Sets default values for this actor's properties. */
	AWeaponBase();

	FORCEINLINE FName GetWeaponAttachSocketName() const { return WeaponAttachSocketName; }

	/* Starts the TimerHandle_TimeBetweenShots for firing the weapon. */
	void StartFire();

	/* Clears the TimerHandle_TimeBetweenShots. */
	void StopFire();

protected:
	/* Called when the game starts or when spawned. */
	virtual void BeginPlay() override;

	/* Logic of weapon firing. */
	virtual void Fire();

	/* Weapon Mesh. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	class USkeletalMeshComponent* WeaponMesh;

	/* Socket name on character owner where weapon will be attached. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName WeaponAttachSocketName;
	
	/* Socket name on WeaponMesh where socket is. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName MuzzleSocketName;

	/* Camera shake to play when firing the weapon. */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<class UCameraShake> FireCamShake;

	/* Particle to play at muzzle when firing. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	class UParticleSystem* MuzzleEffect;

	/* Sound to play when firing the weapon. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	class USoundCue* MuzzleSound;

	/* Timer handle that controls when owner of weapon can fire. */
	FTimerHandle TimerHandle_TimeBetweenShots;

	/* Used with TimerHandle_TimeBetweenShots to control fire rate. */
	float LastFireTime;

	/* RPM - Bullets per minute fired by weapon. */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float RateOfFire;

	/* Bullet Spread in Degrees. */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon", meta = (ClampMin = 0.0f))
	float BulletSpread;

	/* Derived from RateOfFire. */
	float TimeBetweenShots;
};
