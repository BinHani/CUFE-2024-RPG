#pragma once
#include "ECS\ECS.h"
#include "SDL.h"
#include "Game.h"
#include "ECS\Components.h"
#include "EnemyBehaviour.h"
#include <iostream>
#include <random>
#include <vector>

const uint8_t attackCost = 3;
const uint8_t defendCost = 2;
const uint8_t restRefund = 5;

extern Manager manager;

struct CombatFunctions {
	static int GenerateRandomEncounter();
	static void InitializeEnemies(int _enemyCount, EnemyBehaviour _behaviour[]);
	static void ResetPlayerStatus();
	static SDL_Keycode GetAction(SDL_Event& _event);
	static SDL_Keycode GetTarget(SDL_Event& _event);
	static bool PlayerAttack(StatusComponent* attacker, SDL_Keycode& _enemySelector, int _enemyCount, bool& _winCheck);
	static bool EnemyAttack(StatusComponent* attacker, StatusComponent* defender, bool& _lossCheck);
	static bool Defend(StatusComponent* actor);
	static bool Rest(StatusComponent* actor);
	static bool isBattleWon();
	static bool isBattleLost();

	static void createEnemy(Entity* enemyType) {

		auto& _currentEnemy(manager.addEntity());

		TransformComponent* transform = &enemyType->getComponent<TransformComponent>();
		StatusComponent* status = &enemyType->getComponent<StatusComponent>();
		SpriteComponent* sprite = &enemyType->getComponent<SpriteComponent>();
		
		_currentEnemy.addComponent<TransformComponent>(transform->position.x, transform->position.y, transform->height,transform->width,transform->scale);
		_currentEnemy.addComponent<StatusComponent>(status->maxHP, status->maxAP, status->damage, status->baseResistance);
		_currentEnemy.getComponent<StatusComponent>().SetDecisionCoeffs(status->attackCoeff, status->defendCoeff, status->restCoeff, status->irrationalCoeff);
		_currentEnemy.getComponent<StatusComponent>().SetTargetWeights(status->strongestWeight, status->tankiestWeight, status->weakestWeight, status->randomWeight);
		_currentEnemy.addComponent<SpriteComponent>(sprite->id, sprite->animated);
		_currentEnemy.getComponent<SpriteComponent>().battleIndex = sprite->battleIndex;
		_currentEnemy.addGroup(Game::groupCurrentEnemies);

	}

};

//Randomly generate encounter from pool of enemies
int CombatFunctions::GenerateRandomEncounter() {

	int enemyCount;
	int enemyTypeCount = manager.getGroup(Game::groupEnemyCharacters).size();
	int random;

	std::uniform_real_distribution<> enemyCountRandomizer(1, 5);
	std::uniform_real_distribution<> enemyTypeRandomizer(1, enemyTypeCount + 1);

	std::random_device rd;
	std::seed_seq sd{ rd(), rd(), rd(), rd(), rd(), rd(), rd(), rd() };

	std::mt19937 RNG(sd);

	double r = enemyCountRandomizer(RNG);

	if (r >= 1 && r < 2) { enemyCount = 1; }
	else if (r >= 2 && r < 3) { enemyCount = 2; }
	else if (r >= 3 && r < 4) { enemyCount = 3; }
	else if (r >= 4 && r <= 5) { enemyCount = 4; }

	for (int i = 0; i < enemyCount; i++) {

		r = enemyTypeRandomizer(RNG);

		for (int j = 1; j < enemyTypeCount + 1; j++) {

			if (r >= j && r <= j + 1) { createEnemy(manager.getGroup(Game::groupEnemyCharacters)[j - 1]); }
		}

		std::cout << "Battle Index: " << manager.getGroup(Game::groupCurrentEnemies)[i]->getComponent<SpriteComponent>().battleIndex << std::endl;
	}
	std::cout << std::endl;

	return enemyCount;
}

