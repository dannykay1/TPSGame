// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/WeaponBase.h"
#include "Weapon_InstantHit.generated.h"

/**
 * Weapons that do a line trace to do damage (instantly).
 */
UCLASS()
class TPSGAME_API AWeapon_InstantHit : public AWeaponBase
{
	GENERATED_BODY()

public:
	AWeapon_InstantHit();

protected:
	/* Name of the target module on TracerEffect particle where particle end point should be. */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Instant Hit Weapon")
	FName TracerTargetName;

	/* Beam particle from fire point to hit point. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Instant Hit Weapon")
	class UParticleSystem* TracerEffect;

	/* Basic impact effect (non critical hits). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Instant Hit Weapon")
	class UParticleSystem* DefaultImpactEffect;

	/* Critical hit impact effect. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Instant Hit Weapon")
	class UParticleSystem* CriticalImpactEffect;

	/* How many line traces do perform per fire. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Instant Hit Weapon")
	int NumShotsPerFire;

	/* Damage when hitting non critical hits. */
	UPROPERTY(EditDefaultsOnly, Category = "Instant Hit Weapon")
	float BaseDamage;

	/* Overrides the fire function to perform the instant hit functionality. */
	virtual void Fire() override;

	/* Plays the impact effects at trace impact point. */
	void PlayImpactEffects(EPhysicalSurface SurfaceType, FVector ImpactPoint);

	/* Plays the sounds/particles of firing the weapon. */
	void PlayFireEffects(FVector TraceEnd);
};
