// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SAnimInstance.generated.h"

class ASCharacter;

UCLASS()
class LOSTDIARY_API USAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:
	// Holds character movement speed
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float MovementSpeed;

	// Derive from character input values and direction value
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float InputX_Anim;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float InputY_Anim;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float Direction;

	// Character is jumping / falling - true / false
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		bool bIsFalling;

	// Character received initial direction - true / false
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		bool bReceivedInitialDirection;

	// Character is crouching - true / false
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		bool bCrouching;

	// Ref to owner character
	UPROPERTY(BlueprintReadOnly)
		ASCharacter* OwnerCharacter;

public:
	// Called for updating animation properties
	UFUNCTION(BlueprintCallable)
		void UpdateAnimationProperties(float DeltaTime);

	UFUNCTION(BlueprintImplementableEvent)
		void SetDirectionAndReceiveInitDirection();
	
};
