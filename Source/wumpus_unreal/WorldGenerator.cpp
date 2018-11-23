// Fill out your copyright notice in the Description page of Project Settings.

#include "WorldGenerator.h"

// Sets default values for this component's properties
UWorldGenerator::UWorldGenerator()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	FXAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("FXAudio"));
	FXAudio->bAutoActivate = false;
	MoveAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("MoveAudio"));
	MoveAudio->bAutoActivate = false;

	gameRunning = true;
}

UWorldGenerator::~UWorldGenerator() {
	delete world;
}


// Called when the game starts
void UWorldGenerator::BeginPlay()
{
	Super::BeginPlay();

	MoveAudio->SetSound(moveSound);

	auto *wumpi = new std::vector<FVector2D*>();
	wumpi->push_back(new FVector2D(0, 2));
	auto *pits = new std::vector<FVector2D*>();
	pits->push_back(new FVector2D(2, 0));
	pits->push_back(new FVector2D(2, 2));
	pits->push_back(new FVector2D(3, 3));

	// ...
	world = new WumpusWorld(wumpi, pits, FVector2D(1, 2));
	world->OnMove.add_handler([&](FVector2D *newAgentPosition) {
		UE_LOG(LogTemp, Warning, TEXT("(%f,%f)"), newAgentPosition->X, newAgentPosition->Y);
		auto worldPosition = FVector(newAgentPosition->X * WorldPlacementScale, newAgentPosition->Y * WorldPlacementScale, YActorOffset);
		this->MoveAgent(worldPosition);
		MoveAudio->Play();
	});
	world->OnBreezePercepted.add_handler([&]() {
		FXAudio->SetSound(breezeSound);
		FXAudio->Play();
	});
	world->OnStenchPercepted.add_handler([&]() {
		FXAudio->SetSound(stenchSound);
		FXAudio->Play();
	});
	world->OnTreasureEncountered.add_handler([&]() {
		FXAudio->SetSound(goldSound);
		FXAudio->Play();
		treasure->Destroy();
	});
	world->OnPitEncountered.add_handler([&]() {
		FXAudio->SetSound(pitSound);
		FXAudio->Play();
	});
	world->OnWumpusEncountered.add_handler([&]() {
		FXAudio->SetSound(wumpusSound);
		FXAudio->Play();
	});
	world->OnGoalComplete.add_handler([&]() {
		FXAudio->SetSound(goalSound);
		FXAudio->Play();
		gameRunning = false;
	});

	// ...
	ConstructWumpusWorld();
}


// Called every frame
void UWorldGenerator::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!gameRunning)
		return;

	// ...
	if (this->updateTimer > TimeBetweenUpdates) {
		auto currentLocation = agent->GetOwner()->GetActorLocation();
		auto newPos = FVector(currentLocation.X + WorldPlacementScale, currentLocation.Y, currentLocation.Z);
		updateTimer = 0;
		world->Iterate();
	}
	else
		updateTimer += DeltaTime;
}

AActor *UWorldGenerator::SpawnBlueprintAt(UWorld *theWorld, TSubclassOf<AActor> blueprint, float x, float y, float heightOffset, float scale) {
	FTransform ComponentTransform(FQuat::Identity, FVector(x * WorldPlacementScale, y * WorldPlacementScale, heightOffset), FVector(scale, scale, scale));
	return theWorld->SpawnActor(blueprint, &ComponentTransform);
}

void UWorldGenerator::ConstructWumpusWorld() {
	UWorld *unrealWorld = GetWorld();
	if (unrealWorld == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("Failed to find world in WorldGenerator->ConstructWumpusWorld"));
		return;
	}

	for (int i = 0; i < world->WorldHeight; i++)
	{
		for (int j = 0; j < world->WorldWidth; j++)
		{
			if (i == 0 && j == 0)
				SpawnBlueprintAt(unrealWorld, DoorBlueprint, i, j);
			else
				SpawnBlueprintAt(unrealWorld, PlatformBlueprint, i, j);

			FVector2D currentPosition = FVector2D(i, j);

			if (world->GoldAt(currentPosition))
				treasure = SpawnBlueprintAt(unrealWorld, TreasureBlueprint, i, j, YActorOffset, 0.7f);
			else if (world->PitAt(currentPosition))
				SpawnBlueprintAt(unrealWorld, PitBlueprint, i, j, YActorOffset, 0.7f);
			else if (world->WumpusAt(currentPosition))
				SpawnBlueprintAt(unrealWorld, WumpusBlueprint, i, j, YActorOffset, 0.7f);
		}
	}

	auto agentActor = SpawnBlueprintAt(unrealWorld, AgentBlueprint, 0, 0, YActorOffset, 0.7f);
	agent = (UAgent *)agentActor->GetComponentByClass(UAgent::StaticClass());
	if (agent == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("Failed to find a reference to the agent component"));
	}
}

void UWorldGenerator::MoveAgent(FVector newPosition, float moveDuration) {
	if(agent != nullptr)
		agent->SetLerpDestination(newPosition, moveDuration);
}

