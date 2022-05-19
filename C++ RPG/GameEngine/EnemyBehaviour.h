#pragma once
#include "ECS\ECS.h"
#include "ECS\Components.h"
#include "Game.h"
#include <random>

extern Manager manager;

class EnemyBehaviour {

public:

	EnemyBehaviour() {}

	EnemyBehaviour(StatusComponent* _status, double _attackCoeff, double _defendCoeff, double _restCoeff, double _irrationalCoeff) :
		status(_status), attackCoeff(_attackCoeff), defendCoeff(_defendCoeff), restCoeff(_restCoeff), irrationalCoeff(_irrationalCoeff) {}

	enum class decision { ATK, DEF, REST };
	enum class target { weakest, strongest, tankiest };
	enum class attackType { weak, regular, strong };

	void setStatus(StatusComponent* _status) { 
		
		status = _status;
		attackCoeff = _status->attackCoeff;
		defendCoeff = _status->defendCoeff;
		restCoeff = _status->restCoeff;
		irrationalCoeff = _status->irrationalCoeff;
		strongestWeight = _status->strongestWeight;
		tankiestWeight = _status->tankiestWeight;
		weakestWeight = _status->weakestWeight;
		randomWeight = _status->randomWeight;
	}

	// Utility functions to get the strongest (most attack power), tankiest (most HP), and weakest (least HP) characters to aid in the AI decision making
	StatusComponent* GetStrongest() {

		StatusComponent strongest = manager.getGroup(Game::groupPlayerCharacters)[0]->getComponent<StatusComponent>();

		for (auto& p : manager.getGroup(Game::groupPlayerCharacters)) {

			if (p->getComponent<StatusComponent>().damage >= strongest.damage) { strongest = p->getComponent<StatusComponent>(); }
		}

		return &strongest;

	}

	StatusComponent* GetTankiest() {

		StatusComponent tankiest = manager.getGroup(Game::groupPlayerCharacters)[0]->getComponent<StatusComponent>();

		for (auto& p : manager.getGroup(Game::groupPlayerCharacters)) {

			if (p->getComponent<StatusComponent>().currentHP >= tankiest.currentHP) { tankiest = p->getComponent<StatusComponent>(); }
		}

		return &tankiest;

	}

	StatusComponent* GetWeakest() {

		StatusComponent weakest = manager.getGroup(Game::groupPlayerCharacters)[0]->getComponent<StatusComponent>();

		for (auto& p : manager.getGroup(Game::groupPlayerCharacters)) {

			if (p->getComponent<StatusComponent>().currentHP <= weakest.currentHP) { weakest = p->getComponent<StatusComponent>(); }
		}

		return &weakest;
	}

	/* The "AI" used for the decision making process for the enemy combatants. The basic idea here is that we have weights for atack, defend, rest, and irrational behavior. 
	These weights are influenced by several factors, e.g., the enemy's inclination (each enemy type has coefficients which indicate whether this enemy prefers to attack more 
	or defend more and so on) and the enemy's current stats. The decision maker calculates the values of all the weights, and then at the end does a comparison, 
	wherein the weight with the largest value specifies the decision. */
	decision DecisionMaker() {

		SetNormalizedStats();

		if (normalizedAP <= 0.15) { return decision::REST; }

		if (normalizedHP >= 0.5) {

			attackWeight = attackCoeff * normalizedHP;
			defendWeight = defendCoeff * (1 - normalizedHP);
			restWeight = restCoeff * (1 - normalizedAP);
			irrationalWeight = irrationalCoeff * normalizedHP;

		}
		/* The attack weights change based on whether the enemy is near death (below 50% HP) or not. The idea was to have more changes, however right now all this does is lead to 
		more irrational behavior, faking a sense of 'panic'. */
		if (normalizedHP < 0.5) {

			attackWeight = attackCoeff * normalizedHP;
			defendWeight = defendCoeff * (1 - normalizedHP);
			restWeight = restCoeff * (1 - normalizedAP);
			irrationalWeight = irrationalCoeff * (1 - normalizedHP);
		}

		// Current weights, for debugging purposes 
		std::cout << "===========AI INFO===========" << std::endl;
		std::cout << "AttackWeight: " << attackWeight << std::endl;
		std::cout << "DefendWeight: " << defendWeight << std::endl;
		std::cout << "RestWeight: " << restWeight << std::endl;
		std::cout << "IrrationalWeight: " << irrationalWeight << std::endl << std::endl;

		decision _decision = GetDecision();

		return _decision;
	}

