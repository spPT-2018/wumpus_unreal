// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <vector>
#include "WumpusAgent.h"
#include "WumpusStructs.h"
#include "WumpusEvent.h"

class WUMPUS_UNREAL_API WumpusWorld
{
public:
	const float WorldHeight = 4;
	const float WorldWidth = 4;
	WumpusAgent *agentWumpus;

	WumpusEvent<FVector2D*> OnMove;
	WumpusVoidEvent OnWumpusEncountered;
	WumpusVoidEvent OnPitEncountered;
	WumpusVoidEvent OnTreasureEncountered;
	WumpusVoidEvent OnBreezePercepted;
	WumpusVoidEvent OnStenchPercepted;
	WumpusVoidEvent OnGoalComplete;

	WumpusWorld(std::vector<FVector2D*> *wumpusPositions, std::vector<FVector2D*> *pitPositions, FVector2D goldPosition);
	~WumpusWorld();

	bool GoldAt(FVector2D position);
	bool PitAt(FVector2D position);
	bool WumpusAt(FVector2D position);
	bool GoldAt(FVector2D *position);
	bool PitAt(FVector2D *position);
	bool WumpusAt(FVector2D *position);

	static void DeallocatePositionVector(std::vector<FVector2D*> *vector);

	void Iterate();

private:
	std::vector<FVector2D*> *wumpusPositions;
	std::vector<FVector2D*> *pitPositions;
	FVector2D goldPosition;

	Percepts *GeneratePercepts();
	std::vector<FVector2D*> *GetNeighbours();
};
