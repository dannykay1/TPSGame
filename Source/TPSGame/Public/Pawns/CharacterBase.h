// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterBase.generated.h"

/**
 * Base class for all characters.
 */
UCLASS()
class TPSGAME_API ACharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	/* Sets default values for this character's properties. */
	ACharacterBase();

	/* Called every frame. */
	virtual void Tick(float DeltaTime) override;

	/* Called to bind functionality to input. */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/* Gets the camera location used for shooting. */
	virtual FVector GetPawnViewLocation() const override;

	/* Returns CameraBoom subobject. */
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/* Returns FollowCamera subobject. */
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	/* Returns if character is aiming. */
	FORCEINLINE uint8 GetIsAiming() const { return bIsAiming; }

protected:
	/* Called when the game starts or when spawned. */
	virtual void BeginPlay() override;

	/* Camera boom positioning the camera behind the character. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class USpringArmComponent* CameraBoom;

	/* Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class UCameraComponent* FollowCamera;

	/* Weapon character starts with. */
	UPROPERTY(EditDefaultsOnly, Category = "CharacterBase")
	TSubclassOf<class AWeaponBase> StarterWeaponClass;

	/* Currently equipped weapon. */
	class AWeaponBase* CurrentWeapon;

	/* Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	float BaseTurnRate;

	/* Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	float BaseLookUpRate;

	/* Used if character is aming down sights. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CharacterBase")
	uint8 bIsAiming : 1;

	/* Starts the fire function on the current weapon. */
	void StartFire();
	
	/* Stops the fire function on the current weapon. */
	void StopFire();

	/* Starts the reload operation on the current weapon. */
	void Reload();

	/* Called for forwards/backward input. */
	void MoveForward(float Value);

	/* Called for side to side input. */
	void MoveRight(float Value);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/* Character is aiming down ironsight. Adjusts speed and animation to match. */
	void BeginAiming();

	/* Character stops aiming down ironsight. Adjusts speed and animation to match. */
	void EndAiming();

	/* Character enters crouch mode. Adjusts speed and animation to match. */
	void BeginCrouching();

	/* Character exits crouch mode. Adjusts speed and animation to match. */
	void EndCrouching();
};
