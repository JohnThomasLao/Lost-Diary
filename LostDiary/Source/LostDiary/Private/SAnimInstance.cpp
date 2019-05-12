// Fill out your copyright notice in the Description page of Project Settings.

#include "SAnimInstance.h"
#include "SCharacter.h"
#include "Engine/GameEngine.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

void USAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	APawn* OwnerPawn = TryGetPawnOwner();

	if (OwnerPawn) {
		// Update animation properties
		bIsFalling = OwnerPawn->GetMovementComponent()->IsFalling();

		// Cast owner pawn to owner character
		OwnerCharacter = Cast<ASCharacter>(OwnerPawn);

		if (OwnerCharacter != NULL) {
			InputX_Anim = OwnerCharacter->InputX; // Set animation Fwd / Bwd
			InputY_Anim = OwnerCharacter->InputY; // Set animation Lft / Rgt

			bCrouching = OwnerCharacter->GetCharacterMovement()->IsCrouching();

			if (OwnerCharacter->bIsSprinting) {
				MovementSpeed = FMath::Clamp(UKismetMathLibrary::Abs(InputX_Anim) + UKismetMathLibrary::Abs(InputY_Anim), 0.f, 1.f) * 2.f;
			}
			else {
				MovementSpeed = FMath::Clamp(UKismetMathLibrary::Abs(InputX_Anim) + UKismetMathLibrary::Abs(InputY_Anim), 0.f, 1.f);
			}

			// Debug
			if (GEngine) {
				GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Red, FString::Printf(TEXT("Speed (Anim): %f"), MovementSpeed));
			}

			if (MovementSpeed > 0.01f) { // If speed > 0.01 then set direction input to anim
				if (bReceivedInitialDirection) {
					if (MovementSpeed < 0.01f) { // If speed < 0.01 then reset received init direction value for next event
						bReceivedInitialDirection = false;
					}
				}
				else {
					SetDirectionAndReceiveInitDirection();
				}
			}
			else {
				if (MovementSpeed < 0.01f) { // If speed < 0.01 then reset received init direction value for next event
					bReceivedInitialDirection = false;
				}
			}
		}
	}
}