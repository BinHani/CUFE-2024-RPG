#pragma once
#include "ECS.h"
#include "Components.h"
#include <iostream>

extern bool stateInitialized;

class FightNight : public Component {

public:
	Vector2D oldPosition, *currentPosition;
	uint16_t steps;

	void init() override {

		steps = 0;
		currentPosition = &entity->getComponent<TransformComponent>().position;
		oldPosition.x = currentPosition->x;
		oldPosition.y = currentPosition->y;
	}

	void update() override {

		switch (_worldState) {

		case stateRealTime:

			if (oldPosition.x != currentPosition->x || oldPosition.y != currentPosition->y) { steps++; }
			oldPosition.x = currentPosition->x;
			oldPosition.y = currentPosition->y;
			//std::cout << steps << std::endl;
			if (steps >= 500) { stateInitialized = false;  _worldState = stateTurnBased; }

			break;
		}
	}
};