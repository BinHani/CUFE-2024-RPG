#pragma once
#include "Components.h"
#include "SDL.h"
#include "..\TextureManager.h"
#include "Animation.h"
#include "..\AssetManager.h"
#include <map>
#include <cmath>
#include <iostream>

class SpriteComponent : public Component {

private:

	TransformComponent* transform;
	SDL_Texture* texture;
	SDL_Rect srcRect, destRect;

	bool animated = false;
	int frames = 0;
	int speed = 50;
	int x;
	bool firstHalf;
public:

	int animIndex = 0;
	int battleIndex = 0;  //controls where to be placed in battles

	std::map<const char*, Animation> animations;

	SDL_RendererFlip spriteFlip = SDL_FLIP_NONE;

	SpriteComponent() = default;

	SpriteComponent(std::string id) {setTex(id); }

	SpriteComponent(std::string id, bool isAnimated) { 
		
		animated = isAnimated;

		Animation idle = Animation(0, 1, 100);
		Animation walkDown = Animation(0, 4, 100);
		Animation walkLeft = Animation(1, 3, 100);
		Animation walkUp = Animation(2, 3, 100);
		Animation spoopyFullHealth = Animation(0, 2, 100);
		Animation spoopyLowHealth = Animation(1, 2, 100);

		animations.emplace("Idle", idle);
		animations.emplace("WalkDown", walkDown);
		animations.emplace("WalkLeft", walkLeft);
		animations.emplace("WalkUp", spoopyFullHealth);
		animations.emplace("WalkUp", spoopyLowHealth);

		Play("Idle");

		setTex(id);
	}

	~SpriteComponent() {}

	void setTex(std::string id) { texture = Game::assets->GetTexture(id); }

	void init() override {

		transform = &entity->getComponent<TransformComponent>();

		srcRect.x = srcRect.y = 0;
		srcRect.w = transform->width;
		srcRect.h = transform->height;

		x = 40;
		firstHalf = true;
	}

	void update() override {

		switch (_worldState) {

		case stateRealTime:

			//Go to current frame in animation
			if (animated) { srcRect.x = srcRect.w * static_cast<int>((SDL_GetTicks() / speed) % frames); }

			srcRect.y = animIndex * transform->height;

			destRect.x = static_cast<int>(transform->position.x) - Game::camera.x;
			destRect.y = static_cast<int>(transform->position.y) - Game::camera.y;
			destRect.w = transform->width * transform->scale;
			destRect.h = transform->height * transform->scale;

			break;

		case stateTurnBased:

			Play("WalkUp");

			switch (battleIndex) {

			case 0:

				frames = 3;
				animIndex = 2;
				speed = 1000;

				srcRect.x = srcRect.w * static_cast<int>((SDL_GetTicks() / speed) % frames);
				srcRect.y = animIndex * transform->height;

				destRect.w = transform->width;
				destRect.h = transform->height;

				destRect.x = 360;
				destRect.y = 550;
				break;

			case 1:
				speed = 800;
				frames = 4;
				animIndex = 2;

				srcRect.x = srcRect.w * static_cast<int>((SDL_GetTicks() / speed) % frames);
				srcRect.y = animIndex * transform->height;

				destRect.w = transform->width;
				destRect.h = transform->height;

				destRect.x = 560;
				destRect.y = 570;
				break;

			case 2:
				animIndex = 2;

				srcRect.x = 0;
				srcRect.y = animIndex * transform->height;

				destRect.w = transform->width;
				destRect.h = transform->height;

				destRect.x = 760;
				destRect.y = 550;
				break;

			case 3:
				//reserved
				break;

			case 4: 
				speed = 800;
				frames = 2;
				animIndex = 0;

				srcRect.x = srcRect.w * static_cast<int>((SDL_GetTicks() / speed) % frames);
				srcRect.y = animIndex * transform->height;

				destRect.w = transform->width * 0.75;
				destRect.h = transform->height * 0.75;
				
				if (x >= 40 && x < 840 && firstHalf) {

					destRect.x = x += 4;
					destRect.y = round(20 + 0.2 * sqrt((840 - x) * (x - 40)));
					if (x == 840) { firstHalf = false; }
					std::cout << destRect.x << std::endl;
				
				}

				if (x > 40 && x <= 840 && !firstHalf) {

					destRect.x = x -= 4;
					destRect.y = round(20 - 0.2 * sqrt((840 - x) * (x - 40)));
					if (x == 40) { firstHalf = true; }
					std::cout << destRect.x << std::endl;
				}

				break;
			}

			break;
		}
	}

	void draw() override { TextureManager::Draw(texture, srcRect, destRect, spriteFlip); }

	void Play(const char* animName) {

		frames = animations[animName].frames;
		animIndex = animations[animName].index;
		speed = animations[animName].speed;
	}

};