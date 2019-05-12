// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

class UCameraComponent;
class UCameraShake;
class USpringArmComponent;

UCLASS()
class LOSTDIARY_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Spring arm component to control the camera comp
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
		USpringArmComponent* SpringArmComp;

	// Camera component
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
		UCameraComponent* CameraComp;

	// Refs to camera shake - used when character sprints, jumps, fires weapon, etc.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera)
		TSubclassOf<UCameraShake> SprintCamShake;

	// Ref to player controller
	APlayerController* PlayerController;

	// Input binding functions
	// Character
	void MoveX(float Value);
	void MoveY(float Value);

	void StartSprint();
	void StopSprint();
	void ToggleCrouch();
	void StartJump();
	void StopJump();

	// Camera 
	void CameraX(float Value);
	void CameraY(float Value);

	// Called to calculate the character direction
	void CalculateCharacterDirection(float InputX, float InputY);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Charcater input values
	UPROPERTY(BlueprintReadWrite)
		float InputX;

	UPROPERTY(BlueprintReadWrite)
		float InputY;

	UPROPERTY(BlueprintReadWrite)
		float Direction;

	UPROPERTY(BlueprintReadWrite)
		float MovementSpeed;

	// If character is sprinting
	bool bIsSprinting;

};
