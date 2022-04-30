#pragma once
#include "ECS.h"
#include <cmath>;

struct StatusComponent : public Component {

	short maxHP, currentHP;
	short maxAP, currentAP; 
	short damage; 
	short baseResistance, currentResistance;
	short battleIndex;

	bool isAlive;
	bool isDefending;

	StatusComponent() {}

	StatusComponent(short _maxHP, short _maxAP, short _damage, short _resistance) : 
		maxHP(_maxHP), maxAP(_maxAP), damage(_damage), baseResistance(_resistance) {
	}

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
};