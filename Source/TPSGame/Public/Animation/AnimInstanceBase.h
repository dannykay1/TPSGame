// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "AnimInstanceBase.generated.h"

/**
 * Base class for all bi-pedal characters.
 */
UCLASS()
class TPSGAME_API UAnimInstanceBase : public UAnimInstance
{
	GENERATED_BODY()

public:
	/* Sets all default values. */
	UAnimInstanceBase();

	/* Performs initialization. */
	virtual void NativeInitializeAnimation() override;

	/* Updates the animation. */
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	/* The owner of the animation. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	class ACharacterBase* CharacterOwner;

	/* Is character moving? */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	uint8 bIsMoving : 1;

	/* Is character falling?*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	uint8 bIsFalling: 1;

	/* Is character dead?  Based on HealthComponent. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	uint8 bIsDead : 1;

	/* Is character aiming?*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	uint8 bIsAiming : 1;

	/* Is character crouching?*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	uint8 bIsCrouching : 1;

	/* Speed of the character. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	float Speed;

	/* Direction based on character velocity and rotation. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	float Direction;
};