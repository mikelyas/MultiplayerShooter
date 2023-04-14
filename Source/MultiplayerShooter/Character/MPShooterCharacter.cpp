// Fill out your copyright notice in the Description page of Project Settings.


#include "MPShooterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


AMPShooterCharacter::AMPShooterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 700.f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

void AMPShooterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMPShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMPShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMPShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMPShooterCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &AMPShooterCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AMPShooterCharacter::LookUp);
}

void AMPShooterCharacter::MoveForward(float Axis)
{
	if (Controller != nullptr && Axis != 0.f)
	{
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));
		AddMovementInput(Direction, Axis);
	}
}

void AMPShooterCharacter::MoveRight(float Axis)
{
	if (Controller != nullptr && Axis != 0.f)
	{
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));
		AddMovementInput(Direction, Axis);
	}
}

void AMPShooterCharacter::Turn(float Axis)
{
	AddControllerYawInput(Axis);
}

void AMPShooterCharacter::LookUp(float Axis)
{
	AddControllerPitchInput(Axis);
}

