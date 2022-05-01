#pragma once
#include "ECS.h"
#include <cmath>;

struct StatusComponent : public Component {

	short maxHP, currentHP;
	short maxAP, currentAP; 
	short damage; 
	short baseResistance, currentResistance;
	short combatIndex;

	double attackCoeff, defendCoeff, restCoeff, irrationalCoeff;
	double strongestWeight, tankiestWeight, weakestWeight, randomWeight;

	bool isAlive;
	bool isDefending;

	StatusComponent() {}

	StatusComponent(short _maxHP, short _maxAP, short _damage, short _resistance) : 
		maxHP(_maxHP), maxAP(_maxAP), damage(_damage), baseResistance(_resistance) {}

	void init() override {

		currentHP = maxHP;
		currentAP = maxAP;
		currentResistance = baseResistance;
		isAlive = true;
		isDefending = false;
	}

	void update() override {

		if (isDefending) {
			currentResistance = round(7 * baseResistance / 5);
		}

		else { currentResistance = baseResistance; }

	}

	StatusComponent(StatusComponent& _status) {

		maxHP = _status.maxHP;
		maxAP = _status.maxAP;
		damage = _status.damage;
		baseResistance = _status.baseResistance;
		attackCoeff = _status.attackCoeff;
		defendCoeff = _status.defendCoeff;
		restCoeff = _status.restCoeff;
		irrationalCoeff = _status.irrationalCoeff;
		strongestWeight = _status.strongestWeight;
		tankiestWeight = _status.tankiestWeight;
		weakestWeight = _status.weakestWeight;
		randomWeight = _status.randomWeight;
	}

	void SetDecisionCoeffs(double _attackCoeff, double _defendCoeff, double _restCoeff, double _irrationalCoeff) {

		attackCoeff = _attackCoeff;
		defendCoeff = _defendCoeff;
		restCoeff = _restCoeff;
		irrationalCoeff = _irrationalCoeff;
	}

	void SetTargetWeights(double _strongestWeight, double _tankiestWeight, double _weakestWeight, double _randomWeight) {

		strongestWeight = _strongestWeight;
		tankiestWeight = _tankiestWeight;
		weakestWeight = _weakestWeight;
		randomWeight = _randomWeight;
	}
};