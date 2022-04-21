#include "Game.h"
#include "TextureManager.h"
#include "Map.h"
#include "ECS/Components.h"
#include "Vector2D.h"
#include "Collision.h"

Map* map;
Manager manager;


SDL_Renderer* Game::renderer = nullptr; 
SDL_Event Game::event;

std::vector<ColliderComponent*> Game::colliders;

auto& Cleric(manager.addEntity());
auto& wall(manager.addEntity());

enum groupLabels : std::size_t {

	groupMap,
	groupPlayers,
	groupEnemies,
	groupColliders
};

Game::Game() {

}

Game::~Game() {

}

//Window initializer
void Game::init(const char* title, int width, int height, bool fullscreen) {

	int flags = 0;

	if (fullscreen) flags = SDL_WINDOW_FULLSCREEN;

	if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {

		window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
		renderer = SDL_CreateRenderer(window, -1, 0);

		if (renderer) SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

		isRunning = true;
	}

	map = new Map();

	//ECS Implementation

	Map::LoadMap("assets/maps/testMap.map", 16, 16);

	Cleric.addComponent<TransformComponent>(2);
	Cleric.addComponent<SpriteComponent>("assets/characters/cleric_spritesheet.png");
	Cleric.addComponent<KeyboardController>();
	Cleric.addComponent<ColliderComponent>("Cleric");
	Cleric.addGroup(groupPlayers);

	wall.addComponent<TransformComponent>(300.0f, 300.0f, 300, 20, 1);
	wall.addComponent<SpriteComponent>("assets/tiles/Map_tile_08.png");
	wall.addComponent<ColliderComponent>("wall");
	wall.addGroup(groupMap);
}

void Game::handleEvents() {
	
	SDL_PollEvent(&event);
	
	switch (event.type) {
	
	case SDL_QUIT: isRunning = false;

	default: break;
	}
}

//Window Updater
void Game::update() {

	manager.refresh();
	manager.update();

	for (auto cc : colliders) {
		Collision::AABB(Cleric.getComponent<ColliderComponent>(), *cc);
	}
}

auto& tiles(manager.getGroup(groupMap));
auto& players(manager.getGroup(groupPlayers));
auto& enemies(manager.getGroup(groupEnemies));

//Window renderer
void Game::render() {

	SDL_RenderClear(renderer);

	for (auto& t : tiles) { t->draw(); }
	for (auto& p : players) { p->draw(); }
	for (auto& e : enemies) { e->draw(); }

	SDL_RenderPresent(renderer);
}

//Clean up after closing
void Game::clean() {

	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
}

void Game::AddTile(int id, int x, int y) {

	auto& tile(manager.addEntity());
	tile.addComponent<TileComponent>(x, y, 32, 32, id);
	tile.addGroup(groupMap);
}