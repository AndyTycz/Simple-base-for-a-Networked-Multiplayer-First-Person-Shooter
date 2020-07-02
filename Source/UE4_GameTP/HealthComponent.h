// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"


//Evento custom para cuando cambia la vida, se puede llamar desde Blueprints o codigo y tiene parametrosa que nos importan
DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnHealthChangedSignature, UHealthComponent*, HealtComp, float, Health, float, HealthChange, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UE4_GAMETP_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly)
		float TotalHealth;

	UPROPERTY(ReplicatedUsing = OnRep_Health, BlueprintReadOnly)
		float CurrentHealth;

	UFUNCTION()
		void OnRep_Health(float OldHealth);

	UFUNCTION()
		void TakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

		bool bIsDead;
public:

	UPROPERTY(BlueprintAssignable)
		FOnHealthChangedSignature OnHealthChanged;

	float GetHealth() const;
};
