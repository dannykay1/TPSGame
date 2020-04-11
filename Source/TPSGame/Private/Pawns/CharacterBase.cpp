// Fill out your copyright notice in the Description page of Project Settings.

#include "Pawns/CharacterBase.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Weapons/WeaponBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "TPSGame/TPSGame.h"


// Sets default values
ACharacterBase::ACharacterBase()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(45.f, 100.0f);
	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Block);
	bUseControllerRotationYaw = true;

	bIsAiming = false;

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	SecondaryWeaponSocketName = "Socket_SecondaryWeapon";

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm	
}


// Called when the game starts or when spawned
void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();

	// Spawn a default weapon
	GiveWeapon(StarterWeaponClass);
}


// Called every frame
void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


// Called to bind functionality to input
void ACharacterBase::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &ACharacterBase::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACharacterBase::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ACharacterBase::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ACharacterBase::LookUpAtRate);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ACharacterBase::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ACharacterBase::StopFire);

	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ACharacterBase::Reload);
	PlayerInputComponent->BindAction("SwitchWeapons", IE_Pressed, this, &ACharacterBase::PlaySwitchWeaponsAnimMontage);

	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ACharacterBase::BeginAiming);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ACharacterBase::EndAiming);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ACharacterBase::ToggleCrouch);
}


void ACharacterBase::GiveWeapon(TSubclassOf<class AWeaponBase> WeaponToGive)
{
	if (!WeaponToGive || Weapons.Num() >= 2)
		return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AWeaponBase* SpawnedWeapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponToGive, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	if (SpawnedWeapon)
	{
		SpawnedWeapon->SetOwner(this);

		FName SocketName = Weapons.Num() == 0 
			? SpawnedWeapon->GetWeaponAttachSocketName()
			: SecondaryWeaponSocketName;

		SpawnedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
	}

	Weapons.Add(SpawnedWeapon);

	if (!CurrentWeapon)
		CurrentWeapon = SpawnedWeapon;
}


FVector ACharacterBase::GetPawnViewLocation() const
{
	if (FollowCamera && CameraBoom)
	{
		FVector ViewLocation = FollowCamera->GetComponentLocation();

		// Offset the view location by arm length so character cannot hit actors behind this character.
		ViewLocation += FollowCamera->GetForwardVector() * CameraBoom->TargetArmLength;

		return ViewLocation;
	}

	return Super::GetPawnViewLocation();
}


void ACharacterBase::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}


void ACharacterBase::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}


void ACharacterBase::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}


void ACharacterBase::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}


void ACharacterBase::StartFire()
{
	if (CurrentWeapon)
	{
		StopAnimMontage();
		CurrentWeapon->StartFire();
	}
}


void ACharacterBase::StopFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StopFire();
	}
}


void ACharacterBase::Reload()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->PlayReloadAnimMontage();
	}
}


void ACharacterBase::PlaySwitchWeaponsAnimMontage()
{
	if (Weapons.Num() <= 1)
		return;

	StopFire();

	if (SwitchWeaponMontage)
	{
		PlayAnimMontage(SwitchWeaponMontage);
	}
}


void ACharacterBase::SwapWeapons()
{
	if (Weapons.Num() <= 1)
		return;

	if (CurrentWeapon == Weapons[0])
	{
		Weapons[0]->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, SecondaryWeaponSocketName);

		Weapons[1]->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, Weapons[1]->GetWeaponAttachSocketName());

		CurrentWeapon = Weapons[1];
	}
	else
	{
		Weapons[1]->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, SecondaryWeaponSocketName);

		Weapons[0]->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, Weapons[0]->GetWeaponAttachSocketName());

		CurrentWeapon = Weapons[0];
	}
}


void ACharacterBase::BeginAiming()
{
	bIsAiming = true;
	GetCharacterMovement()->MaxWalkSpeed = 170.0f;
}


void ACharacterBase::EndAiming()
{
	bIsAiming = false;
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;
}


void ACharacterBase::ToggleCrouch()
{
	if (GetCharacterMovement()->IsCrouching())
	{
		EndCrouching();
	}
	else
	{
		BeginCrouching();
	}
}


void ACharacterBase::BeginCrouching()
{
	if (GetCharacterMovement()->IsCrouching())
	{
		EndCrouching();
	}
	else
	{
		Crouch();
		GetCharacterMovement()->MaxWalkSpeedCrouched = 170.0f;
	}
}


void ACharacterBase::EndCrouching()
{
	UnCrouch();
	GetCharacterMovement()->MaxWalkSpeedCrouched = 170.0f;
}