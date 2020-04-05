// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapons/Weapon_InstantHit.h"
#include "CollisionQueryParams.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Components/SkeletalMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "TimerManager.h"
#include "GameFramework/DamageType.h"
#include "TPSGame/TPSGame.h"


AWeapon_InstantHit::AWeapon_InstantHit()
{
	TracerTargetName = "Target";
	BaseDamage = 20.0f;
	NumShotsPerFire = 1;
}


void AWeapon_InstantHit::Fire()
{
	Super::Fire();	

	for (int i = 0; i < NumShotsPerFire; i++)
	{
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
			QueryParams.bReturnPhysicalMaterial = true;

			// Particle "Target" parameter
			FVector TracerEndPoint = TraceEnd;

			EPhysicalSurface SurfaceType = SurfaceType_Default;

			FHitResult Hit;
			if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, COLLISION_WEAPON, QueryParams))
			{
				// Blocking hit! Process damage
				AActor* HitActor = Hit.GetActor();

				SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

				float ActualDamage = BaseDamage;
				if (SurfaceType == SURFACE_CRITICAL)
				{
					ActualDamage *= 4.0f;
				}

				UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, Hit, MyOwner->GetInstigatorController(), MyOwner, UDamageType::StaticClass());

				PlayImpactEffects(SurfaceType, Hit.ImpactPoint);

				TracerEndPoint = Hit.ImpactPoint;
			}

			PlayFireEffects(TracerEndPoint);
		}
	}
}


void AWeapon_InstantHit::PlayImpactEffects(EPhysicalSurface SurfaceType, FVector ImpactPoint)
{
	UParticleSystem* SelectedEffect = nullptr;
	switch (SurfaceType)
	{
	case SURFACE_BASIC:
	case SURFACE_CRITICAL:
		SelectedEffect = CriticalImpactEffect;
		break;
	default:
		SelectedEffect = DefaultImpactEffect;
		break;
	}

	if (SelectedEffect)
	{
		FVector MuzzleLocation = WeaponMesh->GetSocketLocation(MuzzleSocketName);

		FVector ShotDirection = ImpactPoint - MuzzleLocation;
		ShotDirection.Normalize();

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, ImpactPoint, ShotDirection.Rotation());
	}
}


void AWeapon_InstantHit::PlayFireEffects(FVector TraceEnd)
{
	if (TracerEffect)
	{
		FVector MuzzleLocation = WeaponMesh->GetSocketLocation(MuzzleSocketName);

		UParticleSystemComponent* TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MuzzleLocation);
		if (TracerComp)
		{
			TracerComp->SetVectorParameter(TracerTargetName, TraceEnd);
		}
	}
}