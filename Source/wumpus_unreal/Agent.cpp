// Fill out your copyright notice in the Description page of Project Settings.

#include "Agent.h"


// Sets default values for this component's properties
UAgent::UAgent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAgent::BeginPlay()
{
	Super::BeginPlay();

	owner = GetOwner();
	// ...
	SetLerpDestination(owner->GetActorLocation(), 0);
}


// Called every frame
void UAgent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	this->timeSpent+= DeltaTime;

	if (lerping && timeSpent > duration) {
		owner->SetActorLocation(*lerpDestination);
		lerping = false;
	}
	else
	{
		owner->SetActorLocation(FMath::Lerp(owner->GetActorLocation(), *lerpDestination, timeSpent / duration));
	}
}

void UAgent::SetLerpDestination(FVector destination, float duration) {
	if (this->lerpDestination)
		delete this->lerpDestination;

	//UE_LOG(LogTemp, Error, TEXT("Moving agent to (%f,%f,%f)"), destination.X, destination.Y, destination.Z)

	this->duration = duration;
	this->lerpDestination = new FVector(destination);
	this->timeSpent = 0;
	lerping = true;
}

