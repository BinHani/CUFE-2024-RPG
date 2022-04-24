#pragma once
#include "ECS.h"
#include "SDL.h"

class TileComponent : public Component {

public:

	SDL_Texture* texture;
	SDL_Rect srcRect, destRect;
	Vector2D position;

	TileComponent() = default;

	~TileComponent() { SDL_DestroyTexture(texture); }

	TileComponent(int srcX, int srcY, int xpos, int ypos, int tsize, int tscale, std::string id) {

		texture = Game::assets->GetTexture(id);

		position.x = static_cast<float>(xpos);
		position.y = static_cast<float>(ypos);

		srcRect.x = srcX;
		srcRect.y = srcY;
		srcRect.w = srcRect.h = tsize;

		destRect.w = destRect.h = tsize * tscale;
	}

	void update() override {

		destRect.x = static_cast<float>(position.x - Game::camera.x);
		destRect.y = static_cast<float>(position.y - Game::camera.y);
	}

	void draw() override { TextureManager::Draw(texture, srcRect, destRect, SDL_FLIP_NONE); }
};