// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/WeaponBase.h"
#include "Weapon_Projectile.generated.h"

/**
 * 
 */
UCLASS()
class TPSGAME_API AWeapon_Projectile : public AWeaponBase
{
	GENERATED_BODY()

public:
	AWeapon_Projectile();

protected:
	/* Overrides the fire function to spawn a projectile. */
	virtual void Fire() override;

	/* Projectile that will be spawned when firing. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile Weapon")
	TSubclassOf<class AProjectileBase> ProjectileToSpawn;
};
