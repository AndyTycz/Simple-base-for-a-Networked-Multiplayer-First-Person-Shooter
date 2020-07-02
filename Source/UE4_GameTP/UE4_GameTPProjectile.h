// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UE4_GameTPProjectile.generated.h"

UCLASS(config = Game)
class AUE4_GameTPProjectile : public AActor
{
	GENERATED_BODY()

		virtual void PostInitializeComponents() override;
	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
		class USphereComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
		class UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditDefaultsOnly, Replicated)
		float ProjectileDamage;

	UPROPERTY(EditDefaultsOnly, Replicated)
		float ProjectileDamageHead;

	UPROPERTY(EditDefaultsOnly, Replicated)
		float ProjectileDamageLimbs;

	UPROPERTY(EditDefaultsOnly, Replicated)
		float ProjectileDamageEnemy;

public:
	AUE4_GameTPProjectile();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TSubclassOf<UDamageType> ProjectileDamageType;

	/** called when projectile hits something */
	//UFUNCTION()
	//	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	UFUNCTION()
		void OnImpact(const FHitResult& HitResult);
	/** Returns CollisionComp subobject **/
	FORCEINLINE class USphereComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ProjectileMovement subobject **/
	FORCEINLINE class UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }
};

