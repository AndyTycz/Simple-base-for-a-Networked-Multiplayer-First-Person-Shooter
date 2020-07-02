// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"
#include "Net/UnrealNetwork.h"
#include "TPGameMode.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...

	TotalHealth = 100.f;
	bIsDead = false;

	SetIsReplicated(true);
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwnerRole() == ROLE_Authority) //Como es un Actor Component no puedo conseguir mi Role, tengo que conseguir el Role de mi dueño por eso uso GetOwnerRole
	{
		AActor* MyOwner = GetOwner();
		if (MyOwner)
		{
			MyOwner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::TakeAnyDamage);
		}
	}

	CurrentHealth = TotalHealth;
}

void UHealthComponent::OnRep_Health(float OldHealth)
{
	float Damage = CurrentHealth - OldHealth;

	OnHealthChanged.Broadcast(this, CurrentHealth, Damage, nullptr, nullptr, nullptr);
}


void UHealthComponent::TakeAnyDamage(AActor * DamagedActor, float Damage, const UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser)
{
	if (Damage <= 0.0f || bIsDead) return;

	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.0f, TotalHealth);

	bIsDead = CurrentHealth <= 0.0f;
	//UE_LOG(LogTemp, Warning, TEXT("Health Changed to: %s"), *FString::SanitizeFloat(CurrentHealth));

	OnHealthChanged.Broadcast(this, CurrentHealth, Damage, DamageType, InstigatedBy, DamageCauser); //Cuando recibo daño, hago un broadcast al macro para poder usarlo en BP o donde sea

	if (CurrentHealth <= 0.0f)
	{
		ATPGameMode* GM = Cast<ATPGameMode>(GetWorld()->GetAuthGameMode());
		if (GM)
		{
			GM->OnActorKilled.Broadcast(GetOwner(), InstigatedBy, Damage);
		}
	}
}


float UHealthComponent::GetHealth() const
{
	return CurrentHealth;
}


void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, CurrentHealth);
	DOREPLIFETIME(ThisClass, TotalHealth);
}