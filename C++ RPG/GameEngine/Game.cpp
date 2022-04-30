#include "Game.h"
#include "TextureManager.h"
#include "Map.h"
#include "ECS\Components.h"
#include "Vector2D.h"
#include "Collision.h"
#include "AssetManager.h"
#include "WorldState.h"
#include "CombatFunctions.h"
#include <sstream>
#include <vector>

//Decalaring all required variables
Manager manager;
AssetManager* Game::assets = new AssetManager(&manager);
Map* map;

//General-purpose variables
SDL_Renderer* Game::renderer = nullptr; 
SDL_Event Game::event;
SDL_Rect Game::camera = { 0,0,1280,720 };
bool Game::isRunning = false;
worldState _worldState = stateRealTime;

//variables for wall-collision
Vector2D clericPos;
Vector2D wizardPos;
Vector2D warriorPos;
Vector2D rangerPos;

//variables for turn based game loop
const uint8_t* keystates = SDL_GetKeyboardState(NULL);
Entity* currentEnemies[4];
bool enemyExists[4];
StatusComponent* enemyStatus[4];
int enemyCount, currentTurn = 0;
bool actorsDetermined = false; 
bool actorsSorted = false;
SDL_Keycode actionType = SDLK_UNKNOWN;
bool actionQueued = false;
SDL_Keycode enemySelector = SDLK_UNKNOWN;
bool enemySelected = false;
SDL_Color white = { 0xFF,0xFF,0xFF,0xFF };

//Entity-Group creation
auto& Cleric(manager.addEntity());
auto& Wizard(manager.addEntity());
auto& Warrior(manager.addEntity());
auto& Ranger(manager.addEntity());
auto& Spoopy(manager.addEntity());
auto& Flameball(manager.addEntity());
auto& label(manager.addEntity());

