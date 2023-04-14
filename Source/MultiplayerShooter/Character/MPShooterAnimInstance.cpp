// Fill out your copyright notice in the Description page of Project Settings.


#include "MPShooterAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "MPShooterCharacter.h"


void UMPShooterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	MPShooterCharacter = Cast<AMPShooterCharacter>(TryGetPawnOwner());
}

void UMPShooterAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (MPShooterCharacter == nullptr)
	{
		MPShooterCharacter = Cast<AMPShooterCharacter>(TryGetPawnOwner());
	}

	if (MPShooterCharacter == nullptr) return;

	FVector Velocity = MPShooterCharacter->GetVelocity();
	Velocity.Z = 0.f;
	Speed = Velocity.Size();

	bIsInAir = MPShooterCharacter->GetCharacterMovement()->IsFalling();

	bIsAccelerating = MPShooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() - SMALL_NUMBER > 0.f;
}