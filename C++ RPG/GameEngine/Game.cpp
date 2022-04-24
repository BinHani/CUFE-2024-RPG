#include "Game.h"
#include "TextureManager.h"
#include "Map.h"
#include "ECS\Components.h"
#include "Vector2D.h"
#include "Collision.h"
#include "AssetManager.h"
#include <sstream>

Map* map;
Manager manager;

SDL_Renderer* Game::renderer = nullptr; 
SDL_Event Game::event;

SDL_Rect Game::camera = { 0,0,1280,720 };

AssetManager* Game::assets = new AssetManager(&manager);

bool Game::isRunning = false;

auto& Cleric(manager.addEntity());
auto& Wizard(manager.addEntity());
auto& Leader(manager.addEntity());

auto& Flameball(manager.addEntity());

auto& label(manager.addEntity());

Game::Game() {}

Game::~Game() {}

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

	if (TTF_Init() == -1) { std::cout << "Error: SDL_TTF" << std::endl; }

	assets->AddTexture("Terrain", "assets/Map_tiles.png");
	assets->AddTexture("Cleric", "assets/cleric_spritesheet.png");
	assets->AddTexture("Wizard", "assets/wizard_spritesheet.png");
	assets->AddTexture("Warrior", "assets/warrior_spritesheet.png");
	assets->AddTexture("Flameball", "assets/flameball.png");
	assets->AddFont("Pixellari", "assets/Pixellari.ttf", 16);

	map = new Map("Terrain", 2, 16);

	//ECS Implementation

	map->LoadMap("assets/cave1.map", 50, 50);

	assets->CreatePlayerCharacter(Leader, Vector2D(550.0f, 600.0f), 184, 109, 0.25, true, "Warrior");
	assets->CreatePlayerCharacter(Cleric, Vector2D(550.0f, 632.0f), 220, 110, 0.25, true, "Cleric");
	assets->CreatePlayerCharacter(Wizard, Vector2D(550.0f, 664.0f), 222, 111, 0.25, true, "Wizard");

	SDL_Color white = { 0xFF,0xFF,0xFF,0xFF };
	label.addComponent<UILabel>(10, 10, "Test String", "Pixellari", white);

	assets->CreateProjectile(Flameball, Vector2D(600, 640), Vector2D(2,0), 300, 1, "Flameball");
}

auto& tiles(manager.getGroup(Game::groupMap));
auto& players(manager.getGroup(Game::groupPlayerCharacters));
auto& colliders(manager.getGroup(Game::groupColliders));
auto& projectiles(manager.getGroup(Game::groupProjectiles));

void Game::handleEvents() {
	
	SDL_PollEvent(&event);
	
	switch (event.type) {
	
	case SDL_QUIT: isRunning = false;

	default: break;
	}
}

//Window Updater
void Game::update() {

	Vector2D clericPos = Cleric.getComponent<TransformComponent>().position;
	Vector2D wizardPos = Wizard.getComponent<TransformComponent>().position;
	Vector2D warriorPos = Leader.getComponent<TransformComponent>().position;

	std::stringstream ss;
	ss << "Player position: " << clericPos;
	label.getComponent<UILabel>().SetLabelText(ss.str(), "Pixellari");

	manager.refresh();
	manager.update();

	for (auto& c : colliders) {
		
		if (Collision::AABB(c->getComponent<ColliderComponent>().collider, Cleric.getComponent<ColliderComponent>().collider) ||
			Collision::AABB (c->getComponent<ColliderComponent>().collider, Wizard.getComponent<ColliderComponent>().collider) ||
			Collision::AABB(c->getComponent<ColliderComponent>().collider, Leader.getComponent<ColliderComponent>().collider)) {
			
			Cleric.getComponent<TransformComponent>().position = clericPos;
			Wizard.getComponent<TransformComponent>().position = wizardPos;
			Leader.getComponent<TransformComponent>().position = warriorPos;
		}
	}

	for (auto& p : projectiles) {

		if (Collision::AABB(Cleric.getComponent<ColliderComponent>().collider, p->getComponent<ColliderComponent>().collider )) {
			
			std::cout << "hit Cleric" << std::endl;
			p->destroy();
		}
	}

	camera.x = Cleric.getComponent<TransformComponent>().position.x - 640;
	camera.y = Cleric.getComponent<TransformComponent>().position.y - 360;

	if (camera.x < 0) camera.x = 0;
	if (camera.y < 0) camera.y = 0;
	if (camera.x + camera.w > map->GetMapSize().x) camera.x = map->GetMapSize().x - camera.w;
	if (camera.y + camera.h > map->GetMapSize().y) camera.y = map->GetMapSize().y - camera.h;
}

//Window renderer
void Game::render() {

	SDL_RenderClear(renderer);

	for (auto& t : tiles) { t->draw(); }
	for (auto& c : colliders) { c->draw(); }
	for (auto& p : players) { p->draw(); }
	for (auto& p : projectiles) { p->draw(); }
	label.draw();

	SDL_RenderPresent(renderer);
}

//Clean up after closing
void Game::clean() {

	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
}