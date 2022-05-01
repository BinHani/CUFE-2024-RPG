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
	}

	StatusComponent* GetStrongest(StatusComponent _playerStatus[]) {

		StatusComponent strongest = _playerStatus[0];

		for (size_t i = 1; i < 4; i++) {

			if (_playerStatus[i].damage >= strongest.damage) { strongest = _playerStatus[i]; }
		}

		return &strongest;

	}

	StatusComponent* GetTankiest(StatusComponent _playerStatus[]) {

		StatusComponent tankiest = _playerStatus[0];

		for (size_t i = 1; i < 4; i++) {

			if (_playerStatus[i].currentHP >= tankiest.currentHP) { tankiest = _playerStatus[i]; }
		}

		return &tankiest;

	}

	StatusComponent* GetWeakest(StatusComponent _playerStatus[]) {

		StatusComponent weakest = _playerStatus[0];

		for (size_t i = 1; i < 4; i++) {

			if (_playerStatus[i].currentHP <= weakest.currentHP) { weakest = _playerStatus[i]; }
		}

		return &weakest;
	}
	
	decision DecisionMaker() {

		GetDecisionCoeffs();

		if (static_cast<double>(status->currentAP) / status->maxAP <= 0.15) { return decision::REST; }

		if (static_cast<double>(status->currentHP) / status->maxHP >= 0.5) {

			attackWeight += attackCoeff * normalizedHP;
			defendWeight += defendCoeff * (1 - normalizedHP);
			restWeight = restCoeff * (1 - normalizedAP);
			irrationalWeight += irrationalCoeff * normalizedHP;

		}

		if (normalizedHP < 0.5) {

			attackWeight += attackCoeff * normalizedHP;
			defendWeight += defendCoeff * (1 - normalizedHP);
			restWeight = restCoeff * (1 - normalizedAP);
			irrationalWeight += irrationalCoeff * (1 - normalizedHP);
		}

		return GetDecision();

	}

	target TargetChooser(StatusComponent _playerStatus[]) {

		GetTargetWeights();

		StatusComponent* weakest = GetWeakest(_playerStatus);
		StatusComponent* strongest = GetStrongest(_playerStatus);
		StatusComponent* tankiest = GetTankiest(_playerStatus);

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

	void GetDecisionCoeffs() { 
		
		attackCoeff = status->attackCoeff;
		defendCoeff = status->defendCoeff;
		restCoeff = status->restCoeff;
		irrationalCoeff = status->irrationalCoeff;
	}

	void GetTargetWeights() {

		strongestWeight = status->strongestWeight;
		tankiestWeight = status->tankiestWeight;
		weakestWeight = status->weakestWeight;
		randomWeight = status->randomWeight;
	}

	void ResetDecisionWeights() { attackWeight = defendWeight = restWeight = irrationalWeight = 0; }

	void ResetTargetWeights() { strongestWeight = tankiestWeight = weakestWeight = randomWeight = 0; }
};