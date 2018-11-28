// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <unordered_map>
#include <stack>
#include <functional>
#include "WumpusStructs.h"


class WUMPUS_UNREAL_API WumpusAgent
{
public:
	bool FoundGold;
	FVector2D CurrentPosition;
	std::unordered_map<int, Percepts*> PerceptedPlaces;
	std::unordered_map<int, Knowledge*> KnowledgeOfPlaces;

	WumpusAgent();
	~WumpusAgent();

	void TellMeAboutTheWorld(int width, int height);
	void PerceiveCurrentPosition(Percepts percepts);
	FVector2D *WhereIWannaGo();

	void ClearTrace();

private:
	int WorldHeight;
	int WorldWidth;

	std::stack<FVector2D*> Trace;

	bool IKnowItsSafe(FVector2D position);
	void PossibleMoves(std::vector<FVector2D*> *possiblePositions);
};
