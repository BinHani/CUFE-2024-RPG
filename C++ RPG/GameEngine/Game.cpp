#include "Game.h"
#include "TextureManager.h"
#include "Map.h"
#include "ECS\Components.h"
#include "Vector2D.h"
#include "Collision.h"
#include "AssetManager.h"
#include "WorldState.h"
#include "CombatFunctions.h"
#include "EnemyBehaviour.h"
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

//variables for collision handling
Vector2D clericPos;
Vector2D wizardPos;
Vector2D warriorPos;
Vector2D rangerPos;

//variables for turn based game loop
const uint8_t* keystates = SDL_GetKeyboardState(NULL);
EnemyBehaviour behaviour[4];
int enemyCount, currentTurn = 0;
bool stateInitialized = false;
SDL_Keycode actionType = SDLK_UNKNOWN;
bool actionQueued = false;
SDL_Keycode enemySelector = SDLK_UNKNOWN;
bool enemySelected = false;
bool lossCheck = false;
bool winCheck = false;
SDL_Color white = { 0xFF,0xFF,0xFF,0xFF };

//Entity-Group creation
auto& Cleric(manager.addEntity());
auto& Warrior(manager.addEntity());
auto& Wizard(manager.addEntity());
auto& Ranger(manager.addEntity());
auto& Spoopy(manager.addEntity());
auto& Tree(manager.addEntity());
auto& Golem(manager.addEntity());
auto& SwordSentinel(manager.addEntity());
auto& ShieldSentinel(manager.addEntity());
auto& Imp(manager.addEntity());
auto& ImpFighter(manager.addEntity());
auto& label(manager.addEntity());

