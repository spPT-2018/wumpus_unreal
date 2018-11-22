// Fill out your copyright notice in the Description page of Project Settings.

#include "WumpusWorld.h"

WumpusWorld::WumpusWorld(std::vector<FVector2D*> *wumpusPositions, std::vector<FVector2D*> *pitPositions, FVector2D goldPosition)
{
	this->goldPosition = goldPosition;
	this->pitPositions = pitPositions;
	this->wumpusPositions = wumpusPositions;

	agentWumpus = new WumpusAgent();
}

WumpusWorld::~WumpusWorld()
{
	DeallocatePositionVector(pitPositions);
	DeallocatePositionVector(wumpusPositions);

	delete agentWumpus;
}

void WumpusWorld::DeallocatePositionVector(std::vector<FVector2D*> *vector) {
	for (auto it = vector->begin(); it != vector->end(); ++it) {
		delete *it;
	}
	delete vector;
}

bool WumpusWorld::GoldAt(FVector2D position) {
	return GoldAt(&position);
}

bool WumpusWorld::PitAt(FVector2D position) {
	return PitAt(&position);
}

bool WumpusWorld::WumpusAt(FVector2D position) {
	return WumpusAt(&position);
}

bool WumpusWorld::GoldAt(FVector2D *position) {
	return position->Equals(this->goldPosition);
}

bool WumpusWorld::PitAt(FVector2D *position) {
	for (size_t i = 0; i < pitPositions->size(); i++)
	{
		if (pitPositions->at(i)->Equals(*position))
			return true;
	}
	return false;
}

bool WumpusWorld::WumpusAt(FVector2D *position) {
	for (size_t i = 0; i < wumpusPositions->size(); i++)
	{
		if (wumpusPositions->at(i)->Equals(*position))
			return true;
	}
	return false;
}

std::vector<FVector2D*> *WumpusWorld::GetNeighbours() {
	auto possiblePositions = new std::vector<FVector2D*>();

	if (agentWumpus->CurrentPosition.X > 0) // we can go west
		possiblePositions->push_back(new FVector2D(agentWumpus->CurrentPosition.X - 1, agentWumpus->CurrentPosition.Y));
	if (agentWumpus->CurrentPosition.X < WorldWidth - 1) // we can go east
		possiblePositions->push_back(new FVector2D(agentWumpus->CurrentPosition.X + 1, agentWumpus->CurrentPosition.Y));
	if (agentWumpus->CurrentPosition.Y > 0) // we can go south
		possiblePositions->push_back(new FVector2D(agentWumpus->CurrentPosition.X, agentWumpus->CurrentPosition.Y - 1));
	if (agentWumpus->CurrentPosition.Y < WorldHeight - 1) // we can go north
		possiblePositions->push_back(new FVector2D(agentWumpus->CurrentPosition.X, agentWumpus->CurrentPosition.Y + 1));

	return possiblePositions;
}

Percepts *WumpusWorld::GeneratePercepts() {
	auto neighbours = GetNeighbours();
	auto percepts = new Percepts();

	for (auto it = neighbours->begin(); it != neighbours->end(); ++it) {
		if (PitAt(*it))
			percepts->Breeze = true;
		else if (WumpusAt(*it))
			percepts->Stench = true;
	}
	percepts->Glitter = agentWumpus->CurrentPosition.Equals(goldPosition) && !agentWumpus->FoundGold;
	return percepts;
}

void WumpusWorld::Iterate()
{
	auto agentMove = agentWumpus->WhereIWannaGo();
	agentWumpus->CurrentPosition = *agentMove;

	//Todo: Invoke OnMove event
	OnMove.invoke(*agentMove);
	delete agentMove;

	if (agentWumpus->FoundGold && agentWumpus->CurrentPosition.Equals(FVector2D(0, 0)))
	{
		OnGoalComplete.invoke();
	}

	if (WumpusAt(agentWumpus->CurrentPosition))
	{
		OnWumpusEncountered.invoke();
	}
	else if (PitAt(agentWumpus->CurrentPosition))
	{
		OnPitEncountered.invoke();
	}

	auto percepts = GeneratePercepts();
	if (percepts->Breeze) {
		OnBreezePercepted.invoke();
	}
	if (percepts->Stench) {
		OnStenchPercepted.invoke();
	}
	if (percepts->Glitter) {
		OnTreasureEncountered.invoke();
	}

	agentWumpus->PerceiveCurrentPosition(*percepts);
}
