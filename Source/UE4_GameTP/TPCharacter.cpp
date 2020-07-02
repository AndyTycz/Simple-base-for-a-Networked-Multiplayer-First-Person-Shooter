// Fill out your copyright notice in the Description page of Project Settings.


#include "TPCharacter.h"
#include "UE4_GameTPProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "HealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Weapon_Hitscan.h"
#include "Weapon_Projectile.h"

// Sets default values
ATPCharacter::ATPCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	GetCharacterMovement()->AirControl = 0.75f;

	MyHealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health Component"));

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetOwnerNoSee(false);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	Mesh3P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh3P"));
	Mesh3P->SetOnlyOwnerSee(false);
	Mesh3P->SetOwnerNoSee(true);
	Mesh3P->SetupAttachment(GetCapsuleComponent());
	Mesh3P->CastShadow = true;

}

// Called when the game starts or when spawned
void ATPCharacter::BeginPlay()
{
	Super::BeginPlay();

	Mesh1P->SetHiddenInGame(false, true);

	MyHealthComponent->OnHealthChanged.AddDynamic(this, &ATPCharacter::OnHealthChanged);

	if (Role == ROLE_Authority)
	{
		PlayerCurrentScore = 0.0f;

		//Spawn con arma default
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;


		CurrentProjectileWeapon = GetWorld()->SpawnActor<AWeapon_Projectile>(ProjectileWeapon, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if (CurrentProjectileWeapon)
		{
			CurrentProjectileWeapon->SetOwner(this);
			CurrentProjectileWeapon->SetInstigator(this);
			CurrentProjectileWeapon->AttachToComponent(Mesh1P, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "GripPoint");
			CurrentProjectileWeapon->SetActorHiddenInGame(true);
			bCanShootSecondary = false;

		}
		CurrentHSWeapon = GetWorld()->SpawnActor<AWeapon_Hitscan>(StartWeapon, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if (CurrentHSWeapon)
		{
			CurrentHSWeapon->SetOwner(this);
			CurrentHSWeapon->AttachToComponent(Mesh1P, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "GripPoint");
		}



		bDied = false;
	}


}

// Called to bind functionality to input
void ATPCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{

	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ATPCharacter::OnFire);
	PlayerInputComponent->BindAction("Change Weapon", IE_Pressed, this, &ATPCharacter::OnChangeWeapon);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &ATPCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATPCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ATPCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ATPCharacter::LookUpAtRate);
}



void ATPCharacter::OnFire()
{
	// try and play the sound if specified
	if (FireSound != NULL)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	// try and play a firing animation if specified
	if (FireAnimation != NULL)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
		if (AnimInstance != NULL)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}

	if (!bCanShootSecondary)
	{
		if (CurrentHSWeapon)
		{
			CurrentHSWeapon->Fire();
		}
	}
	else
	{
		if (CurrentProjectileWeapon)
		{
			CurrentProjectileWeapon->Fire();
		}
	}
}

void ATPCharacter::OnChangeWeapon()
{

	bCanShootSecondary = !bCanShootSecondary;

}

void ATPCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ATPCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ATPCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ATPCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}


FVector ATPCharacter::GetPawnViewLocation() const
{

	if (FirstPersonCameraComponent)
		return FirstPersonCameraComponent->GetComponentLocation();


	return Super::GetPawnViewLocation();  //if fails fallback
}



void ATPCharacter::OnHealthChanged(UHealthComponent* HealtComp, float Health, float HealthChange, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{

	if (Health <= 0.0f && !bDied)
	{
		//i ded
		bDied = true;
		CurrentHSWeapon->SetHidden(true);
		CurrentProjectileWeapon->SetHidden(true);

		GetMovementComponent()->StopMovementImmediately();
		Mesh3P->SetCollisionProfileName(TEXT("Ragdoll"));
		SetActorEnableCollision(true);
		Mesh3P->SetAllBodiesSimulatePhysics(true);
		Mesh3P->SetSimulatePhysics(true);
		Mesh3P->WakeAllRigidBodies();
		Mesh3P->bBlendPhysics = true;
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);


		LifeCount--;

		DetachFromControllerPendingDestroy();
		SetLifeSpan(5.0f);
	}
}

void ATPCharacter::UpdatePlayerScore(float Score)
{
	PlayerCurrentScore += Score;
	UE_LOG(LogTemp, Warning, TEXT("Player Score is: %f"), PlayerCurrentScore);
}

int ATPCharacter::GetLifeCount() const
{
	return LifeCount;
}
void ATPCharacter::SetLifeCount(int RemoveLifeAmount)
{

	LifeCount = FMath::Clamp(LifeCount - RemoveLifeAmount, 0, 3);
}


void ATPCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, CurrentHSWeapon);
	DOREPLIFETIME(ThisClass, CurrentProjectileWeapon);
	DOREPLIFETIME(ThisClass, bDied);
	DOREPLIFETIME(ThisClass, bCanShootSecondary);
	DOREPLIFETIME(ThisClass, LifeCount);
}