auto& tiles(manager.getGroup(Game::groupMap));
auto& players(manager.getGroup(Game::groupPlayerCharacters));
auto& enemies(manager.getGroup(Game::groupEnemyCharacters));
auto& colliders(manager.getGroup(Game::groupColliders));
auto& doors(manager.getGroup(Game::groupDoors));
auto& currentEnemies(manager.getGroup(Game::groupCurrentEnemies));

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
	assets->AddTexture("Tree", "assets/tree_spritesheet.jpeg");
	assets->AddTexture("Golem", "assets/golem_spritesheet.jpeg");
	assets->AddTexture("Sword Sentinel", "assets/sword_automaton_spritesheet.png");
	assets->AddTexture("Shield Sentinel", "assets/shield_automaton_spritesheet.png");
	assets->AddTexture("Imp", "assets/birb_spritesheet.png");
	assets->AddTexture("Imp Fighter", "assets/birb_fighter_spritesheet.png");
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
	Cleric.addComponent<FightNight>();
	Cleric.getComponent<StatusComponent>().combatIndex = 0;
	Cleric.getComponent<SpriteComponent>().battleIndex = 0;
	Cleric.addGroup(Game::groupPlayerCharacters);

	//Warrior
	Warrior.addComponent<TransformComponent>(Cleric.getComponent<TransformComponent>().position.x, Cleric.getComponent<TransformComponent>().position.y + 54, 184, 109, 0.35);
	Warrior.addComponent<StatusComponent>(50, 15, 30, 40);
	Warrior.addComponent<SpriteComponent>("Warrior", true);
	Warrior.addComponent<ColliderComponent>("Warrior");
	Warrior.addComponent<KeyboardController>();
	Warrior.getComponent<StatusComponent>().combatIndex = 1;
	Warrior.getComponent<SpriteComponent>().battleIndex = 1;
	Warrior.addGroup(Game::groupPlayerCharacters);

	//Wizard
	Wizard.addComponent<TransformComponent>(Cleric.getComponent<TransformComponent>().position.x - 45, Cleric.getComponent<TransformComponent>().position.y, 220, 110, 0.35);
	Wizard.addComponent<StatusComponent>(30, 35, 45, 40);
	Wizard.addComponent<SpriteComponent>("Wizard", true);
	Wizard.addComponent<ColliderComponent>("Wizard");
	Wizard.addComponent<KeyboardController>();
	Wizard.getComponent<StatusComponent>().combatIndex = 2;
	Wizard.getComponent<SpriteComponent>().battleIndex = 2;
	Wizard.addGroup(Game::groupPlayerCharacters);

	//Ranger
	Ranger.addComponent<TransformComponent>(Cleric.getComponent<TransformComponent>().position.x - 45, Cleric.getComponent<TransformComponent>().position.y + 45, 220, 110, 0.35);
	Ranger.addComponent<StatusComponent>(30, 50, 20, 40);
	Ranger.addComponent<SpriteComponent>("Ranger", true);
	Ranger.addComponent<ColliderComponent>("Ranger");
	Ranger.addComponent<KeyboardController>();
	Ranger.getComponent<StatusComponent>().combatIndex = 3;
	Ranger.getComponent<SpriteComponent>().battleIndex = 3;
	Ranger.addGroup(Game::groupPlayerCharacters);

	//Spoopy
	Spoopy.addComponent<TransformComponent>(1100.0f, 1200.0f, 582, 370, 1);
	Spoopy.addComponent<StatusComponent>(450, 50, 30, 55);
	Spoopy.getComponent<StatusComponent>().SetDecisionCoeffs(0.075, 0.03, 0.02, 0.025);
	Spoopy.getComponent<StatusComponent>().SetTargetWeights(0.5, 0.5, 0.7, 0.3);
	Spoopy.addComponent<SpriteComponent>("Spoopy", true);
	Spoopy.getComponent<SpriteComponent>().battleIndex = 4;
	Spoopy.addGroup(Game::groupEnemyCharacters);

	//Tree
	Tree.addComponent<TransformComponent>(1100.0f, 1200.0f, 582, 370, 1);
	Tree.addComponent<StatusComponent>(450, 50, 30, 55);
	Tree.getComponent<StatusComponent>().SetDecisionCoeffs(0.05, 0.06, 0.03, 0.05);
	Tree.getComponent<StatusComponent>().SetTargetWeights(0.7, 0.3, 0.7, 0.4);
	Tree.addComponent<SpriteComponent>("Tree", false);
	Tree.getComponent<SpriteComponent>().battleIndex = 5;
	Tree.addGroup(Game::groupEnemyCharacters);

	//Golem
	Golem.addComponent<TransformComponent>(1100.0f, 1200.0f, 582, 370, 1);
	Golem.addComponent<StatusComponent>(450, 50, 30, 55);
	Golem.getComponent<StatusComponent>().SetDecisionCoeffs(0.06, 0.04, 0.03, 0.01);
	Golem.getComponent<StatusComponent>().SetTargetWeights(0.6, 0.7, 0.4, 0.1);
	Golem.addComponent<SpriteComponent>("Golem", false);
	Golem.getComponent<SpriteComponent>().battleIndex = 6;
	Golem.addGroup(Game::groupEnemyCharacters);

	//Sword Sentinel
	SwordSentinel.addComponent<TransformComponent>(1100.0f, 1200.0f, 582, 370, 1);
	SwordSentinel.addComponent<StatusComponent>(450, 50, 30, 55);
	SwordSentinel.getComponent<StatusComponent>().SetDecisionCoeffs(0.09, 0.02, 0.03, 0.01);
	SwordSentinel.getComponent<StatusComponent>().SetTargetWeights(0.8, 0.7, 0.4, 0.1);
	SwordSentinel.addComponent<SpriteComponent>("Sword Sentinel", false);
	SwordSentinel.getComponent<SpriteComponent>().battleIndex = 7;
	SwordSentinel.addGroup(Game::groupEnemyCharacters);

	//Shield Sentinel
	ShieldSentinel.addComponent<TransformComponent>(1100.0f, 1200.0f, 582, 370, 1);
	ShieldSentinel.addComponent<StatusComponent>(450, 50, 30, 55);
	ShieldSentinel.getComponent<StatusComponent>().SetDecisionCoeffs(0.06, 0.06, 0.03, 0.01);
	ShieldSentinel.getComponent<StatusComponent>().SetTargetWeights(0.6, 0.4, 0.7, 0.1);
	ShieldSentinel.addComponent<SpriteComponent>("Shield Sentinel", false;
	ShieldSentinel.getComponent<SpriteComponent>().battleIndex = 8;
	ShieldSentinel.addGroup(Game::groupEnemyCharacters);

	//Imp
	Imp.addComponent<TransformComponent>(1100.0f, 1200.0f, 582, 370, 1);
	Imp.addComponent<StatusComponent>(450, 50, 30, 55);
	Imp.getComponent<StatusComponent>().SetDecisionCoeffs(0.03, 0.08, 0.04, 0.05);
	Imp.getComponent<StatusComponent>().SetTargetWeights(0.3, 0.7, 0.8, 0.5);
	Imp.addComponent<SpriteComponent>("Imp", false);
	Imp.getComponent<SpriteComponent>().battleIndex = 9;
	Imp.addGroup(Game::groupEnemyCharacters);

	//Imp Fighter
	ImpFighter.addComponent<TransformComponent>(1100.0f, 1200.0f, 582, 370, 1);
	ImpFighter.addComponent<StatusComponent>(450, 50, 30, 55);
	ImpFighter.getComponent<StatusComponent>().SetDecisionCoeffs(0.065, 0.075, 0.03, 0.055);
	ImpFighter.getComponent<StatusComponent>().SetTargetWeights(0.6, 0.7, 0.5, 0.4);
	ImpFighter.addComponent<SpriteComponent>("Imp Fighter", false);
	ImpFighter.getComponent<SpriteComponent>().battleIndex = 10;
	ImpFighter.addGroup(Game::groupEnemyCharacters);



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

		if (!stateInitialized) {
		
			Warrior.getComponent<TransformComponent>().position.x = Cleric.getComponent<TransformComponent>().position.x;
			Warrior.getComponent<TransformComponent>().position.y = Cleric.getComponent<TransformComponent>().position.y + 54;

			Wizard.getComponent<TransformComponent>().position.x = Cleric.getComponent<TransformComponent>().position.x - 45;
			Wizard.getComponent<TransformComponent>().position.y = Cleric.getComponent<TransformComponent>().position.y;

			Ranger.getComponent<TransformComponent>().position.x = Cleric.getComponent<TransformComponent>().position.x - 45;
			Ranger.getComponent<TransformComponent>().position.y = Cleric.getComponent<TransformComponent>().position.y + 45;

			stateInitialized = true;
		}

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

		camera.x = Cleric.getComponent<TransformComponent>().position.x - 640;
		camera.y = Cleric.getComponent<TransformComponent>().position.y - 360;


		if (camera.x < 0) camera.x = 0;
		if (camera.y < 0) camera.y = 0;
		if (camera.x + camera.w > map->GetMapSize().x) camera.x = map->GetMapSize().x - camera.w;
		if (camera.y + camera.h > map->GetMapSize().y) camera.y = map->GetMapSize().y - camera.h;

		break;

	case stateTurnBased:

		manager.refresh();
		manager.update();

		if (!stateInitialized) {

			//Generate enemy encounter randomly
			enemyCount = CombatFunctions::GenerateRandomEncounter();

			//sort enemies by AP descendingly and generate an array of pointers to their status components
			CombatFunctions::InitializeEnemies(enemyCount, behaviour);

			Cleric.getComponent<FightNight>().steps = 0;

			stateInitialized = true;
		}

		if (Game::event.type == SDL_KEYDOWN && !actionQueued) {
			actionType = CombatFunctions::GetAction(event);
			if (actionType != SDLK_UNKNOWN) { actionQueued = true; }
			
		}

		if (Game::event.type == SDL_KEYDOWN && !enemySelected && actionQueued) {
			enemySelector = CombatFunctions::GetTarget(event);
			if (enemySelector != SDLK_UNKNOWN) { enemySelected = true; }
			
		}

		// This is the player combat loop. It loops over the player-controlled characters and does an action (attack, defend, or rest) based on player input
		for (auto& p : players) {

			if (currentTurn == p->getComponent<StatusComponent>().combatIndex) {

				if (p->getComponent<StatusComponent>().isAlive) { //Make each player attack once per turn

					if (actionQueued) {

						switch (actionType) {

						case SDLK_j:	//attack

							if (enemySelected) {

								if (CombatFunctions::PlayerAttack(&p->getComponent<StatusComponent>(), enemySelector, enemyCount, winCheck)) {

									p->getComponent<SpriteComponent>().attack = true;
									currentTurn++;
								}

								actionQueued = enemySelected = false;
							}

							break;

						case SDLK_k:	//defend

							if (CombatFunctions::Defend(&p->getComponent<StatusComponent>())) { currentTurn++; }

							actionQueued = false;

							break;

						case SDLK_l:	//rest

							if (CombatFunctions::Rest(&p->getComponent<StatusComponent>())) { currentTurn++; }

							actionQueued = false;

							break;
						}
					}
				}

				else { currentTurn++; }
			}
		}
		/* This is the enemy combat loop. It loops over the enemy combatants, queries the decision making AI for each enemy type
		and does an action (attack, defend, or rest) based on the result of that */
		for (int i = 0; i < enemyCount; i++) {

			if (currentTurn == currentEnemies[i]->getComponent<StatusComponent>().combatIndex) {

				if (currentEnemies[i]->getComponent<StatusComponent>().isAlive) {

					switch (behaviour[i].DecisionMaker()) {

					case EnemyBehaviour::decision::ATK:

						switch (behaviour[i].TargetChooser()) {

						case EnemyBehaviour::target::strongest:

							if (CombatFunctions::EnemyAttack(&currentEnemies[i]->getComponent<StatusComponent>(), behaviour[i].GetStrongest(), lossCheck)) { currentTurn++; }

							break;

						case EnemyBehaviour::target::tankiest:

							if (CombatFunctions::EnemyAttack(&currentEnemies[i]->getComponent<StatusComponent>(), behaviour[i].GetTankiest(), lossCheck)) { currentTurn++; }

							break;

						case EnemyBehaviour::target::weakest:

							if (CombatFunctions::EnemyAttack(&currentEnemies[i]->getComponent<StatusComponent>(), behaviour[i].GetWeakest(), lossCheck)) { currentTurn++; }

							break;
						}

						break;

					case EnemyBehaviour::decision::DEF:

						if (CombatFunctions::Defend(&currentEnemies[i]->getComponent<StatusComponent>())) { currentTurn++; }

						break;

					case EnemyBehaviour::decision::REST:

						if (CombatFunctions::Rest(&currentEnemies[i]->getComponent<StatusComponent>())) { currentTurn++; }

						break;

					}
				}

				else { currentTurn++; }
			}
		}

		if (currentTurn == 4 + enemyCount) { currentTurn = 0; }

		if (winCheck && isBattleWon()) {

			_worldState = stateRealTime;
			CombatFunctions::ResetPlayerStatus();
			currentEnemies.clear();
			currentTurn = 0;
			stateInitialized = false;
		}

		winCheck = false;

		if (lossCheck && isBattleLost()) { 
			
			_worldState = stateGameOver;
		}

		lossCheck = false;

		break; //end of turn based game loop

	case stateGameOver:

		isRunning = false;
		break;

	}
}

//Window renderer
void Game::render() {

	SDL_RenderClear(renderer);
	
	switch (_worldState) {
		
	case stateRealTime:

			for (auto& t : tiles) { t->draw(); }
			for (auto& c : colliders) { c->draw(); }
			for (auto& p : players) { p->draw(); }

			label.draw();

		break;

	case stateTurnBased:

				SDL_RenderCopy(renderer, assets->GetTexture("Background"), NULL, NULL);
				for (auto& p : players) { p->draw(); }
				for (auto& e : currentEnemies) {

					if (e->getComponent<StatusComponent>().isAlive) { e->draw(); }
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