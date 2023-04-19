// Fill out your copyright notice in the Description page of Project Settings.


#include "MPShooterAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include "MPShooterCharacter.h"
#include "MultiplayerShooter/Weapon/Weapon.h"


void UMPShooterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	MPShooterCharacter = Cast<AMPShooterCharacter>(TryGetPawnOwner());
	WeaponSocketLeftHand = FName("LeftHandSocket");
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
	bWeaponEquipped = MPShooterCharacter->IsWeaponEquipped();
	EquippedWeapon = MPShooterCharacter->GetEquippedWeapon();
	bIsCrouched = MPShooterCharacter->bIsCrouched;
	bIsAiming = MPShooterCharacter->IsAiming();

	// Offset Yaw for Strafing
	FRotator AimRotation = MPShooterCharacter->GetBaseAimRotation();
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(MPShooterCharacter->GetVelocity());
	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
	DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaTime, 6.f);
	YawOffset = DeltaRotation.Yaw;

	FRotator CharacterRotation = MPShooterCharacter->GetActorRotation();
	const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
	const float Target = Delta.Yaw / DeltaTime;
	const float Interp = FMath::FInterpTo(Lean, Target, DeltaTime, 6.f);
	Lean = FMath::Clamp(Interp, -90.f, 90.f);
	CharacterRotationLastFrame = CharacterRotation;

	AO_Yaw = MPShooterCharacter->GetAO_Yaw();
	AO_Pitch = MPShooterCharacter->GetAO_Pitch();

	if (bWeaponEquipped && EquippedWeapon && EquippedWeapon->GetWeaponMesh() && MPShooterCharacter->GetMesh())
	{
		LeftHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(WeaponSocketLeftHand, ERelativeTransformSpace::RTS_World);
		FVector OutLocation;
		FRotator OutRotation;
		MPShooterCharacter->GetMesh()->TransformToBoneSpace("hand_r", LeftHandTransform.GetLocation(), FRotator(0.f), OutLocation, OutRotation);
		LeftHandTransform.SetLocation(OutLocation);
		LeftHandTransform.SetRotation(FQuat(OutRotation));
	}
}