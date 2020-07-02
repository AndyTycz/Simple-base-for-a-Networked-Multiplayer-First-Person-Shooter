// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon_Hitscan.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SkeletalMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "TPCharacter.h"
#include "Net/UnrealNetwork.h"

AWeapon_Hitscan::AWeapon_Hitscan()
{

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon Mesh"));
	RootComponent = WeaponMesh;


	BaseDamage = 15.0f;

	DmgMultiplierHead = 2.0f;
	DmgMultiplierLimbs = 0.75f;
	DmgMultiplierTorso = 1.25f;

	ScoreHead = 150.0f;
	ScoreLimbs = 25.0f;
	ScoreTorso = 75.0f;

	SetReplicates(true);

	NetUpdateFrequency = 66.0f;
	MinNetUpdateFrequency = 33.0f;
}

void AWeapon_Hitscan::Fire()
{

	if (Role < ROLE_Authority)
	{
		ServerFire(); //Si no tengo la autoridad, voy al implementation del server para que dispare del server side
							//Igual pasa el if y ejecuta el disparo para tener feedback visual
	}


	AActor* MyOwner = GetOwner();
	if (MyOwner)
	{
		FVector EyeLocation;
		FRotator EyeRotation;

		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector ShotDirection = EyeRotation.Vector();

		FVector TraceEnd = (EyeLocation + (ShotDirection * 10000));

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;
		QueryParams.bReturnPhysicalMaterial = true;

		FVector TraceEndPosition = TraceEnd; // -> Para el tray VFX

		FHitResult Hit;
		bool bHitFlesh = false;
		FRotator TraceRot;

		EPhysicalSurface SurfaceType = EPhysicalSurface(SurfaceType_Default);
		bHitFlesh = false;
		HitScanTrace.NetTraceTo = TraceEnd;
		HitScanTrace.NetPlayerScore = 0.0f;

		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, ECC_Visibility, QueryParams))
		{
			AActor* HitActor = Hit.GetActor();

			float WeaponDamage = 15.0f;
			// Consigo el Phys material de lo que le pego
			SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());
			// Consigo pos y roto el VFX de sangre
			TraceEndPosition = Hit.ImpactPoint;
			TraceRot = Hit.ImpactNormal.Rotation();
			TraceRot.Pitch -= 90.0f;

			switch (SurfaceType)
			{
			case SurfaceType1: //Torso
				WeaponDamage = 15.0f;
				bHitFlesh = true;
				PlayBloodEffect(TraceEndPosition, TraceRot, bHitFlesh);
				if (Role == ROLE_Authority)
				{
					HitScanTrace.NetPlayerScore = ScoreTorso;
					HitScanTrace.NetWeaponOwner = MyOwner;
				}
				break;
			case SurfaceType2: //Head
				WeaponDamage = 25.0f;
				bHitFlesh = true;
				PlayBloodEffect(TraceEndPosition, TraceRot, bHitFlesh);
				if (Role == ROLE_Authority)
				{
					HitScanTrace.NetWeaponOwner = MyOwner;
					HitScanTrace.NetPlayerScore = ScoreHead;
				}
				//SendScoreUpdate(ScoreHead, MyOwner);
				break;
			case SurfaceType3: //Limbs
				WeaponDamage = 8.0f;
				bHitFlesh = true;
				PlayBloodEffect(TraceEndPosition, TraceRot, bHitFlesh);
				if (Role == ROLE_Authority)
				{
					HitScanTrace.NetWeaponOwner = MyOwner;
					HitScanTrace.NetPlayerScore = ScoreLimbs;
				}
				//SendScoreUpdate(ScoreLimbs, MyOwner);
				break;

			default:
				break;
			}
			WeaponDamage += BaseDamage;
			UGameplayStatics::ApplyPointDamage(HitActor, WeaponDamage, ShotDirection, Hit, MyOwner->GetInstigatorController(), this, WeaponDamageType);
		}

		PlayShootingEffects(TraceEndPosition);

		if (Role == ROLE_Authority)
		{
			HitScanTrace.NetTraceTo = TraceEndPosition;
			HitScanTrace.NetVFXFinalRotator = TraceRot;
			HitScanTrace.bIsBloodHit = bHitFlesh;
		}
	}

}

void AWeapon_Hitscan::PlayShootingEffects(FVector TraceEnd)
{

	if (RaytraceEffect)
	{
		FVector MuzzlePos = WeaponMesh->GetSocketLocation(FName("Muzzle"));
		UParticleSystemComponent* RayComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), RaytraceEffect, MuzzlePos);
		if (RayComp)
		{
			RayComp->SetVectorParameter("Target", TraceEnd);
		}

	}

	APawn* MyOwner = Cast<APawn>(GetOwner());
	if (MyOwner)
	{
		APlayerController* PlController = Cast<APlayerController>(MyOwner->GetController());
		if (PlController)
		{
			PlController->ClientPlayCameraShake(FireCameraShake);
		}
	}
}

void AWeapon_Hitscan::PlayBloodEffect(FVector VFXPosition, FRotator VFXRotation, bool bHitFleshMat)
{
	if (ImpactEffect && bHitFleshMat)
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, VFXPosition, VFXRotation);
}


void AWeapon_Hitscan::SendScoreUpdate(float ScoreUpdate, AActor* MyOwner)
{
	ATPCharacter* MyPlayerOwner = Cast<ATPCharacter>(MyOwner);
	if (MyPlayerOwner)
		MyPlayerOwner->UpdatePlayerScore(ScoreUpdate);
}

void AWeapon_Hitscan::ServerFire_Implementation()
{
	Fire();
}

bool AWeapon_Hitscan::ServerFire_Validate()
{
	return true;
}

void AWeapon_Hitscan::OnRep_HitscanTrace()
{

	PlayShootingEffects(HitScanTrace.NetTraceTo);

	PlayBloodEffect(HitScanTrace.NetTraceTo, HitScanTrace.NetVFXFinalRotator, HitScanTrace.bIsBloodHit);
	SendScoreUpdate(HitScanTrace.NetPlayerScore, HitScanTrace.NetWeaponOwner);

}


void AWeapon_Hitscan::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, HitScanTrace, COND_SkipOwner); //Replica a todos menos al owner de esta arma, que ya ve el trace
}