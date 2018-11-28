// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "WumpusWorld.h"
#include "Agent.h"
#include "Runtime/Engine/Classes/Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "Runtime/CoreUObject/Public/UObject/UObjectGlobals.h"
#include <FileHelper.h>
#include <PlatformFilemanager.h>
#include <PlatformFile.h>
#include "Timer.h"
#include "WorldGenerator.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WUMPUS_UNREAL_API UWorldGenerator : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWorldGenerator();
	~UWorldGenerator();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void ConstructWumpusWorld();
	UAudioComponent *CreateAudioComponent(const char *name);
	//The actor to spawn in the world
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> PlatformBlueprint;
	UPROPERTY(EditAnywhere)
		TSubclassOf<AActor> DoorBlueprint;
	UPROPERTY(EditAnywhere)
		TSubclassOf<AActor> PitBlueprint;
	UPROPERTY(EditAnywhere)
		TSubclassOf<AActor> WumpusBlueprint;
	UPROPERTY(EditAnywhere)
		TSubclassOf<AActor> AgentBlueprint;
	UPROPERTY(EditAnywhere)
		TSubclassOf<AActor> TreasureBlueprint;

	UPROPERTY(EditAnywhere, Category = "Sound")
		USoundCue *pitSound;
	UPROPERTY(EditAnywhere, Category = "Sound")
		USoundCue *wumpusSound;
	UPROPERTY(EditAnywhere, Category = "Sound")
		USoundCue *goldSound;
	UPROPERTY(EditAnywhere, Category = "Sound")
		USoundCue *breezeSound;
	UPROPERTY(EditAnywhere, Category = "Sound")
		USoundCue *stenchSound;
	UPROPERTY(EditAnyWhere, Category = "Sound")
		USoundCue *goalSound;
	UPROPERTY(EditAnyWhere, Category = "Sound")
		USoundCue *moveSound;

	UAudioComponent *MoveAudio;
	UAudioComponent *FXAudio;

	UPROPERTY(EditAnywhere)
		float YActorOffset;
	UPROPERTY(EditAnywhere)
		float WorldPlacementScale;
	UPROPERTY(EditAnywhere)
		float TimeBetweenUpdates = 1.0f;

private:
	WumpusWorld *world;
	AActor *treasure;
	UAgent *agent;
	float updateTimer;
	bool gameRunning;
	const int numberOfIterations = 10;
	int iterations = 0;
	int repeats = 0;

	AActor *SpawnBlueprintAt(UWorld *theWorld, TSubclassOf<AActor> blueprint, float x, float y, float heightOffset = 0, float scale = 1.0f);
	void MoveAgent(FVector newPosition, float moveDuration = 1.0f);
};
