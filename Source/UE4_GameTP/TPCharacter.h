// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TPCharacter.generated.h"

class UInputComponent;
class AWeapon_Hitscan;
class AWeapon_Projectile;
class UHealthComponent;

UCLASS()
class UE4_GAMETP_API ATPCharacter : public ACharacter
{
	GENERATED_BODY()



		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FirstPersonCameraComponent;

public:
	// Sets default values for this character's properties
	ATPCharacter();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Mesh)
		class USkeletalMeshComponent* Mesh1P;

	UPROPERTY(EditDefaultsOnly)
		class USkeletalMeshComponent* Mesh3P;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;



public:

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class UAnimMontage* FireAnimation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UHealthComponent* MyHealthComponent;



protected:

	void OnFire();
	void MoveForward(float Val);
	void MoveRight(float Val);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);


	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float PlayerCurrentScore;

	UPROPERTY(Replicated)
		AWeapon_Hitscan* CurrentHSWeapon;

	UPROPERTY(Replicated)
		AWeapon_Projectile* CurrentProjectileWeapon;
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<AWeapon_Projectile> ProjectileWeapon;

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<AWeapon_Hitscan> StartWeapon;

	UFUNCTION()
		void OnChangeWeapon();

	UPROPERTY(Replicated, BlueprintReadOnly)
		bool bCanShootSecondary;

	UFUNCTION()
		void OnHealthChanged(UHealthComponent* HealtComp, float Health, float HealthChange, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UPROPERTY(Replicated, BlueprintReadOnly)
		bool bDied;
	UPROPERTY(Replicated)
		int LifeCount;

public:


	int GetLifeCount() const;
	void SetLifeCount(int RemoveLifeAmount);

	void UpdatePlayerScore(float Score);

	virtual FVector GetPawnViewLocation() const override;
};
