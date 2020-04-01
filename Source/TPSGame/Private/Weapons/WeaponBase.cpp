// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapons/WeaponBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Sound/SoundCue.h"


/* Sets default values for this actor's properties. */
AWeaponBase::AWeaponBase()
{
	PrimaryActorTick.bCanEverTick = false;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetRootComponent(WeaponMesh);

	// Setup socket names. 
	WeaponAttachSocketName = "WeaponSocket_WeaponName";
	MuzzleSocketName = "MuzzleSocket";

	BulletSpread = 2.0f;
	RateOfFire = 600;
}


/* Called when the game starts or when spawned. */
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	LastFireTime = -100.0f;
	TimeBetweenShots = 60 / RateOfFire;
}


void AWeaponBase::StartFire()
{
	float FirstDelay = FMath::Max(LastFireTime + TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);
	GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &AWeaponBase::Fire, TimeBetweenShots, true, FirstDelay);
}

void AWeaponBase::StopFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
}


void AWeaponBase::Fire()
{
	if (MuzzleEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, WeaponMesh, MuzzleSocketName);
	}

	if (MuzzleSound)
	{
		UGameplayStatics::SpawnSoundAttached(MuzzleSound, WeaponMesh, MuzzleSocketName);
	}

	APawn* MyOwner = Cast<APawn>(GetOwner());
	if (MyOwner)
	{
		APlayerController* PC = Cast<APlayerController>(MyOwner->GetController());
		if (PC)
		{
			PC->ClientPlayCameraShake(FireCamShake);
		}
	}

	LastFireTime = GetWorld()->TimeSeconds;
}