void CombatFunctions::InitializeEnemies(int _enemyCount, EnemyBehaviour _behaviour[]) {
/*
	for (int i = 0; i < _enemyCount; i++) {

		for (int j = 0; j < _enemyCount - (i + 1); j++) {

			if (enemies[j]->getComponent<StatusComponent>().maxAP < enemies[j + 1]->getComponent<StatusComponent>().maxAP) {
				
				Entity temp = *enemies[j];
				*enemies[j] = *enemies[j + 1];
				*enemies[j + 1] = temp;
			}
		}
	}
*/

	int spoopyCounter = 0;
	bool isSpoopy[4] = {0, 0, 0, 0};
	int a = 0;

	for (int i = 0; i < _enemyCount; i++) {
		if (manager.getGroup(Game::groupCurrentEnemies)[i]->getComponent<SpriteComponent>().battleIndex == 4) {
			spoopyCounter++;
			isSpoopy[i] = true;
		}
	}



	for (int i = 0; i < _enemyCount; i++) {
		manager.getGroup(Game::groupCurrentEnemies)[i]->getComponent<StatusComponent>().combatIndex = i + 4;
		_behaviour[i].setStatus(&manager.getGroup(Game::groupCurrentEnemies)[i]->getComponent<StatusComponent>());
	}

		for (int i = 0; i < _enemyCount; i++) {
			
			for (int j = 0; j < i; j++) {
				if (isSpoopy[j]) a++;
			}

			if (!isSpoopy[i]) {

				manager.getGroup(Game::groupCurrentEnemies)[i]->getComponent<SpriteComponent>().destRect.x = (640 / (_enemyCount - spoopyCounter)) * (2 * (i - a) + 1)
					- manager.getGroup(Game::groupCurrentEnemies)[i]->getComponent<TransformComponent>().width * manager.getGroup(Game::groupCurrentEnemies)[i]->getComponent<TransformComponent>().scale / 2;
			}

			else manager.getGroup(Game::groupCurrentEnemies)[i]->getComponent<SpriteComponent>().destRect.x = (415 / spoopyCounter) * (2 * a + 1)
				- manager.getGroup(Game::groupCurrentEnemies)[i]->getComponent<TransformComponent>().width * manager.getGroup(Game::groupCurrentEnemies)[i]->getComponent<TransformComponent>().scale / 2;

			a = 0;
		}
}

void CombatFunctions::ResetPlayerStatus() {

	for (auto& p : manager.getGroup(Game::groupPlayerCharacters)) {

		p->getComponent<StatusComponent>().currentHP = p->getComponent<StatusComponent>().maxHP;
		p->getComponent<StatusComponent>().currentAP = p->getComponent<StatusComponent>().maxAP;
		p->getComponent<StatusComponent>().currentResistance = p->getComponent<StatusComponent>().baseResistance;
		p->getComponent<StatusComponent>().isAlive = true;
		p->getComponent<StatusComponent>().isDefending = false;
		p->getComponent<SpriteComponent>().Play("Idle");
		p->getComponent<SpriteComponent>().attack = false;
		p->getComponent<TransformComponent>().velocity.Zero();
	}
}

SDL_Keycode CombatFunctions::GetAction(SDL_Event& _event) {
	if (_event.key.keysym.sym == SDLK_j || _event.key.keysym.sym == SDLK_k || _event.key.keysym.sym == SDLK_l) { 
		
		return _event.key.keysym.sym;
	}

	else return SDLK_UNKNOWN;
}

SDL_Keycode CombatFunctions::GetTarget(SDL_Event& _event) {
	if (_event.key.keysym.sym == SDLK_1 || _event.key.keysym.sym == SDLK_2 || 
		_event.key.keysym.sym == SDLK_3 || _event.key.keysym.sym == SDLK_4) { 
		
		return _event.key.keysym.sym; 
	}

	else return SDLK_UNKNOWN;
}

