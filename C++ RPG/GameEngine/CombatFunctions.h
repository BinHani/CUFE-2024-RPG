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

struct CombatFunctions {
	static int GenerateRandomEncounter(Manager* man, Entity* enemies[]);
	static void InitializeEnemies(Entity* enemies[], int _enemyCount, StatusComponent _enemyStatus[], EnemyBehaviour _behaviour[]);
	static void SetPlayerStatus(Manager* man, StatusComponent _playerStatus[]);
	static SDL_Keycode GetAction(SDL_Event& _event);
	static SDL_Keycode GetTarget(SDL_Event& _event);
	static bool PlayerAttack(StatusComponent* attacker, StatusComponent defenderStatus[], SDL_Keycode& _enemySelector, int _enemyCount, bool& _winCheck);
	static bool EnemyAttack(StatusComponent* attacker, StatusComponent* defender, bool& _lossCheck);
	static bool Defend(StatusComponent* actor);
	static bool Rest(StatusComponent* actor);
	static worldState isBattleWon(StatusComponent playerStatus[], StatusComponent enemyStatus[], int enemyCount);
	static bool isBattleLost(StatusComponent playerStatus[]);
};

//Randomly generate encounter from pool of enemies
int CombatFunctions::GenerateRandomEncounter(Manager* man, Entity* enemies[]) {

	int enemyCount;
	int enemyTypeCount = man->getGroup(Game::groupEnemyCharacters).size();
	int random;

	std::uniform_real_distribution<> enemyCountRandomizer(1, 5);
	std::uniform_real_distribution<> enemyTypeRandomizer(1, 1 + enemyTypeCount);

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
			if (r >= j && r <= j + 1) { random = j - 1; }
		}

		enemies[i] = man->getGroup(Game::groupEnemyCharacters)[random];
		std::cout << "enemy generated!" << std::endl;
	}

	return enemyCount;
}

void CombatFunctions::InitializeEnemies(Entity* enemies[], int _enemyCount, StatusComponent _enemyStatus[], EnemyBehaviour _behaviour[]) {

	for (int i = 0; i < _enemyCount; i++) {
		for (int j = 0; j < _enemyCount - (i + 1); j++) {
			if (enemies[j]->getComponent<StatusComponent>().maxAP < enemies[j + 1]->getComponent<StatusComponent>().maxAP) {
				Entity* temp = enemies[j];
				enemies[j] = enemies[j + 1];
				enemies[j + 1] = enemies[j];
			}
		}
	}

	for (int i = 0; i < _enemyCount; i++) { 
		_enemyStatus[i] = enemies[i]->getComponent<StatusComponent>();
		_enemyStatus[i].combatIndex = i + 4;
	}

	for (int i = 0; i < _enemyCount; i++) { _behaviour[i].setStatus(&_enemyStatus[i]); }
}

void CombatFunctions::SetPlayerStatus(Manager* man, StatusComponent _playerStatus[]) {

	for (int i = 0; i < 4; i++) {
		_playerStatus[i] = man->getGroup((Game::groupPlayerCharacters))[i]->getComponent<StatusComponent>();
		_playerStatus[i].combatIndex = i;
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

bool CombatFunctions::PlayerAttack(StatusComponent* attacker, StatusComponent defenderStatus[], SDL_Keycode& _enemySelector, int _enemyCount, bool& _winCheck) {

	if (attacker->currentAP >= attackCost) {

		switch (_enemySelector) {
	
		case SDLK_1:

			if (defenderStatus[0].isAlive) { 
			
			defenderStatus[0].currentHP -= attacker->damage;

			if (defenderStatus[0].currentHP <= 0) {

				defenderStatus[0].isAlive = false;
				defenderStatus[0].currentHP = 0;
				_winCheck = true;
			}

			attacker->currentAP -= attackCost;

			std::cout << "Enemy 1's HP: "<< defenderStatus[0].currentHP << std::endl;
			std::cout << "Current AP: " << attacker->currentAP << std::endl << std::endl;

			return true;

			}

			std::cout << "Invalid Target!" << std::endl << std::endl;
			return false;

		case SDLK_2:

			if (_enemyCount >= 2) {

				if (defenderStatus[1].isAlive) {

					defenderStatus[1].currentHP -= attacker->damage;

					if (defenderStatus[1].currentHP <= 0) {

						defenderStatus[1].isAlive = false;
						defenderStatus[1].currentHP = 0;
						_winCheck = true;
					}

					attacker->currentAP -= attackCost;

					std::cout << "Enemy 2's HP: " << defenderStatus[1].currentHP << std::endl;
					std::cout << "Current AP: " << attacker->currentAP << std::endl << std::endl;

					return true;
				}

			}

			std::cout << "Invalid Target!" << std::endl << std::endl;
			return false;

		case SDLK_3:

			if (_enemyCount >= 3) {

				if (defenderStatus[2].isAlive) {

					defenderStatus[2].currentHP -= attacker->damage;

					if (defenderStatus[2].currentHP <= 0) {

						defenderStatus[2].isAlive = false;
						defenderStatus[2].currentHP = 0;
						_winCheck = true;
					}

					attacker->currentAP -= attackCost;

					std::cout << "Enemy 3's HP: " << defenderStatus[2].currentHP << std::endl;
					std::cout << "Current AP: " << attacker->currentAP << std::endl << std::endl;

					return true;
				}

			}

			std::cout << "Invalid Target!" << std::endl << std::endl;
			return false;

		case SDLK_4:

			if (_enemyCount == 4) {

				if (defenderStatus[3].isAlive) {

					defenderStatus[3].currentHP -= attacker->damage;

					if (defenderStatus[3].currentHP <= 0) {

						defenderStatus[3].isAlive = false;
						defenderStatus[3].currentHP = 0;
						_winCheck = true;
					}

					attacker->currentAP -= attackCost;

					std::cout << "Enemy 4's HP: " << defenderStatus[2].currentHP << std::endl ;
					std::cout << "Current AP: " << attacker->currentAP << std::endl << std::endl;

					return true;
				}

			}

			std::cout << "Invalid Target!" << std::endl << std::endl;
			return false;
		}

		std::cout << "Not Enough AP!" << std::endl << std::endl;
		return false;

	}

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

bool isBattleWon(StatusComponent enemyStatus[], int enemyCount) {

	bool anyEnemyAlive = false;

	for (int i = 0; i < enemyCount; i++) { if (enemyStatus[i].isAlive) { anyEnemyAlive = true; break; } }

	return !anyEnemyAlive;
}

bool isBattleLost(StatusComponent playerStatus[]) {

	bool anyPlayerAlive = false;

	for (int i = 0; i < 4; i++) { if (playerStatus[i].isAlive) { anyPlayerAlive = true; break; } }

	return !anyPlayerAlive;
}