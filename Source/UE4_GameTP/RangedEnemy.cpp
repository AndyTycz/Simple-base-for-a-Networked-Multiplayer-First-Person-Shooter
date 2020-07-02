// Fill out your copyright notice in the Description page of Project Settings.


#include "RangedEnemy.h"
#include "Perception/PawnSensingComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "HealthComponent.h"
#include "TPCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "AIController.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"
#include "Weapon_Projectile.h"

// Sets default values
ARangedEnemy::ARangedEnemy()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PawnSenses = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("Pawn Senses"));


	PawnSenses->OnSeePawn.AddDynamic(this, &ARangedEnemy::OnPawnSeen);

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	HealthComponent->OnHealthChanged.AddDynamic(this, &ARangedEnemy::HandleTakeDamage);


	AttackSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AttackSphere"));
	AttackSphere->InitSphereRadius(SphereRadius);
	AttackSphere->SetupAttachment(RootComponent);

	AIController = Cast<AAIController>(GetController());
	bIsAlive = true;
}

// Called when the game starts or when spawned
void ARangedEnemy::BeginPlay()
{
	Super::BeginPlay();

	AttackSphere->OnComponentBeginOverlap.AddDynamic(this, &ARangedEnemy::AttackSphereOnOverlapBegin);
	AttackSphere->OnComponentEndOverlap.AddDynamic(this, &ARangedEnemy::AttackSphereOnOverlapEnd);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	if (Role == ROLE_Authority)
	{
		CurrentProjectileWeapon = GetWorld()->SpawnActor<AWeapon_Projectile>(ProjectileWeapon, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if (CurrentProjectileWeapon)
		{
			CurrentProjectileWeapon->SetOwner(this);
			CurrentProjectileWeapon->SetInstigator(this);
			CurrentProjectileWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "GripPoint");
		}
	}
}

// Called every frame
void ARangedEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsInAttackRange && bCanSeePlayer)
	{
		if (PlayerTarget)
		{
			FRotator LookAt = GetLookAtYaw(PlayerTarget->GetActorLocation());
			FRotator AimRotation = FMath::RInterpTo(GetActorRotation(), LookAt, DeltaTime, 15.f);
			SetActorRotation(AimRotation);
		}
	}
}


void ARangedEnemy::OnPawnSeen(APawn * SeenPawn)
{
	if (SeenPawn == nullptr)
	{
		PlayerTarget = nullptr;
		bCanSeePlayer = false;
		return;
	}


	if (SeenPawn)
	{
		ATPCharacter* Player = Cast<ATPCharacter>(SeenPawn);

		if (Player)
		{
			bCanSeePlayer = true;
			PlayerTarget = Player;
			bIsInAttackRange = true;


			MoveToTarget(PlayerTarget);
		}

	}

}

void ARangedEnemy::MoveToTarget(ATPCharacter* Target)
{
	if (Role == ROLE_Authority)
	{
		if (AIController && bIsAlive)
		{
			FAIMoveRequest MoveRequest;
			MoveRequest.SetGoalActor(Target);
			MoveRequest.SetAcceptanceRadius(10.f);

			FNavPathSharedPtr NavPath;
			AIController->MoveTo(MoveRequest, &NavPath);

		}
	}
}

void ARangedEnemy::AttackSphereOnOverlapBegin(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor && OtherActor == PlayerTarget)
	{
		GetWorld()->GetTimerManager().SetTimer(ShootTimer, this, &ARangedEnemy::AttackPlayer, 1.0f, true, 0.5f);

		bCanShoot = true;

		if (!bCanSeePlayer)
		{
			ATPCharacter* Player = Cast<ATPCharacter>(OtherActor);
			if (Player)
			{
				AIController->StopMovement();
				PlayerTarget = Player;
				bIsInAttackRange = true;
				bCanSeePlayer = true;


			}
		}
	}
}

void ARangedEnemy::AttackSphereOnOverlapEnd(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == PlayerTarget)
	{
		PlayerTarget = nullptr;
		GetWorld()->GetTimerManager().ClearTimer(ShootTimer);
		bCanShoot = false;
	}

}

void ARangedEnemy::HandleTakeDamage(UHealthComponent * HealtComp, float Health, float HealthChange, const UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser)
{

	if (bIsAlive)
	{
		if (MatInstance == nullptr)
		{

			MatInstance = GetMesh()->CreateAndSetMaterialInstanceDynamicFromMaterial(0, GetMesh()->GetMaterial(0));
		}
		if (MatInstance)
		{
			MatInstance->SetScalarParameterValue("DamageTakenTimer", GetWorld()->TimeSeconds);
		}
	}

	if (Health <= 0.0f)
	{

		bIsAlive = false;
		bCanShoot = false;

		//GetMovementComponent()->StopMovementImmediately();
		GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
		SetActorEnableCollision(true);
		GetMesh()->SetAllBodiesSimulatePhysics(true);
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->WakeAllRigidBodies();
		GetMesh()->bBlendPhysics = true;
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		CurrentProjectileWeapon->SetHidden(true);
		DetachFromControllerPendingDestroy();
		SetLifeSpan(5.0f);

		SetLifeSpan(10.0f);
	}
}


void ARangedEnemy::AttackPlayer()
{
	if (CurrentProjectileWeapon && bIsAlive)
	{
		if (bCanShoot)
			CurrentProjectileWeapon->Fire();
	}
}

FRotator ARangedEnemy::GetLookAtYaw(FVector Target)
{
	FRotator GetLookAtRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target);
	return FRotator(0.f, GetLookAtRot.Yaw, 0.f);
}

void ARangedEnemy::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, CurrentProjectileWeapon);
}