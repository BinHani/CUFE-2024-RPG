#pragma once
#include "Components.h"
#include <iostream>

class StatusComponent : public Component {

public:
	TransformComponent* transform;
	short HP;
	Vector2D oldPos;
	uint16_t AP, DAMAGE, RESISTANCE, stepCounter = 0;

	StatusComponent() {}

	StatusComponent(short hp, short ap, short damage, short resistance) : HP(hp), AP(ap), DAMAGE(damage), RESISTANCE(resistance) {}

	void init() override {



	}

	void update() override {



	}

};