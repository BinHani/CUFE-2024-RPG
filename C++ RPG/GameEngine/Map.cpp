#include "Map.h"
#include "Game.h"
#include "ECS\ECS.h"
#include "ECS\Components.h"
#include "Collision.h"
#include <fstream>
#include <string>

extern Manager manager;

Map::Map(std::string tID, int ms, int ts) : texID(tID), mapScale(ms), tileSize(ts) { scaledSize = ms * ts; }

Map::~Map() { 

}

void Map::LoadMap(std::string path, int sizeX, int sizeY) {

	MapSize.x = scaledSize * sizeX;
	MapSize.y = scaledSize * sizeY;
	ColliderComponent* lastCol = nullptr;
	char tile[3];
	char col;
	char oldCol = '0';
	std::fstream mapFile;
	mapFile.open(path);

	int srcX, srcY;

	for (int y = 0; y < sizeY; y++) {
		for (int x = 0; x < sizeX; x++) {

			for (int i = 0; i < 3; i++) { mapFile.get(tile[i]); }

			srcY = atoi(tile) / 14 * tileSize;
			srcX = atoi(tile) % 14 * tileSize;

			AddTile(srcX, srcY, x * scaledSize, y * scaledSize);

			mapFile.ignore();
		}

		mapFile.ignore();
	}

	mapFile.ignore();

	for (int y = 0; y < sizeY; y++) {
		for (int x = 0; x < sizeX; x++) {

				mapFile.get(col);

				if (col == '1') {
					auto& tcol(manager.addEntity());
					tcol.addComponent<ColliderComponent>("terrain", x * scaledSize, y * scaledSize, 0.5 * scaledSize);
					tcol.addGroup(Game::groupColliders);
				}

				else if (col == '2') {

					if (oldCol == '2') { lastCol->collider.w += lastCol->collider.w; }

					else {
						auto& dcol(manager.addEntity());
						dcol.addComponent<ColliderComponent>("door", x * scaledSize, y * scaledSize, 0.5 * scaledSize);
						dcol.addGroup(Game::groupDoors);
						lastCol = &dcol.getComponent<ColliderComponent>();
					}
				}

				oldCol = col;

			mapFile.ignore();
		}

		mapFile.ignore();
	}

	mapFile.close();
}

void Map::AddTile(int srcX, int srcY, int xpos, int ypos) {

	auto& tile(manager.addEntity());
	tile.addComponent<TileComponent>(srcX, srcY, xpos, ypos, tileSize, mapScale, texID);
	tile.addGroup(Game::groupMap);
}

Vector2D Map::GetMapSize() { return MapSize; std::cout << MapSize << std::endl; }