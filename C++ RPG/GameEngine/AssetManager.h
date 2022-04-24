#pragma once
#include <map>
#include <string>
#include "TextureManager.h"
#include "Vector2D.h"
#include "ECS\ECS.h"
#include "SDL_ttf.h"

class AssetManager {

public:
	AssetManager(Manager* man);
	~AssetManager();

	void CreatePlayerCharacter(Entity& charName, Vector2D pos, int h, int w, float sc, bool isAnimated, std::string id);
	void CreateProjectile(Entity& projName, Vector2D pos, Vector2D vel, int range, int speed, std::string id);

	void AddTexture(std::string id, const char* path);
	SDL_Texture* GetTexture(std::string id);

	void AddFont(std::string id, std::string path, int fontSize);
	TTF_Font* GetFont(std::string id);

private:

	Manager* manager;
	std::map<std::string, SDL_Texture*> textures;
	std::map<std::string, TTF_Font*> fonts;
};