// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy_Melee.h"
#include "Perception/PawnSensingComponent.h"
#include "DrawDebugHelpers.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "HealthComponent.h"
#include "TPCharacter.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AEnemy_Melee::AEnemy_Melee()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	PawnSenses = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("Pawn Senses"));


	PawnSenses->OnSeePawn.AddDynamic(this, &AEnemy_Melee::OnPawnSeen);

	//HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
//	HealthComponent->OnHealthChanged.AddDynamic(this, &AEnemy_Melee::HandleTakeDamage);


	
}

// Called when the game starts or when spawned
void AEnemy_Melee::BeginPlay()
{
	Super::BeginPlay();

	bIsInAttackRange = false;
}

void AEnemy_Melee::OnPawnSeen(APawn * SeenPawn)
{
	if (SeenPawn == nullptr) return;

	if (SeenPawn)
	{
		ATPCharacter* Player = Cast<ATPCharacter>(SeenPawn);
		{
			if (Player)
			{
				PlayerTarget = Player;
				bIsInAttackRange = true;

				AttackPlayer();
			}
		}
	}

	DrawDebugSphere(GetWorld(), SeenPawn->GetActorLocation(), 32.0f, 8, FColor::Red, false, 7.0f);

}

void AEnemy_Melee::HandleTakeDamage(UHealthComponent * HealtComp, float Health, float HealthChange, const UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser)
{

	if (MatInstance == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Enemy Mat creation"));
		MatInstance = GetMesh()->CreateAndSetMaterialInstanceDynamicFromMaterial(0, GetMesh()->GetMaterial(0));
	}
	if (MatInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("Enemy Mat Scalar value mat"));
		MatInstance->SetScalarParameterValue("DamageTakenTimer", GetWorld()->TimeSeconds);
	}


	UE_LOG(LogTemp, Warning, TEXT("Enemy HP (should shine): %f"), Health);

	if (Health <= 0.0f)
	{
		UE_LOG(LogTemp, Warning, TEXT("Entro a 0"));

		//GetMovementComponent()->StopMovementImmediately();
		GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
		SetActorEnableCollision(true);
		GetMesh()->SetAllBodiesSimulatePhysics(true);
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->WakeAllRigidBodies();
		GetMesh()->bBlendPhysics = true;
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);


		DetachFromControllerPendingDestroy();
		SetLifeSpan(10.0f);
	}
}

void AEnemy_Melee::AttackPlayer()
{


	UE_LOG(LogTemp, Warning, TEXT("Enemy does pium pium"));
}

FRotator AEnemy_Melee::GetLookAtYaw(FVector Target)
{
	FRotator GetLookAtRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target);
	return FRotator(0.f, GetLookAtRot.Yaw, 0.f);
}


// Called every frame
void AEnemy_Melee::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsInAttackRange)
	{
		if (PlayerTarget)
		{
			FRotator LookAt = GetLookAtYaw(PlayerTarget->GetActorLocation());
			FRotator AimRotation = FMath::RInterpTo(GetActorRotation(), LookAt, DeltaTime, 15.f);
			SetActorRotation(AimRotation);
		}
	}

}