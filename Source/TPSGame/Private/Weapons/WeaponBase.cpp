// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapons/WeaponBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Sound/SoundCue.h"
#include "TPSGame/TPSGame.h"
#include "Components/SceneComponent.h"
#include "Pawns/CharacterBase.h"
#include "Animation/AnimMontage.h"


/* Sets default values for this actor's properties. */
AWeaponBase::AWeaponBase()
{
	PrimaryActorTick.bCanEverTick = false;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetRootComponent(WeaponMesh);

	WeaponAttachSocketName = "WeaponSocket_WeaponName";
	MuzzleSocketName = "MuzzleSocket";

	bIsAutomaticFire = false;
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


void AWeaponBase::PlayFireAnimMontage()
{
	ACharacterBase* MyOwner = Cast<ACharacterBase>(GetOwner());
	if (MyOwner)
	{
		if (MyOwner->GetIsAiming())
		{
			MyOwner->PlayAnimMontage(FireMontageIronsights);
		}
		else
		{
			MyOwner->PlayAnimMontage(FireMontageHip);
		}
	}
}


void AWeaponBase::PlayReloadAnimMontage()
{
	ACharacterBase* MyOwner = Cast<ACharacterBase>(GetOwner());
	if (MyOwner)
	{
		if (MyOwner->GetIsAiming())
		{
			MyOwner->PlayAnimMontage(ReloadMontageIronsights);
		}
		else
		{
			MyOwner->PlayAnimMontage(ReloadMontageHip);
		}
	}
}


void AWeaponBase::StartFire()
{
	float FirstDelay = FMath::Max(LastFireTime + TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);
	GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &AWeaponBase::Fire, TimeBetweenShots, bIsAutomaticFire, FirstDelay);
}


void AWeaponBase::StopFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
}


void AWeaponBase::Fire()
{
	PlayFireAnimMontage();

	if (MuzzleEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, WeaponMesh, MuzzleSocketName);
	}
	
	AActor* MyOwner = GetOwner();

	if (MuzzleSound)
	{
		if (MyOwner)
		{
			FVector EyeLocation = FVector::ZeroVector;
			FRotator EyeRotation;
			MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);
			UGameplayStatics::SpawnSoundAttached(MuzzleSound, MyOwner->GetRootComponent(), NAME_None, EyeLocation, EAttachLocation::SnapToTarget);
		}
		else
		{
			UGameplayStatics::SpawnSoundAttached(MuzzleSound, WeaponMesh, MuzzleSocketName);
		}
	}

	APawn* PawnOwner = Cast<APawn>(MyOwner);
	if (PawnOwner)
	{
		APlayerController* PC = Cast<APlayerController>(PawnOwner->GetController());
		if (PC)
		{
			PC->ClientPlayCameraShake(FireCamShake);
		}
	}

	LastFireTime = GetWorld()->TimeSeconds;
}