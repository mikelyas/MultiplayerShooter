// Fill out your copyright notice in the Description page of Project Settings.


#include "MPShooterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"

#include "MultiplayerShooter/Weapon/Weapon.h"
#include "MultiplayerShooter/MPShooterComponents/CombatComponent.h"


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

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(RootComponent);

	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	Combat->SetIsReplicated(true);

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

void AMPShooterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AMPShooterCharacter, OverlappingWeapon, COND_OwnerOnly);
}

void AMPShooterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMPShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// called on the server
void AMPShooterCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(false);
	}
	OverlappingWeapon = Weapon;
	// if player is a server
	if (IsLocallyControlled())
	{
		if (OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickupWidget(true);
		}
	}
}

void AMPShooterCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}
	if (LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false);
	}
}

void AMPShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMPShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMPShooterCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &AMPShooterCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AMPShooterCharacter::LookUp);
	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &AMPShooterCharacter::EquipButtonPressed);
}

void AMPShooterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (Combat)
	{
		Combat->Character = this;
	}
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

void AMPShooterCharacter::EquipButtonPressed()
{
	if (Combat)
	{
		if (HasAuthority())
		{
			Combat->EquipWeapon(OverlappingWeapon);
		}
		else
		{
			ServerEquipButtonPressed();
		}
	}
}

void AMPShooterCharacter::ServerEquipButtonPressed_Implementation()
{
	if (Combat)
	{
		Combat->EquipWeapon(OverlappingWeapon);
	}
}

