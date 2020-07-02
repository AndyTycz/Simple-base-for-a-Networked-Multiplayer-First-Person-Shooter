// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon_Projectile.h"
#include "Components/SkeletalMeshComponent.h"
#include "UE4_GameTPProjectile.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AWeapon_Projectile::AWeapon_Projectile()
{
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon Mesh"));
	RootComponent = WeaponMesh;

	BaseDamage = 25.0f;

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



void AWeapon_Projectile::Fire()
{

	ServerFire(); //Si no tengo la autoridad, voy al implementation del server para que dispare del server side
						//Igual pasa el if y ejecuta el disparo para tener feedback visual


}

void AWeapon_Projectile::ServerFire_Implementation()
{
	if (Role < ROLE_Authority) return;

	if (ProjectileClass)
	{
		FVector MuzzlePos = WeaponMesh->GetSocketLocation(FName("ProjectileMuzzle"));
		FRotator MuzzleRot = WeaponMesh->GetSocketRotation(FName("ProjectileMuzzle"));

		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		ActorSpawnParams.Instigator = Cast<APawn>(GetOwner());
		AActor* MyOwner = this;


		AUE4_GameTPProjectile* NewProjectile = GetWorld()->SpawnActor<AUE4_GameTPProjectile>(ProjectileClass, MuzzlePos, MuzzleRot, ActorSpawnParams);
		NewProjectile->SetOwner(MyOwner);


	}

}

bool AWeapon_Projectile::ServerFire_Validate()
{
	return true;
}

//float AWeapon_Projectile::GetWeaponDamageMultiplier(int Multiplier, float ProyectileDamage)
//{
//	if (Multiplier == 0)
//	{
//		return ProyectileDamage * DmgMultiplierTorso;
//	}
//	if (Multiplier == 1)
//	{
//		return ProyectileDamage * DmgMultiplierHead;
//	}
//	if (Multiplier == 2)
//	{
//		return ProyectileDamage * DmgMultiplierLimbs;
//	}
//
//	return ProyectileDamage;
//}