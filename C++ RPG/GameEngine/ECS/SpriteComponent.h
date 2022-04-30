#pragma once
#include "SDL.h"
#include "..\TextureManager.h"
#include "Components.h"
#include "Animation.h"
#include "..\AssetManager.h"
#include <map>
#include <cmath>

class SpriteComponent : public Component {

private:

	TransformComponent* transform;
	//StatusComponent* status; //causes error
	SDL_Texture* texture;
	SDL_Rect srcRect, destRect;

	bool animated = false;
	int frames = 0;
	int speed = 50;
	bool firstHalf;
	int combatAnimator = 0;

public:

	int animIndex = 0;
	int battleIndex;  //controls where to be placed in battles
	bool attack;

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
		animations.emplace("WalkUp", walkUp);

		animations.emplace("SpoopyFull", spoopyFullHealth);
		animations.emplace("SpoopyWeak", spoopyLowHealth);

		Play("Idle");

		setTex(id);
	}

	~SpriteComponent() {}

	void setTex(std::string id) { texture = Game::assets->GetTexture(id); }

	void init() override {

		transform = &entity->getComponent<TransformComponent>();
		//status = &entity->getComponent<StatusComponent>();

		srcRect.x = srcRect.y = 0;
		srcRect.w = transform->width;
		srcRect.h = transform->height;

		attack = false;
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

				destRect.x = 290;
				if (!attack) { destRect.y = 550; }
				else AnimateAttack(combatAnimator);
				break;

			case 1:
				speed = 800;
				frames = 4;
				animIndex = 2;

				srcRect.x = srcRect.w * static_cast<int>((SDL_GetTicks() / speed) % frames);
				srcRect.y = animIndex * transform->height;

				destRect.w = transform->width;
				destRect.h = transform->height;

				destRect.x = 490;
				if (!attack) { destRect.y = 570; }
				else AnimateAttack(combatAnimator);
				break;

			case 2:
				animIndex = 2;

				srcRect.x = 0;
				srcRect.y = animIndex * transform->height;

				destRect.w = transform->width;
				destRect.h = transform->height;

				destRect.x = 690;
				if (!attack) { destRect.y = 550; }
				else AnimateAttack(combatAnimator);
				break;

			case 3:
				speed = 600;
				frames = 4;
				animIndex = 2;

				srcRect.x = srcRect.w * static_cast<int>((SDL_GetTicks() / speed) % frames);
				srcRect.y = animIndex * transform->height;

				destRect.w = transform->width;
				destRect.h = transform->height;

				destRect.x = 890;

				if (!attack) { destRect.y = 550; }
				else AnimateAttack(combatAnimator);
				break;

			case 4: 
				speed = 800;
				frames = 2;
				//if (status->currentHP > status->maxHP / 2) 
				{ animIndex = 0; }
				//else { animIndex = 1; }

				srcRect.x = srcRect.w * static_cast<int>((SDL_GetTicks() / speed) % frames);
				srcRect.y = animIndex * transform->height;

				destRect.w = transform->width * 0.75;
				destRect.h = transform->height * 0.75;

				if (destRect.x < 40 || destRect.x > 840) { destRect.x = 40; }

				//Trace the Ellipse: (x-440) ^ 2 / 400 ^ 2 + (y - 20) ^ 2 / 80 ^ 2 = 1

				//if (status->currentHP > status->maxHP / 2) {
				Ellipse();
			//	}

			//	else if (status->currentHP <= status->maxHP / 2) { LemniscateOfGerono(); }

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

	inline void LemniscateOfGerono() {

		if (destRect.x >= 40 && destRect.x < 840 && firstHalf) {

			destRect.x += 4;
			destRect.y = round(20 + 0.00125 * (destRect.x - 440) * sqrt((840 - destRect.x) * (destRect.x - 40)));
			if (destRect.x == 840) { firstHalf = false; }

		}

		if (destRect.x > 40 && destRect.x <= 840 && !firstHalf) {

			destRect.x -= 4;
			destRect.y = round(20 - 0.00125 * (destRect.x - 440) * sqrt((840 - destRect.x) * (destRect.x - 40)));
			if (destRect.x == 40) { firstHalf = true; }
		}

	}

	inline void Ellipse() {

		if (destRect.x >= 40 && destRect.x < 840 && firstHalf) {

			destRect.x += 4;
			destRect.y = round(20 + 0.2 * sqrt((840 - destRect.x) * (destRect.x - 40)));
			if (destRect.x == 840) { firstHalf = false; }

		}

		if (destRect.x > 40 && destRect.x <= 840 && !firstHalf) {

			destRect.x -= 4;
			destRect.y = round(20 - 0.2 * sqrt((840 - destRect.x) * (destRect.x - 40)));
			if (destRect.x == 40) { firstHalf = true; }
		}

	}

	inline void AnimateAttack(int &combatAnimator) {

		if (attack) {

			if (combatAnimator < 5) { destRect.y -= 10; combatAnimator++; }
			else if (combatAnimator < 10) { destRect.y -= 10; combatAnimator++; }
			else { attack = false; combatAnimator = 0; }
		}
	}
};