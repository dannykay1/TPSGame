// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapons/Weapon_Projectile.h"
#include "CollisionQueryParams.h"
#include "TPSGame/TPSGame.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SkeletalMeshComponent.h"


AWeapon_Projectile::AWeapon_Projectile()
{
	MuzzleSpeed = 1500.0f;
}


void AWeapon_Projectile::Fire()
{
	Super::Fire();

	AActor* MyOwner = GetOwner();
	if (MyOwner)
	{
		FVector EyeLocation;
		FRotator EyeRotation;
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector ShotDirection = EyeRotation.Vector();

		// Bullet Spread
		float HalfRad = FMath::DegreesToRadians(BulletSpread);
		ShotDirection = FMath::VRandCone(ShotDirection, HalfRad, HalfRad);

		FVector TraceEnd = EyeLocation + (ShotDirection * 10000);

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;

		FVector EndLocation = TraceEnd;

		FHitResult Hit;
		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, COLLISION_WEAPON, QueryParams))
		{
			EndLocation = Hit.ImpactPoint;
		}

		if (ProjectileToSpawn)
		{
			FVector MuzzleLocation = WeaponMesh->GetSocketLocation(MuzzleSocketName);
			FRotator LookRotation = UKismetMathLibrary::FindLookAtRotation(MuzzleLocation, EndLocation);

			FTransform SpawnTransform = FTransform(LookRotation, MuzzleLocation, FVector::OneVector);

			AActor* Projectile = GetWorld()->SpawnActorDeferred<AActor>(ProjectileToSpawn, SpawnTransform, MyOwner, MyOwner->GetInstigator(), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
			if (Projectile)
			{
				UGameplayStatics::FinishSpawningActor(Projectile, SpawnTransform);
			}
		}
	}
}