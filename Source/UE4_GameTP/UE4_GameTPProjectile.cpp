// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "UE4_GameTPProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"
#include "RangedEnemy.h"
#include "TPCharacter.h"
#include "Weapon_Projectile.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Kismet/GameplayStatics.h"

AUE4_GameTPProjectile::AUE4_GameTPProjectile()
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");

	CollisionComp->AlwaysLoadOnClient = true;
	CollisionComp->AlwaysLoadOnServer = true;

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;
	CollisionComp->bReturnMaterialOnMove = true;
	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 15000.f;
	ProjectileMovement->MaxSpeed = 15000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;


	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;

	ProjectileDamage = 20.f;
	ProjectileDamageHead = 35.f;
	ProjectileDamageLimbs = 10.f;

	ProjectileDamageEnemy = 10.f;


	bReplicates = true;
	SetReplicatingMovement(true);

}

void AUE4_GameTPProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	//CollisionComp->OnComponentHit.AddDynamic(this, &AUE4_GameTPProjectile::OnHit);		// set up a notification for when this component hits something blocking

	/*AActor* MyOwner = GetOwner();
	if (MyOwner)
	{
		ARangedEnemy* EnemyOwner = Cast<ARangedEnemy>(MyOwner->GetOwner());
		if (EnemyOwner)
		{
			UE_LOG(LogTemp, Warning, TEXT("Set enemy damage"));
			ProjectileDamage = 10.0f;
		}

		AWeapon_Projectile* PlayerOwner = Cast<AWeapon_Projectile>(MyOwner);
		if (PlayerOwner)
		{
			UE_LOG(LogTemp, Warning, TEXT("Set player damage"));
			ProjectileDamage = PlayerOwner->GetWeaponDamageMultiplier(0, ProjectileDamage);
			ProjectileDamageHead = PlayerOwner->GetWeaponDamageMultiplier(1, ProjectileDamage);
			ProjectileDamageLimbs = PlayerOwner->GetWeaponDamageMultiplier(2, ProjectileDamage);
		}

	}*/
	ProjectileMovement->OnProjectileStop.AddDynamic(this, &AUE4_GameTPProjectile::OnImpact);
	CollisionComp->MoveIgnoreActors.Add(GetInstigator());

	SetLifeSpan(3.0f);
	//MyController = GetInstigatorController();
}

void AUE4_GameTPProjectile::OnImpact(const FHitResult& HitResult)
{
	if (Role == ROLE_Authority)
	{
		AActor* MyOwner = GetOwner();
		const FVector NudgedImpactLocation = HitResult.ImpactPoint + HitResult.ImpactNormal * 10.0f;
		ARangedEnemy* EnemyHit = Cast<ARangedEnemy>(HitResult.GetActor());
		EPhysicalSurface SurfaceType = EPhysicalSurface(SurfaceType_Default);
		if (EnemyHit && MyOwner)
		{

			SurfaceType = UPhysicalMaterial::DetermineSurfaceType(HitResult.PhysMaterial.Get());
			float dmg;
			if (SurfaceType)
			{
				switch (SurfaceType)
				{
				case SurfaceType1: //Torso
					dmg = ProjectileDamage;

					break;
				case SurfaceType2: //Head
					dmg = ProjectileDamageHead;

					break;
				case SurfaceType3: //Limbs
					dmg = ProjectileDamageLimbs;
					break;

				default:
					break;
				}
			}
			UE_LOG(LogTemp, Warning, TEXT("Hit Enemy with Damage %f"), dmg);
			AActor* DamagedActor = HitResult.GetActor();
			if (DamagedActor)
			{
				UGameplayStatics::ApplyPointDamage(DamagedActor, dmg, NudgedImpactLocation, HitResult, GetOwner()->GetInstigatorController(), GetOwner(), ProjectileDamageType);
				Destroy();
			}
		}

		ATPCharacter* PlayerHit = Cast<ATPCharacter>(HitResult.GetActor());
		if (PlayerHit)
		{
			UE_LOG(LogTemp, Warning, TEXT("Hit PLAYER DAMAGE: 10"));
			AActor* DamagedActor = HitResult.GetActor();
			if (DamagedActor)
			{
				UGameplayStatics::ApplyPointDamage(DamagedActor, ProjectileDamageEnemy, NudgedImpactLocation, HitResult, GetOwner()->GetInstigatorController(), GetOwner(), ProjectileDamageType);
				Destroy();
			}

		}
	}


	Destroy();
}

//void AUE4_GameTPProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
//{
//	EPhysicalSurface SurfaceType = EPhysicalSurface(SurfaceType_Default);
//
//	// Only add impulse and destroy projectile if we hit a physics
//	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && OtherComp->IsSimulatingPhysics())
//	{
//		OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());
//	}
//	if (GetLocalRole() == ROLE_Authority)
//	{
//#pragma region FailToHitEnemy
//
//		//AActor* MyOwner = GetOwner();
//		//ARangedEnemy* EnemyHit = Cast<ARangedEnemy>(OtherActor);
//		//if (EnemyHit)
//		//{
//		//	SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());
//		//	float dmg = 0.0f;
//		//	if (SurfaceType)
//		//	{
//		//		switch (SurfaceType)
//		//		{
//		//		case SurfaceType1: //Torso
//		//			dmg = ProjectileDamage;
//
//		//			break;
//		//		case SurfaceType2: //Head
//		//			dmg = ProjectileDamageHead;
//
//		//			break;
//		//		case SurfaceType3: //Limbs
//		//			dmg = ProjectileDamageLimbs;
//		//			break;
//
//		//		default:
//		//			break;
//		//		}
//		//	}
//
//
//		//	UE_LOG(LogTemp, Warning, TEXT("HitEnemy"));
//			//UGameplayStatics::ApplyPointDamage(EnemyHit, dmg, FVector::ZeroVector, Hit, MyOwner->GetInstigatorController(), MyOwner->GetOwner(), ProjectileDamageType);
//		//	UGameplayStatics::ApplyDamage(OtherActor, dmg, MyOwner->GetInstigatorController(), MyOwner, ProjectileDamageType);
//		/*	FDamageEvent DmgEvent;
//			DmgEvent.DamageTypeClass = ProjectileDamageType;
//			OtherActor->TakeDamage(dmg, DmgEvent, MyOwner->GetInstigatorController(), MyOwner->GetOwner());*/
//
//			//}
//#pragma endregion
//
//
//		ATPCharacter* PlayerHit = Cast<ATPCharacter>(OtherActor);
//		if (PlayerHit)
//		{
//		const FVector NudgedImpactLocation = Hit.ImpactPoint + Hit.ImpactNormal * 10.0f;
//			UGameplayStatics::ApplyPointDamage(PlayerHit, ProjectileDamage, NudgedImpactLocation, Hit, GetOwner()->GetInstigatorController(), this, ProjectileDamageType);
//			//UGameplayStatics::ApplyRadialDamage(this, ProjectileDamage, NudgedImpactLocation, 50.0f, ProjectileDamageType, TArray<AActor*>(), this, GetOwner()->GetInstigatorController());
//		}
//
//	}
//	Destroy();
//}


void AUE4_GameTPProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ProjectileDamage);
	DOREPLIFETIME(ThisClass, ProjectileDamageHead);
	DOREPLIFETIME(ThisClass, ProjectileDamageLimbs);
	DOREPLIFETIME(ThisClass, ProjectileDamageEnemy);

}