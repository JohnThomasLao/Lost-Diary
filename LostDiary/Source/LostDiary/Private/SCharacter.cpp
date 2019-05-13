// Fill out your copyright notice in the Description page of Project Settings.

#include "SCharacter.h"
#include "Camera/CameraComponent.h"
#include "Camera/CameraShake.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Engine/GameEngine.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Automatically possess this character
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	// Disable the rotations that are based on the controller rotation
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure the character movement settings
	GetCharacterMovement()->bOrientRotationToMovement = true; // To rotate the character by turning it dynamically
	GetCharacterMovement()->RotationRate = FRotator(0.f, 50.f, 0.f);
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true; // Since crouching ability is disabled by default
	GetCharacterMovement()->MaxWalkSpeed = 550.f;
	GetCharacterMovement()->MaxWalkSpeedCrouched = 300.f;

	// Initialize components and variables
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 300.f; // Distance from the character to camera comp
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->bEnableCameraLag = true; // Moves camera in a realistic pace
	SpringArmComp->CameraLagSpeed = 4.f;
	SpringArmComp->bEnableCameraRotationLag = true; // Rotates camera in a realistic pace
	SpringArmComp->CameraRotationLagSpeed = 10.f;
	SpringArmComp->CameraLagMaxDistance = 10.f;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName); // Attach to the end of the spring arm comp
	CameraComp->bUsePawnControlRotation = false;

}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Get player controller of the player character
	PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CalculateCharacterDirection(InputX, InputY);

	// Debug
	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Green, FString::Printf(TEXT("Direction: %f"), Direction));
	}

}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Setup gameplay bindings
	// Character //////////////////////////////////////////////////////////////////////////
	PlayerInputComponent->BindAxis("MoveX", this, &ASCharacter::MoveX);
	PlayerInputComponent->BindAxis("MoveY", this, &ASCharacter::MoveY);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ASCharacter::StartSprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ASCharacter::StopSprint);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ASCharacter::ToggleCrouch);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASCharacter::StartJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ASCharacter::StopJump);

	// Camera /////////////////////////////////////////////////////////////////////////////
	PlayerInputComponent->BindAxis("CameraX", this, &ASCharacter::CameraX);
	PlayerInputComponent->BindAxis("CameraY", this, &ASCharacter::CameraY);

}

/* Character's input binding functions */
void ASCharacter::MoveX(float Value)
{
	InputX = Value;

	// Debug
	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Blue, FString::Printf(TEXT("InputX: %f"), InputX));
	}

	if (Controller != NULL && Value != 0.f) {
		if (bIsSprinting) {
			Value *= 2;
		}

		// Determine the foward direction
		FRotator NewYawRotation = FRotator(0.f, GetControlRotation().Yaw, 0.f);

		// Get forward direction
		FVector Direction = UKismetMathLibrary::GetForwardVector(NewYawRotation);

		// Add forward direction to charcater movement input
		AddMovementInput(Direction, Value);
	}
}

void ASCharacter::MoveY(float Value)
{
	InputY = Value;

	// Debug
	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Blue, FString::Printf(TEXT("InputY: %f"), InputY));
	}

	if (Controller != NULL && Value != 0.f) {
		if (bIsSprinting) {
			Value *= 2;
		}

		// Determine the right direction
		FRotator NewYawRotation = FRotator(0.f, GetControlRotation().Yaw, 0.f);

		// Get right direction
		FVector Direction = UKismetMathLibrary::GetRightVector(NewYawRotation);

		// Add right direction to charcater movement input
		AddMovementInput(Direction, Value);
	}
}

void ASCharacter::StartSprint()
{
	bIsSprinting = true;

	if (!GetCharacterMovement()->IsCrouching()) { // Only play cam shake when character is not crouching
		if (InputX != 0.f || InputY != 0.f) {
			if (PlayerController != NULL) {
				PlayerController->ClientPlayCameraShake(SprintCamShake);
			}
		}
	}
}

void ASCharacter::StopSprint()
{
	bIsSprinting = false;

	if (PlayerController != NULL) {
		PlayerController->ClientStopCameraShake(SprintCamShake);
	}

}

void ASCharacter::ToggleCrouch()
{
	if (GetCharacterMovement()->IsCrouching()) { // If character is found already in crouching state
		UnCrouch();
	}
	else {
		Crouch();
	}
}

void ASCharacter::StartJump()
{
	Jump();

}

void ASCharacter::StopJump()
{
	StopJumping();

}

/* Camera's input binding functions */
void ASCharacter::CameraX(float Value)
{
	AddControllerYawInput(Value);

}

void ASCharacter::CameraY(float Value)
{
	AddControllerPitchInput(Value);

}

/* Calculate move-to direction to drive root motion - thus rotating character using calculated direction */
void ASCharacter::CalculateCharacterDirection(float InputX, float InputY)
{
	// Make input vector of X and Y values of character input values
	FVector InputVector = UKismetMathLibrary::MakeVector(InputX, InputY * -1.f, 0.f);

	// Make rotation using X value of input vector
	FRotator InputRotation = UKismetMathLibrary::MakeRotFromX(InputVector);

	// Make rotation of camera comp and capsule comp of the character
	FRotator CompRotation = UKismetMathLibrary::NormalizedDeltaRotator(CameraComp->GetComponentRotation(), GetCapsuleComponent()->GetComponentRotation());

	// Make direction rotattion based on component rotation and input rotation
	FRotator DirectionRotation = UKismetMathLibrary::NormalizedDeltaRotator(CompRotation, InputRotation);

	// Assign direction as yaw value of the direction rotation
	Direction = DirectionRotation.Yaw;

}

