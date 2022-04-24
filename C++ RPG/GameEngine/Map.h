#pragma once
#include "Vector2D.h"
#include <string>

class Map {

public:
	Map(std::string tID, int ms, int ts);
	~Map();

	void LoadMap(std::string path, int sizeX, int sizeY);
	void AddTile(int srcX, int srcY, int x, int y);
	Vector2D GetMapSize();

private:
	std::string texID;
	Vector2D MapSize;
	int mapScale;
	int tileSize;
	int scaledSize;
};