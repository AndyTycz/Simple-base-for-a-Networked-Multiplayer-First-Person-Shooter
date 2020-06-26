// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RangedEnemy.generated.h"

class UPawnSensingComponent;
class UHealthComponent;
class ATPCharacter;
class USphereComponent;
class AAIController;
class AWeapon_Projectile;

UCLASS()
class UE4_GAMETP_API ARangedEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ARangedEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;



	UPROPERTY(VisibleAnywhere)
		UPawnSensingComponent* PawnSenses;

	UPROPERTY(VisibleDefaultsOnly)
		UHealthComponent* HealthComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
		USphereComponent* AttackSphere;

	UPROPERTY(EditDefaultsOnly)
		float SphereRadius;

	UPROPERTY(Replicated)
		AWeapon_Projectile* CurrentProjectileWeapon;
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<AWeapon_Projectile> ProjectileWeapon;

	UFUNCTION()
		virtual void AttackSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
		virtual void AttackSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	void MoveToTarget(class ATPCharacter* Target);



	UFUNCTION()
		void OnPawnSeen(APawn* SeenPawn);

	UFUNCTION()
		void HandleTakeDamage(UHealthComponent* HealtComp, float Health, float HealthChange, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UMaterialInstanceDynamic* MatInstance;

	void AttackPlayer();

	//UFUNCTION(BlueprintCallable)
	//	void AttackTimer();

	FRotator GetLookAtYaw(FVector Target);

	bool bIsInAttackRange;

	bool bCanShoot;

	ATPCharacter* PlayerTarget;


	AAIController* AIController;

	FTimerHandle ShootTimer;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
