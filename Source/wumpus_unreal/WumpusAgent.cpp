// Fill out your copyright notice in the Description page of Project Settings.

#include "WumpusAgent.h"

int hash_position(FVector2D position) {
	auto x = (int)position.X * 397;
	auto y = (int)position.Y;

	return x ^ y;
}

int hash_position(FVector2D *position) {
	return hash_position(*position);
}

void DeallocatePositionVector(std::vector<FVector2D*> vector) {
	for (auto it = vector.begin(); it != vector.end(); ++it) {
		delete *it;
	}
}

WumpusAgent::WumpusAgent()
{
	CurrentPosition = FVector2D(0, 0);
	FoundGold = false;
}

WumpusAgent::~WumpusAgent()
{
	
}

bool has_percepted(std::unordered_map<int, Percepts> map, FVector2D key) {
	auto pos_hash = hash_position(key);

	return map.find(pos_hash) != map.end();
}
bool has_knowledge(std::unordered_map<int, Knowledge> map, FVector2D key) {
	auto pos_hash = hash_position(key);

	return map.find(pos_hash) != map.end();
}

void filter(std::vector<FVector2D*> in_vector, std::vector<FVector2D*> *out_vector, std::function<bool(FVector2D*)> pred) {
	for (auto it = in_vector.begin(); it != in_vector.end(); ++it) {
		if (pred(*it))
			out_vector->push_back(*it);
	}
}

void WumpusAgent::TellMeAboutTheWorld(int width, int height) {
	this->WorldHeight = height;
	this->WorldWidth = width;
}
void WumpusAgent::PerceiveCurrentPosition(Percepts percepts) {
	auto pos_hash = hash_position(CurrentPosition);

	PerceptedPlaces[pos_hash] = percepts;
	KnowledgeOfPlaces[pos_hash] = Knowledge();

	if (percepts.Glitter)
	{
		FoundGold = true;
	}

	std::vector<FVector2D*> moves;
	PossibleMoves(&moves);
	std::vector<FVector2D*> newPlacesToGo;
	filter(moves, &newPlacesToGo, [this](FVector2D *p) -> bool { return has_percepted(PerceptedPlaces, *p); });

	for (auto position = newPlacesToGo.begin(); position != newPlacesToGo.end(); ++position) {
		auto hasKnowledge = has_knowledge(KnowledgeOfPlaces, **position);
		auto position_hash = hash_position(*position);

		if (hasKnowledge)
		{
			auto knowledge = KnowledgeOfPlaces[position_hash];
			if (!percepts.Stench && knowledge.MightHaveWumpus)
				knowledge.MightHaveWumpus = false;

			if (!percepts.Breeze && knowledge.MightHavePit)
				knowledge.MightHavePit = false;
		}
		else
		{
			auto knowledge = Knowledge();
			knowledge.MightHavePit = percepts.Breeze;
			knowledge.MightHaveWumpus = percepts.Stench;
			KnowledgeOfPlaces[position_hash] = knowledge;
		}
	}
}
FVector2D *WumpusAgent::WhereIWannaGo() {
	if (FoundGold)
	{
		if (Trace.size() == 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("Trace is empty, standing still"))
			return new FVector2D(0, 0);
		}
		auto move = Trace.top();
		Trace.pop();

		return move;
	}
	else
	{
		// Find gold 'n kill wumpi
		std::vector<FVector2D*> placesToGo;
		PossibleMoves(&placesToGo);
		std::vector<FVector2D*> placesIveBeen;
		filter(placesToGo, &placesIveBeen, [this](FVector2D *p) -> bool { return has_percepted(PerceptedPlaces, *p); });
		std::vector<FVector2D*> newPlacesToGo;
		filter(placesToGo, &newPlacesToGo, [this](FVector2D *p) -> bool { return !has_percepted(PerceptedPlaces, *p); });
		std::vector<FVector2D*> safeNewPlacesToGo;
		filter(newPlacesToGo, &safeNewPlacesToGo, [this](FVector2D *p) -> bool { return IKnowItsSafe(*p); });

		if (safeNewPlacesToGo.size() > 0)
		{
			auto move = safeNewPlacesToGo.at(0);
			auto move_copy = new FVector2D(*move);
			auto move_copy2 = new FVector2D(*move);

			DeallocatePositionVector(placesToGo);

			Trace.push(move_copy2);
			return move_copy;
		}

		std::vector<FVector2D*> safePlacesToGo;
		filter(placesToGo, &safePlacesToGo, [this](FVector2D *p) -> bool {return IKnowItsSafe(*p); });
		
		if (safePlacesToGo.size() > 0)
		{
			auto move = safePlacesToGo.at(0);
			auto move_copy = new FVector2D(*move);
			auto move_copy2 = new FVector2D(*move);

			DeallocatePositionVector(placesToGo);

			Trace.push(move_copy2);
			return move_copy;
		}

		auto dangerousMove = newPlacesToGo.size() > 0 ? newPlacesToGo.at(0) : placesToGo.at(0);
		auto move_copy = new FVector2D(*dangerousMove);
		auto move_copy2 = new FVector2D(*dangerousMove);

		DeallocatePositionVector(placesToGo);

		Trace.push(move_copy2);
		return move_copy;
	}
}

bool WumpusAgent::IKnowItsSafe(FVector2D position) {
	auto p = hash_position(position);

	return has_knowledge(KnowledgeOfPlaces, position) &&
		!KnowledgeOfPlaces[p].MightHaveWumpus &&
		!KnowledgeOfPlaces[p].MightHavePit;
}
void WumpusAgent::PossibleMoves(std::vector<FVector2D*> *possiblePositions) {
	if (CurrentPosition.X > 0) // we can go west
		possiblePositions->push_back(new FVector2D(CurrentPosition.X - 1, CurrentPosition.Y));
	if (CurrentPosition.X < WorldWidth - 1) // we can go east
		possiblePositions->push_back(new FVector2D(CurrentPosition.X + 1, CurrentPosition.Y));
	if (CurrentPosition.Y > 0) // we can go south
		possiblePositions->push_back(new FVector2D(CurrentPosition.X, CurrentPosition.Y - 1));
	if (CurrentPosition.Y < WorldHeight - 1) // we can go north
		possiblePositions->push_back(new FVector2D(CurrentPosition.X, CurrentPosition.Y + 1));
}