// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/AnimInstanceBase.h"
#include "Pawns/CharacterBase.h"


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
	bIsAiming = CharacterOwner->GetIsAiming();
	Direction = CalculateDirection(Velocity, CharacterOwner->GetActorRotation());
}

