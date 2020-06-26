// Fill out your copyright notice in the Description page of Project Settings.


#include "ExitZone.h"
#include "Components/BoxComponent.h"
#include "TPCharacter.h"

// Sets default values
AExitZone::AExitZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	OverlapComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapBox"));
	OverlapComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	OverlapComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	OverlapComponent->SetBoxExtent(FVector(200.0f));
	RootComponent = OverlapComponent;

	OverlapComponent->SetHiddenInGame(false);
	
}

void AExitZone::BeginPlay()
{
	OverlapComponent->OnComponentBeginOverlap.AddDynamic(this, &AExitZone::BoxOverlap);
}


void AExitZone::BoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	UE_LOG(LogTemp, Log, TEXT("OVERLAPPED WITH EXIT"));

	ATPCharacter* MyPawn = Cast<ATPCharacter>(OtherActor);
	if (MyPawn) // && MyPawn->>bCanExitZone)
	{
		//AUE4_GameTPGameMode* GM = Cast<AUE4_GameTPGameMode>(GetWorld()->GetAuthGameMode());
		//if (GM)
		//{
			//GM->EndGame(MyPawn);
		//}
	}
}