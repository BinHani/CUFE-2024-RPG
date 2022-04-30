#pragma once
#include "..\Game.h"
#include "ECS.h"
#include "..\WorldState.h"
#include "Components.h"
#include <queue>


class KeyboardController : public Component {

public:
	
	TransformComponent* transform;
	SpriteComponent* sprite;
	//std::queue<int> inputDelayer;
	const uint8_t* keystates;

	void init() override {
		
		transform = &entity->getComponent<TransformComponent>();
		sprite = &entity->getComponent<SpriteComponent>();
		keystates = SDL_GetKeyboardState(NULL);
	}

	void update() override {

		if (_worldState == stateRealTime) {

			if (Game::event.type == SDL_KEYDOWN) {

				//inputDelayer.push(Game::event.key.keysym.sym);

				switch (Game::event.key.keysym.sym) {

				case SDLK_w:

					if (keystates[SDL_SCANCODE_A]) {
						//inputDelayer.push(SDL_SCANCODE_A);
						transform->velocity.x = -1;
						transform->velocity.y = -1;
						sprite->Play("WalkUp");
					}

					else if (keystates[SDL_SCANCODE_S]) {
						//inputDelayer.push(SDL_SCANCODE_S);
						transform->velocity.y = 0;
						sprite->Play("Idle");
					}

					else if (keystates[SDL_SCANCODE_D]) {
						//inputDelayer.push(SDL_SCANCODE_D);
						transform->velocity.x = 1;
						transform->velocity.y = -1;
						sprite->Play("WalkUp");
					}

					else {
						//inputDelayer.push(SDL_SCANCODE_UNKNOWN);
						transform->velocity.y = -1;
						sprite->Play("WalkUp");
					}

					break;

				case SDLK_a:

					if (keystates[SDL_SCANCODE_W]) {
						//inputDelayer.push(SDL_SCANCODE_W);
						transform->velocity.x = -1;
						transform->velocity.y = -1;
						sprite->Play("WalkLeft");
					}

					else if (keystates[SDL_SCANCODE_S]) {
						//inputDelayer.push(SDL_SCANCODE_S);
						transform->velocity.x = -1;
						transform->velocity.y = 1;
						sprite->Play("WalkLeft");
					}

					else if (keystates[SDL_SCANCODE_D]) {
						//inputDelayer.push(SDL_SCANCODE_D);
						transform->velocity.x = 0;
						sprite->Play("Idle");
					}

					else {
						//inputDelayer.push(SDL_SCANCODE_UNKNOWN);
						transform->velocity.x = -1;
						sprite->Play("WalkLeft");
					}

					break;

				case SDLK_s:

					if (keystates[SDL_SCANCODE_W]) {
						//inputDelayer.push(SDL_SCANCODE_W);
						transform->velocity.y = 0;
						sprite->Play("Idle");
					}

					else if (keystates[SDL_SCANCODE_A]) {
						//inputDelayer.push(SDL_SCANCODE_A);
						transform->velocity.x = -1;
						transform->velocity.y = 1;
						sprite->Play("WalkDown");
					}

					else if (keystates[SDL_SCANCODE_D]) {
						//inputDelayer.push(SDL_SCANCODE_D);
						transform->velocity.x = 1;
						transform->velocity.y = 1;
						sprite->Play("WalkDown");
					}

					else {
						//inputDelayer.push(SDL_SCANCODE_UNKNOWN);
						transform->velocity.y = 1;
						sprite->Play("WalkDown");
					}

					break;

				case SDLK_d:

					if (keystates[SDL_SCANCODE_W]) {
						//inputDelayer.push(SDL_SCANCODE_W);
						transform->velocity.x = 1;
						transform->velocity.y = -1;
						sprite->Play("WalkLeft");
						sprite->spriteFlip = SDL_FLIP_HORIZONTAL;
					}

					else if (keystates[SDL_SCANCODE_A]) {
						//inputDelayer.push(SDL_SCANCODE_A);
						transform->velocity.x = 0;
						sprite->Play("Idle");
					}

					else if (keystates[SDL_SCANCODE_S]) {
						//inputDelayer.push(SDL_SCANCODE_S);
						transform->velocity.x = 1;
						transform->velocity.y = 1;
						sprite->Play("WalkLeft");
						sprite->spriteFlip = SDL_FLIP_HORIZONTAL;
					}

					else {
						//inputDelayer.push(SDL_SCANCODE_UNKNOWN);
						transform->velocity.x = 1;
						sprite->Play("WalkLeft");
						sprite->spriteFlip = SDL_FLIP_HORIZONTAL;
					}
					break;
				}
			}

			if (Game::event.type == SDL_KEYUP) {

				//inputDelayer.push(Game::event.key.keysym.sym);

				switch (Game::event.key.keysym.sym) {

				case SDLK_w:

					if (keystates[SDL_SCANCODE_A]) {
						//inputDelayer.push(SDL_SCANCODE_A);
						transform->velocity.x = -1;
						transform->velocity.y = 0;
						sprite->Play("WalkLeft");
						sprite->spriteFlip = SDL_FLIP_NONE;
					}

					else if (keystates[SDL_SCANCODE_S]) {
						//inputDelayer.push(SDL_SCANCODE_S);
						transform->velocity.y = 1;
						sprite->Play("WalkDown");
					}

					else if (keystates[SDL_SCANCODE_D]) {
						//inputDelayer.push(SDL_SCANCODE_D);
						transform->velocity.x = 1;
						transform->velocity.y = 0;
						sprite->Play("WalkLeft");
						sprite->spriteFlip = SDL_FLIP_HORIZONTAL;

					}

					else {
						//inputDelayer.push(SDL_SCANCODE_UNKNOWN);
						transform->velocity.y = 0;
						sprite->Play("Idle");
					}

					break;

				case SDLK_a:

					sprite->spriteFlip = SDL_FLIP_NONE;

					if (keystates[SDL_SCANCODE_W]) {
						//inputDelayer.push(SDL_SCANCODE_W);
						transform->velocity.x = 0;
						transform->velocity.y = -1;
						sprite->Play("WalkUp");
					}

					else if (keystates[SDL_SCANCODE_S]) {
						//inputDelayer.push(SDL_SCANCODE_S);
						transform->velocity.x = 0;
						transform->velocity.y = 1;
						sprite->Play("WalkDown");
					}

					else if (keystates[SDL_SCANCODE_D]) {
						//inputDelayer.push(SDL_SCANCODE_D);
						transform->velocity.x = 1;
						sprite->Play("WalkLeft");
						sprite->spriteFlip = SDL_FLIP_HORIZONTAL;
					}

					else {
						//inputDelayer.push(SDL_SCANCODE_UNKNOWN);
						transform->velocity.x = 0;
						sprite->Play("Idle");
					}

					break;

				case SDLK_s:

					if (keystates[SDL_SCANCODE_W]) {
						//inputDelayer.push(SDL_SCANCODE_W);
						transform->velocity.y = -1;
						sprite->Play("WalkUp");
					}

					else if (keystates[SDL_SCANCODE_A]) {
						//inputDelayer.push(SDL_SCANCODE_A);
						transform->velocity.x = -1;
						transform->velocity.y = 0;
						sprite->Play("WalkLeft");
						sprite->spriteFlip = SDL_FLIP_NONE;
					}

					else if (keystates[SDL_SCANCODE_D]) {
						//inputDelayer.push(SDL_SCANCODE_D);
						transform->velocity.x = 1;
						transform->velocity.y = 1;
						sprite->Play("WalkDown");
						sprite->spriteFlip = SDL_FLIP_HORIZONTAL;
					}

					else {
						//inputDelayer.push(SDL_SCANCODE_UNKNOWN);
						transform->velocity.y = 0;
						sprite->Play("Idle");
					}

					break;

				case SDLK_d:

					if (keystates[SDL_SCANCODE_W]) {
						//inputDelayer.push(SDL_SCANCODE_W);
						transform->velocity.x = 0;
						transform->velocity.y = -1;
						sprite->Play("WalkUp");
						sprite->spriteFlip = SDL_FLIP_NONE;
					}

					else if (keystates[SDL_SCANCODE_A]) {
						//inputDelayer.push(SDL_SCANCODE_A);
						transform->velocity.x = -1;
						sprite->Play("WalkLeft");
						sprite->spriteFlip = SDL_FLIP_NONE;
					}

					else if (keystates[SDL_SCANCODE_S]) {
						//inputDelayer.push(SDL_SCANCODE_S);
						transform->velocity.x = 0;
						transform->velocity.y = 1;
						sprite->Play("WalkDown");
						sprite->spriteFlip = SDL_FLIP_NONE;
					}

					else {
						//inputDelayer.push(SDL_SCANCODE_UNKNOWN);
						transform->velocity.x = 0;
						sprite->Play("Idle");
						sprite->spriteFlip = SDL_FLIP_NONE;
					}
					break;
				}
			}
		}
	}
};