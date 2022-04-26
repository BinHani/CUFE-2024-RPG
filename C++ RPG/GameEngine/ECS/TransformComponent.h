#pragma once
#include "ECS.h"
#include "Components.h"
#include "..\Vector2D.h"
#include "..\WorldState.h"

struct TransformComponent : public Component {

	Vector2D position;
	Vector2D velocity;

	int height = 32;
	int width = 32;
	float scale = 1;
	int speed = 3;

	TransformComponent() { position.Zero(); }

	TransformComponent(float sc) { 

		position.x = 640;
		position.y = 360;
		scale = sc; 
	}

	TransformComponent(float x, float y) {

		position.x = x;
		position.y = y;
	}

	TransformComponent(float x, float y, int h, int w, float sc) { 
		position.x = x;
		position.y = y;
		height = h;
		width = w;
		scale = sc;
	}

	void init() override { velocity.Zero(); }

	void update() override { 

		switch (_worldState) {

		case stateRealTime:
			position.x += velocity.x * speed;
			position.y += velocity.y * speed;
			break;

		case stateTurnBased:
			speed = 0;
		}
	}

};