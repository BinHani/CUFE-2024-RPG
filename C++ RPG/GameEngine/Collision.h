#pragma once
#include "SDL.h"

class ColliderComponent;

class Collision {

public:
	//Axis-Aligned Bounding Box
	static bool AABB(const SDL_Rect& RecA, const SDL_Rect& RecB);
	static bool AABB(const ColliderComponent& colA, const ColliderComponent& colB);
};