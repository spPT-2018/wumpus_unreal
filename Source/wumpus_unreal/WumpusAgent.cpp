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
	for (auto& k : KnowledgeOfPlaces) {
		delete k.second;
	}
	for (auto& p : PerceptedPlaces) {
		delete p.second;
	}
}

bool has_percepted(std::unordered_map<int, Percepts*> map, FVector2D key) {
	auto pos_hash = hash_position(key);

	return map.find(pos_hash) != map.end();
}
bool has_knowledge(std::unordered_map<int, Knowledge*> map, FVector2D key) {
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

	if (!has_percepted(PerceptedPlaces, CurrentPosition)) {
		PerceptedPlaces[pos_hash] = new Percepts(percepts);
	}
	if (!has_knowledge(KnowledgeOfPlaces, CurrentPosition)) {
		auto k = new Knowledge();
		k->MightHavePit = false;
		k->MightHaveWumpus = false;
		KnowledgeOfPlaces[pos_hash] = k;
	}

	if (percepts.Glitter)
	{
		FoundGold = true;
	}

	std::vector<FVector2D*> moves;
	PossibleMoves(&moves);

	for (auto& position : moves) {
		auto hasKnowledge = has_knowledge(KnowledgeOfPlaces, *position);
		auto position_hash = hash_position(*position);

		if (hasKnowledge)
		{
			auto knowledge = KnowledgeOfPlaces[position_hash];
			if (!percepts.Stench && knowledge->MightHaveWumpus)
				knowledge->MightHaveWumpus = false;

			if (!percepts.Breeze && knowledge->MightHavePit)
				knowledge->MightHavePit = false;
		}
		else
		{
			auto knowledge = new Knowledge();
			knowledge->MightHavePit = percepts.Breeze;
			knowledge->MightHaveWumpus = percepts.Stench;
			KnowledgeOfPlaces[position_hash] = knowledge;
		}
	}

	DeallocatePositionVector(moves);
}
FVector2D *WumpusAgent::WhereIWannaGo() {
	if (FoundGold)
	{
		UE_LOG(LogTemp, Warning, TEXT("I have found the gold"))
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

		/*UE_LOG(LogTemp, Warning, TEXT("Places to go from (%f,%f): "), CurrentPosition.X, CurrentPosition.Y);
		for (auto& p : placesToGo) {
			UE_LOG(LogTemp, Warning, TEXT("(%f,%f)"), p->X, p->Y);
		}*/
		UE_LOG(LogTemp, Warning, TEXT("Current percepts:"));
		for (auto& p : PerceptedPlaces) {
			UE_LOG(LogTemp, Warning, TEXT("Percepts in (%d): Breeze %d, Glitter %d, Stench %d"), p.first, p.second->Breeze, p.second->Glitter, p.second->Stench);
		}
		UE_LOG(LogTemp, Warning, TEXT("Current knowledge"));
		for (auto& p : KnowledgeOfPlaces) {
			UE_LOG(LogTemp, Warning, TEXT("Knowledge in (%d): Pit %d, Wumpus %d"), p.first, p.second->MightHavePit, p.second->MightHaveWumpus);
		}

		if (safeNewPlacesToGo.size() > 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("There is a safe new place to go, going there."))
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
			UE_LOG(LogTemp, Warning, TEXT("There is a safe place to go, going there"))
			auto move = safePlacesToGo.at(0);
			auto move_copy = new FVector2D(*move);
			auto move_copy2 = new FVector2D(*move);

			DeallocatePositionVector(placesToGo);

			Trace.push(move_copy2);
			return move_copy;
		}

		UE_LOG(LogTemp, Warning, TEXT("Executing an unsafe move"))
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
		!KnowledgeOfPlaces[p]->MightHaveWumpus &&
		!KnowledgeOfPlaces[p]->MightHavePit;
}
void WumpusAgent::PossibleMoves(std::vector<FVector2D*> *possiblePositions) {
	if (CurrentPosition.X > 0) // we can go west
	{
		possiblePositions->push_back(new FVector2D(CurrentPosition.X - 1, CurrentPosition.Y));
	}
	if (CurrentPosition.X < WorldWidth - 1) // we can go east
	{
		possiblePositions->push_back(new FVector2D(CurrentPosition.X + 1, CurrentPosition.Y));
	}
	if (CurrentPosition.Y > 0) // we can go south
	{
		possiblePositions->push_back(new FVector2D(CurrentPosition.X, CurrentPosition.Y - 1));
	}
	if (CurrentPosition.Y < WorldHeight - 1) // we can go north
	{
		possiblePositions->push_back(new FVector2D(CurrentPosition.X, CurrentPosition.Y + 1));
	}
}