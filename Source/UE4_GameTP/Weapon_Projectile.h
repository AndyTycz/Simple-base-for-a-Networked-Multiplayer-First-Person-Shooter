// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon_Projectile.generated.h"

class UDamageType;
class AUE4_GameTPProjectile;

UCLASS()
class UE4_GAMETP_API AWeapon_Projectile : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWeapon_Projectile();



protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		USkeletalMeshComponent* WeaponMesh;

	TSubclassOf<UDamageType> WeaponDamageType;

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<UCameraShake> FireCameraShake;

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<AUE4_GameTPProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Damage")
		float BaseDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Damage")
		float DmgMultiplierHead;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Damage")
		float DmgMultiplierTorso;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Damage")
		float DmgMultiplierLimbs;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Score")
		float ScoreHead;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Score")
		float ScoreTorso;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Score")
		float ScoreLimbs;

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerFire();


public:

	UFUNCTION(BlueprintCallable)
		void Fire();

	float GetWeaponDamageMultiplier(int Multiplier);
};