	/* The "AI" used for the targeting process for the enemy combatants on an attack. The idea here is that we have weights for each possible target (random, strongest, tankiest, etc.). 
	These weights are influenced by 2 factors; the enemy's inclination (each enemy type has coefficients which indicate how much this enemy prefers to attack the strongest character and so on)
	and the player characters' current stats. The target chooser calculates the values of all the weights, and then at the end does a comparison, wherein the weight with the largest value 
	specifies the target. */
	target TargetChooser() {

		StatusComponent* weakest = GetWeakest();
		StatusComponent* strongest = GetStrongest();
		StatusComponent* tankiest = GetTankiest();

		ResetTargetWeights();

		if (static_cast<double>(strongest->currentHP) / strongest->maxHP <= 0.65) {
			
			strongestWeight += 0.5;
			randomWeight += 0.1;
		}

		if (weakest->currentHP <= 1.2 * status->damage) {
			
			weakestWeight += 0.8;
			randomWeight += 0.1;
		}

		if (tankiest->damage >= status->damage) {
			
			tankiestWeight += 0.7;
			randomWeight += 0.1;
		}

		else randomWeight += 1;

		return GetTarget();
	}

private:
	StatusComponent* status;

	double normalizedHP, normalizedAP;
	double attackWeight, defendWeight, restWeight, irrationalWeight;
	double strongestWeight, tankiestWeight, weakestWeight, randomWeight;
	double attackCoeff, defendCoeff, restCoeff, irrationalCoeff;

	/* The utility functions that are responsible for the comparison step in the decision making and targeting AI. The way both work is by analyzing the corresponding weights and then
	returning the decision based on the distribution of the weights. */
	decision GetDecision() {

		double interval[] = { 1, 2, 3, 4, 5 };
		double weights[] = { attackWeight, defendWeight, restWeight, irrationalWeight };
		std::piecewise_constant_distribution<> mainDist(std::begin(interval), std::end(interval), std::begin(weights));
		std::uniform_real_distribution<> subDist(1, 4);

		std::random_device rd;
		std::seed_seq sd{ rd(), rd(), rd(), rd(), rd(), rd(), rd(), rd() };

		std::mt19937 RNG(sd);

		double r = mainDist(RNG);

		if (r >= 1 && r < 2) return decision::ATK;

		if (r >= 2 && r < 3) return decision::DEF;

		if (r >= 3 && r < 4) return decision::REST;

		if (r >= 4 && r < 5) { // irrational behaviour

			r = subDist(RNG);

			if (r >= 1 && r < 2) return decision::ATK;

			if (r >= 2 && r < 3) return decision::DEF;

			if (r >= 3 && r < 4) return decision::REST;

		}
	}

	target GetTarget() {

		double interval[] = { 1, 2, 3, 4, 5 };
		double weights[] = { weakestWeight, strongestWeight, tankiestWeight, randomWeight };
		std::piecewise_constant_distribution<> mainDist(std::begin(interval), std::end(interval), std::begin(weights));
		std::uniform_real_distribution<> subDist(1, 4);

		std::random_device rd;
		std::seed_seq sd{ rd(), rd(), rd(), rd(), rd(), rd(), rd(), rd() };

		std::mt19937 RNG(sd);

		double r = mainDist(RNG);

		if (r >= 1 && r < 2) return target::weakest;

		if (r >= 2 && r < 3) return target::strongest;

		if (r >= 3 && r < 4) return target::tankiest;

		if (r >= 4 && r < 5) { // irrational behaviour

			r = subDist(RNG);

			if (r >= 1 && r < 2) return target::weakest;

			if (r >= 2 && r < 3) return target::strongest;

			if (r >= 3 && r < 4) return target::tankiest;

		}
	}

	void ResetTargetWeights() {

		strongestWeight = status->strongestWeight;
		tankiestWeight = status->tankiestWeight;
		weakestWeight = status->weakestWeight;
		randomWeight = status->randomWeight;
	}

	void SetNormalizedStats() {

		normalizedHP = static_cast<double>(status->currentHP) / status->maxHP;
		normalizedAP = static_cast<double>(status->currentAP) / status->maxAP;
	}

};