bool CombatFunctions::PlayerAttack(StatusComponent* attacker, SDL_Keycode& _enemySelector, int _enemyCount, bool& _winCheck) {

	if (attacker->currentAP >= attackCost) {

		switch (_enemySelector) {
	
		case SDLK_1:

			if (manager.getGroup(Game::groupCurrentEnemies)[0]->getComponent<StatusComponent>().isAlive) {
			
				manager.getGroup(Game::groupCurrentEnemies)[0]->getComponent<StatusComponent>().currentHP -= attacker->damage;

			if (manager.getGroup(Game::groupCurrentEnemies)[0]->getComponent<StatusComponent>().currentHP <= 0) {

				manager.getGroup(Game::groupCurrentEnemies)[0]->getComponent<StatusComponent>().isAlive = false;
				manager.getGroup(Game::groupCurrentEnemies)[0]->getComponent<StatusComponent>().currentHP = 0;
				_winCheck = true;
			}

			attacker->currentAP -= attackCost;

			std::cout << "Enemy 1's HP: "<< manager.getGroup(Game::groupCurrentEnemies)[0]->getComponent<StatusComponent>().currentHP << std::endl;
			std::cout << "Current AP: " << attacker->currentAP << std::endl << std::endl;

			return true;

			}

			std::cout << "Invalid Target!" << std::endl << std::endl;
			return false;

		case SDLK_2:

			if (_enemyCount >= 2) {

				if (manager.getGroup(Game::groupCurrentEnemies)[1]->getComponent<StatusComponent>().isAlive) {

					manager.getGroup(Game::groupCurrentEnemies)[1]->getComponent<StatusComponent>().currentHP -= attacker->damage;

					if (manager.getGroup(Game::groupCurrentEnemies)[1]->getComponent<StatusComponent>().currentHP <= 0) {

						manager.getGroup(Game::groupCurrentEnemies)[1]->getComponent<StatusComponent>().isAlive = false;
						manager.getGroup(Game::groupCurrentEnemies)[1]->getComponent<StatusComponent>().currentHP = 0;
						_winCheck = true;
					}

					attacker->currentAP -= attackCost;

					std::cout << "Enemy 2's HP: " << manager.getGroup(Game::groupCurrentEnemies)[1]->getComponent<StatusComponent>().currentHP << std::endl;
					std::cout << "Current AP: " << attacker->currentAP << std::endl << std::endl;

					return true;
				}

			}

			std::cout << "Invalid Target!" << std::endl << std::endl;
			return false;

		case SDLK_3:

			if (_enemyCount >= 3) {

				if (manager.getGroup(Game::groupCurrentEnemies)[2]->getComponent<StatusComponent>().isAlive) {

					manager.getGroup(Game::groupCurrentEnemies)[2]->getComponent<StatusComponent>().currentHP -= attacker->damage;

					if (manager.getGroup(Game::groupCurrentEnemies)[2]->getComponent<StatusComponent>().currentHP <= 0) {

						manager.getGroup(Game::groupCurrentEnemies)[2]->getComponent<StatusComponent>().isAlive = false;
						manager.getGroup(Game::groupCurrentEnemies)[2]->getComponent<StatusComponent>().currentHP = 0;
						_winCheck = true;
					}

					attacker->currentAP -= attackCost;

					std::cout << "Enemy 3's HP: " << manager.getGroup(Game::groupCurrentEnemies)[2]->getComponent<StatusComponent>().currentHP << std::endl;
					std::cout << "Current AP: " << attacker->currentAP << std::endl << std::endl;

					return true;
				}

			}

			std::cout << "Invalid Target!" << std::endl << std::endl;
			return false;

		case SDLK_4:

			if (_enemyCount == 4) {

				if (manager.getGroup(Game::groupCurrentEnemies)[3]->getComponent<StatusComponent>().isAlive) {

					manager.getGroup(Game::groupCurrentEnemies)[3]->getComponent<StatusComponent>().currentHP -= attacker->damage;

					if (manager.getGroup(Game::groupCurrentEnemies)[3]->getComponent<StatusComponent>().currentHP <= 0) {

						manager.getGroup(Game::groupCurrentEnemies)[3]->getComponent<StatusComponent>().isAlive = false;
						manager.getGroup(Game::groupCurrentEnemies)[3]->getComponent<StatusComponent>().currentHP = 0;
						_winCheck = true;
					}

					attacker->currentAP -= attackCost;

					std::cout << "Enemy 4's HP: " << manager.getGroup(Game::groupCurrentEnemies)[3]->getComponent<StatusComponent>().currentHP << std::endl ;
					std::cout << "Current AP: " << attacker->currentAP << std::endl << std::endl;

					return true;
				}

			}

			std::cout << "Invalid Target!" << std::endl << std::endl;
			return false;
		}

	}

	std::cout << "Not Enough AP!" << std::endl << std::endl;
	return false;

}

bool CombatFunctions::EnemyAttack(StatusComponent* attacker, StatusComponent* defender, bool& _lossCheck) {

	if (attacker->currentAP > attackCost) {

		defender->currentHP -= attacker->damage;
		attacker->currentAP -= attackCost;

		if (defender->currentHP <= 0) {

			defender->isAlive = false;
			defender->currentHP = 0;
			_lossCheck = true;
		}

		std::cout << "Enemy Attacked!" << std::endl << std::endl;

		return true;
	}

	std::cout << "Attack attempt failed!" << std::endl << std::endl;

	return false;

}

bool CombatFunctions::Defend(StatusComponent* actor) {

	if (actor->currentAP >= defendCost) {

		actor->isDefending = true;

		std::cout << "Actor is defending!\n";
		return true;

	}

	std::cout << "Not enough AP!\n";
	return false;

}

bool CombatFunctions::Rest(StatusComponent* actor) {

	if (actor->currentAP != actor->maxAP) {

		actor->currentAP += restRefund;

		if (actor->currentAP >= actor->maxAP) {

			actor->currentAP = actor->maxAP;
			std::cout << "AP fully restored!" << std::endl << std::endl;
		}

		else { std::cout << "AP partially restored!" << std::endl << std::endl; }

		return true;

	}

	std::cout << "AP is already full!" << std::endl << std::endl;
	return false;

}

bool isBattleWon() {

	bool anyEnemyAlive = false;

	for (auto& e : manager.getGroup(Game::groupCurrentEnemies)) { 
		
		if (e->getComponent<StatusComponent>().isAlive) { anyEnemyAlive = true; break; } 
	}

	return !anyEnemyAlive;
}

bool isBattleLost() {

	bool anyPlayerAlive = false;

	for (auto& p : manager.getGroup(Game::groupPlayerCharacters)) { if (p->getComponent<StatusComponent>().isAlive) { anyPlayerAlive = true; break; } }

	return !anyPlayerAlive;
}