auto& tiles(manager.getGroup(Game::groupMap));
auto& players(manager.getGroup(Game::groupPlayerCharacters));
auto& enemies(manager.getGroup(Game::groupEnemyCharacters));
auto& colliders(manager.getGroup(Game::groupColliders));
auto& projectiles(manager.getGroup(Game::groupProjectiles));
auto& doors(manager.getGroup(Game::groupDoors));

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

	//Asset Preparation (Asset Manager initialization)
	assets->AddTexture("Overworld", "assets/overworld.png");
	assets->AddTexture("Cleric", "assets/cleric_spritesheet.png");
	assets->AddTexture("Wizard", "assets/wizard_spritesheet.png");
	assets->AddTexture("Warrior", "assets/warrior_spritesheet.png");
	assets->AddTexture("Ranger", "assets/ranger_spritesheet.png");
	assets->AddTexture("Spoopy", "assets/spoopy_spritesheet.png");
	assets->AddTexture("Flameball", "assets/flameball.png");
	assets->AddTexture("Background", "assets/underground_battle.png");
	assets->AddFont("Pixellari", "assets/Pixellari.ttf", 16);

	map = new Map("Overworld", 4, 16);

	//ECS Implementation

	map->LoadMap("assets/overworld.map", 130, 50);

	//Cleric
	Cleric.addComponent<TransformComponent>(301.0f, 2944.0f, 220, 110, 0.35);
	Cleric.addComponent<StatusComponent>(30, 50, 15, 40);
	Cleric.addComponent<SpriteComponent>("Cleric", true);
	Cleric.addComponent<ColliderComponent>("Cleric");
	Cleric.addComponent<KeyboardController>();
	Cleric.getComponent<SpriteComponent>().battleIndex = 0;
	Cleric.getComponent<StatusComponent>().battleIndex = 0;
	Cleric.addGroup(Game::groupPlayerCharacters);

	//Warrior
	Warrior.addComponent<TransformComponent>(301.0f, 2998.0f, 184, 109, 0.35);
	Warrior.addComponent<StatusComponent>(50, 15, 30, 40);
	Warrior.addComponent<SpriteComponent>("Warrior", true);
	Warrior.addComponent<ColliderComponent>("Warrior");
	Warrior.addComponent<KeyboardController>();
	Warrior.addComponent<FightNight>();
	Warrior.getComponent<SpriteComponent>().battleIndex = 1;
	Warrior.getComponent<StatusComponent>().battleIndex = 1;
	Warrior.addGroup(Game::groupPlayerCharacters);

	//Wizard
	Wizard.addComponent<TransformComponent>(256.0f, 2944.0f, 220, 110, 0.35);
	Wizard.addComponent<StatusComponent>(30, 35, 45, 40);
	Wizard.addComponent<SpriteComponent>("Wizard", true);
	Wizard.addComponent<ColliderComponent>("Wizard");
	Wizard.addComponent<KeyboardController>();
	Wizard.getComponent<SpriteComponent>().battleIndex = 2;
	Wizard.getComponent<StatusComponent>().battleIndex = 2;
	Wizard.addGroup(Game::groupPlayerCharacters);

	//Ranger
	Ranger.addComponent<TransformComponent>(256.0f, 2989.0f, 220, 110, 0.35);
	Ranger.addComponent<StatusComponent>(30, 50, 20, 40);
	Ranger.addComponent<SpriteComponent>("Ranger", true);
	Ranger.addComponent<ColliderComponent>("Ranger");
	Ranger.addComponent<KeyboardController>();
	Ranger.getComponent<SpriteComponent>().battleIndex = 3;
	Ranger.getComponent<StatusComponent>().battleIndex = 3;
	Ranger.addGroup(Game::groupPlayerCharacters);

	//Spoopy
	Spoopy.addComponent<TransformComponent>(1100.0f, 1200.0f, 582, 370, 1);
	Spoopy.addComponent<StatusComponent>(450, 50, 30, 55);
	Spoopy.addComponent<SpriteComponent>("Spoopy", true);
	Spoopy.getComponent<SpriteComponent>().battleIndex = 4;
	Spoopy.getComponent<StatusComponent>().battleIndex = 4;
	Spoopy.addComponent<EnemyBehaviour>();
	Spoopy.addGroup(Game::groupEnemyCharacters);

	assets->CreateProjectile(Flameball, Vector2D(600, 640), Vector2D(2, 0), 300, 1, "Flameball");
	label.addComponent<UILabel>(10, 10, "Player Position", "Pixellari", white);
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

	std::stringstream ss;

	switch (_worldState) {

	case stateRealTime:

		clericPos = Cleric.getComponent<TransformComponent>().position;
		wizardPos = Wizard.getComponent<TransformComponent>().position;
		warriorPos = Warrior.getComponent<TransformComponent>().position;
		rangerPos = Ranger.getComponent<TransformComponent>().position;


		ss << "Player position: " << clericPos;
		label.getComponent<UILabel>().SetLabelText(ss.str(), "Pixellari");

		manager.refresh();
		manager.update();

		for (auto& c : colliders) {

			if (Collision::AABB(c->getComponent<ColliderComponent>().collider, Cleric.getComponent<ColliderComponent>().collider) ||
				Collision::AABB(c->getComponent<ColliderComponent>().collider, Wizard.getComponent<ColliderComponent>().collider) ||
				Collision::AABB(c->getComponent<ColliderComponent>().collider, Warrior.getComponent<ColliderComponent>().collider) ||
				Collision::AABB(c->getComponent<ColliderComponent>().collider, Ranger.getComponent<ColliderComponent>().collider)) {

				Cleric.getComponent<TransformComponent>().position = clericPos;
				Wizard.getComponent<TransformComponent>().position = wizardPos;
				Warrior.getComponent<TransformComponent>().position = warriorPos;
				Ranger.getComponent<TransformComponent>().position = rangerPos;
			}
		}

		for (int i = 0; i < doors.size(); i++) {

			if (Collision::AABB(doors[i]->getComponent<ColliderComponent>().collider, Cleric.getComponent<ColliderComponent>().collider) ||
				Collision::AABB(doors[i]->getComponent<ColliderComponent>().collider, Wizard.getComponent<ColliderComponent>().collider) ||
				Collision::AABB(doors[i]->getComponent<ColliderComponent>().collider, Warrior.getComponent<ColliderComponent>().collider) ||
				Collision::AABB(doors[i]->getComponent<ColliderComponent>().collider, Ranger.getComponent<ColliderComponent>().collider)) {

				if (i % 2 == 0) {

					Cleric.getComponent<TransformComponent>().position.x = doors[i + 1]->getComponent<ColliderComponent>().collider.x + 45.0f;
					Cleric.getComponent<TransformComponent>().position.y = doors[i + 1]->getComponent<ColliderComponent>().collider.y + 45.0f;

					Warrior.getComponent<TransformComponent>().position.x = doors[i + 1]->getComponent<ColliderComponent>().collider.x + 45.0f;
					Warrior.getComponent<TransformComponent>().position.y = doors[i + 1]->getComponent<ColliderComponent>().collider.y + 99.0f;

					Wizard.getComponent<TransformComponent>().position.x = doors[i + 1]->getComponent<ColliderComponent>().collider.x;
					Wizard.getComponent<TransformComponent>().position.y = doors[i + 1]->getComponent<ColliderComponent>().collider.y + 45.0f;

					Ranger.getComponent<TransformComponent>().position.x = doors[i + 1]->getComponent<ColliderComponent>().collider.x;
					Ranger.getComponent<TransformComponent>().position.y = doors[i + 1]->getComponent<ColliderComponent>().collider.y + 90.0f;
				}

				else if (i % 2 == 1) {

					Cleric.getComponent<TransformComponent>().position.x = doors[i - 1]->getComponent<ColliderComponent>().collider.x + 45.0f;
					Cleric.getComponent<TransformComponent>().position.y = doors[i - 1]->getComponent<ColliderComponent>().collider.y + 45.0f;

					Warrior.getComponent<TransformComponent>().position.x = doors[i - 1]->getComponent<ColliderComponent>().collider.x + 45.0f;
					Warrior.getComponent<TransformComponent>().position.y = doors[i - 1]->getComponent<ColliderComponent>().collider.y + 99.0f;

					Wizard.getComponent<TransformComponent>().position.x = doors[i - 1]->getComponent<ColliderComponent>().collider.x;
					Wizard.getComponent<TransformComponent>().position.y = doors[i - 1]->getComponent<ColliderComponent>().collider.y + 45.0f;

					Ranger.getComponent<TransformComponent>().position.x = doors[i - 1]->getComponent<ColliderComponent>().collider.x;
					Ranger.getComponent<TransformComponent>().position.y = doors[i - 1]->getComponent<ColliderComponent>().collider.y + 90.0f;
				}
			}
		}

		for (auto& p : projectiles) {

			if (Collision::AABB(Cleric.getComponent<ColliderComponent>().collider, p->getComponent<ColliderComponent>().collider)) {

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

		break;

	case stateTurnBased:
	{

		manager.refresh();
		manager.update();

		if (!actorsDetermined) {

			//Generate enemy encounter randomly
			enemyCount = CombatFunctions::GenerateRandomEncounter(&manager, currentEnemies);
			std::cout << "Enemy Count: " << enemyCount << std::endl;
			actorsDetermined = true;
		}

		if (!actorsSorted) {

			//sort enemies by AP descendingly and generate an array of pointers to their status components
			CombatFunctions::SortEnemiesByAP(currentEnemies, enemyCount, enemyStatus);
			actorsSorted = true;
		}

		if (Game::event.type == SDL_KEYDOWN && !actionQueued) {
			actionType = CombatFunctions::GetAction(event);
			if (actionType != SDLK_UNKNOWN) { actionQueued = true; }
			
		}

		if (Game::event.type == SDL_KEYDOWN && !enemySelected && actionQueued) {
			enemySelector = CombatFunctions::GetTarget(event);
			if (enemySelector != SDLK_UNKNOWN) { enemySelected = true; }
			
		}

		for (auto& p : players) {

			//Make each player attack once per turn make and sure enemies don't attack themselves
			
			if (p->getComponent<StatusComponent>().isAlive && currentTurn == p->getComponent<StatusComponent>().battleIndex) {

				if (actionQueued) {

					switch (actionType) {

						case SDLK_j:	//attack
						
						if (enemySelected) {

						if (CombatFunctions::Attack(p, currentEnemies, enemyStatus, enemySelector, enemyCount)) {

							p->getComponent<SpriteComponent>().attack = true;
							currentTurn++;

						}

						actionQueued = enemySelected = false;

						}

						break;

					case SDLK_k:	//defend

						if (CombatFunctions::Defend(p)) { currentTurn++; }

						actionQueued = false;

						break;

					case SDLK_l:	//rest

						if (CombatFunctions::Rest(p)) { currentTurn++; }

						actionQueued = false;

						break;
					}

					//std::cout << "currentTurn: " << currentTurn << std::endl;
					
				}

			}
		}

		break; //end of turn based game loop

	case stateGameOver:

		//game over method
		break;
	}
	}
}

Transition* blinds = new Transition(Game::renderer, "assets/blinds.png",TRANS_TYPE::IN, 5);
Transition* eyes = new Transition(Game::renderer, "assets/eyes.png", TRANS_TYPE::IN, 5);
Transition* wipe = new Transition(Game::renderer, "assets/wipe.png", TRANS_TYPE::IN, 5);

//Window renderer
void Game::render() {

	SDL_RenderClear(renderer);
	
	switch (_worldState) {
		
	case stateRealTime:

		for (auto& t : tiles) { t->draw(); }
		for (auto& c : colliders) { c->draw(); }
		for (auto& p : players) { p->draw(); }
		for (auto& p : projectiles) { p->draw(); }
		for (auto& e : enemies) { e->draw(); }

		label.draw();

		break;


	case stateTurnBased:

			blinds->Update(renderer);
			SDL_RenderCopy(renderer, blinds->texture, NULL, NULL);

			if (blinds->isComplete()) { 
				SDL_RenderCopy(renderer, assets->GetTexture("Background"), NULL, NULL);
				for (auto& p : players) { p->draw(); }
				for (int e = 0; e < enemyCount; e++) 
				{ currentEnemies[e]->draw(); }
			}

		break;

	case stateGameOver:

		//Render Game Over screen
		break;

	}

	SDL_RenderPresent(renderer);

}

//Clean up after closing
void Game::clean() {

	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
}