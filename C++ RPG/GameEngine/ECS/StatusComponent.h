#pragma once
#include "ECS.h"
#include <cmath>;

struct StatusComponent : public Component {

	short maxHP, currentHP;
	short maxAP, currentAP; 
	short damage; 
	short baseResistance, currentResistance;
	short battleIndex;

	double attackCoeff, defendCoeff, restCoeff, irrationalCoeff;

	bool isAlive;
	bool isDefending;

	StatusComponent() {}

	StatusComponent(short _maxHP, short _maxAP, short _damage, short _resistance, short _battleIndex) : 
		maxHP(_maxHP), maxAP(_maxAP), damage(_damage), baseResistance(_resistance), battleIndex(_battleIndex) {}

	void init() override {

		currentHP = maxHP;
		currentAP = maxAP;
		currentResistance = baseResistance;
		isAlive = true;
		isDefending = false;
	}

	void update() override {

		if (currentHP <= 0) { currentHP = 0; isAlive = false; }

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
	}

	void setCoeffs(double _attackCoeff, double _defendCoeff, double _restCoeff, double _irrationalCoeff) {

		attackCoeff = _attackCoeff;
		defendCoeff = _defendCoeff;
		restCoeff = _restCoeff;
		irrationalCoeff = _irrationalCoeff;
	}

};