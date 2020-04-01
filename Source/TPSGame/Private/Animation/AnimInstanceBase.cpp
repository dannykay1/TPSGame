// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/AnimInstanceBase.h"
#include "Pawns/CharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"


/* Sets all default values. */
UAnimInstanceBase::UAnimInstanceBase()
{
	bIsAiming = false;
	bIsMoving = false;
	Speed = 0.0f;
	Direction = 0.0f;
}


/* Performs initialization and sets up references. */
void UAnimInstanceBase::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	APawn* PawnOwner = TryGetPawnOwner();
	if (PawnOwner)
	{
		CharacterOwner = Cast<ACharacterBase>(PawnOwner);
	}
}


/* Updates the animation. */
void UAnimInstanceBase::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!CharacterOwner)
	{
		return;
	}

	FVector Velocity = CharacterOwner->GetVelocity();

	// Zero out z velocity so animation only uses x and y values for speed.
	Velocity.Z = 0.0f;

	Speed = Velocity.Size();

	bIsMoving = Speed > 0.1f;

	if (CharacterOwner->GetCharacterMovement())
	{
		bIsFalling = CharacterOwner->GetCharacterMovement()->IsFalling();
		bIsCrouching = CharacterOwner->GetCharacterMovement()->IsCrouching();
	}

	bIsAiming = CharacterOwner->GetIsAiming();

	Direction = CalculateDirection(Velocity, CharacterOwner->GetActorRotation());

	FRotator ControlRotation = CharacterOwner->GetControlRotation();
	FRotator ActorRotation = CharacterOwner->GetActorRotation();

	FRotator CurrentRotation = FRotator(Pitch, 0.0f, 0.0f);
	FRotator TargetRotation = UKismetMathLibrary::ComposeRotators(ControlRotation, ActorRotation);

	FRotator FinalRotator = UKismetMathLibrary::RInterpTo(CurrentRotation, TargetRotation, DeltaSeconds, 15.0f);
	Pitch = UKismetMathLibrary::ClampAngle(FinalRotator.Pitch, -90.0f, 90.0f);
}

