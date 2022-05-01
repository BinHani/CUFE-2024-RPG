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
	static void SortEnemiesByAP(Entity* enemies[], int _enemyCount, StatusComponent _enemyStatus[], EnemyBehaviour _behaviour[]);
	static SDL_Keycode GetAction(SDL_Event& _event);
	static SDL_Keycode GetTarget(SDL_Event& _event);
	static void Command();
	static bool Attack(Entity* attacker, StatusComponent defenderStatus[], SDL_Keycode& _enemySelector, int _enemyCount);
	static bool Defend(Entity* actor);
	static bool Rest(Entity* actor);
	static unsigned short isOver();
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

void CombatFunctions::SortEnemiesByAP(Entity* enemies[], int _enemyCount, StatusComponent _enemyStatus[], EnemyBehaviour _behaviour[]) {

	for (int i = 0; i < _enemyCount; i++) {
		for (int j = 0; j < _enemyCount - (i + 1); j++) {
			if (enemies[j]->getComponent<StatusComponent>().maxAP < enemies[j + 1]->getComponent<StatusComponent>().maxAP) {
				Entity* temp = enemies[j];
				enemies[j] = enemies[j + 1];
				enemies[j + 1] = enemies[j];
			}

		}
	}

	for (int i = 0; i < _enemyCount; i++) { _enemyStatus[i] = enemies[i]->getComponent<StatusComponent>(); }

	for (int i = 0; i < enemyCount; i++) { _behaviour[i].setStatus(&_enemyStatus[i]); }
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

void CombatFunctions::Command() {

	switch (Game::event.key.keysym.sym) {

	case SDLK_j:
		//Attack();
		break;

	case SDLK_k:
		//Defend();
		break;

	case SDLK_l:
		//Rest();
		break;
	}
}

bool CombatFunctions::Attack(Entity* attacker, StatusComponent defenderStatus[], SDL_Keycode& _enemySelector, int _enemyCount) {

	if (attacker->getComponent<StatusComponent>().currentAP >= attackCost) {

		switch (_enemySelector) {
	
		case SDLK_1:

			if (defenderStatus[0].isAlive) { 
			
			defenderStatus[0].currentHP -= attacker->getComponent<StatusComponent>().damage;
			std::cout << "Enemy 1's HP: "<< defenderStatus[0].currentHP << std::endl;

			attacker->getComponent<StatusComponent>().currentAP -= attackCost;
			std::cout << "Current AP: " << attacker->getComponent<StatusComponent>().currentAP << std::endl;

			if (defenderStatus[0].currentHP < 0) { defenderStatus[0].currentHP = 0; }

			return true;
			}

			std::cout << "Invalid Target!\n";
			return false;

		case SDLK_2:

			if (_enemyCount >= 2) {

				if (defenderStatus[1].isAlive) {

					defenderStatus[1].currentHP -= attacker->getComponent<StatusComponent>().damage;
					std::cout << "Enemy 2's HP: " << defenderStatus[1].currentHP << std::endl;

					attacker->getComponent<StatusComponent>().currentAP -= attackCost;
					std::cout << "Current AP: " << attacker->getComponent<StatusComponent>().currentAP << std::endl;

					if (defenderStatus[1].currentHP < 0) { defenderStatus[1].currentHP = 0; }

					return true;
				}

			}

			std::cout << "Invalid Target!\n";
			return false;

		case SDLK_3:

			if (_enemyCount >= 3) {

				if (defenderStatus[2].isAlive) {

					defenderStatus[2].currentHP -= attacker->getComponent<StatusComponent>().damage;
					std::cout << "Enemy 3's HP: " << defenderStatus[2].currentHP << std::endl;

					attacker->getComponent<StatusComponent>().currentAP -= attackCost;
					std::cout << "Current AP: " << attacker->getComponent<StatusComponent>().currentAP << std::endl;

					if (defenderStatus[2].currentHP < 0) { defenderStatus[2].currentHP = 0; }

					return true;
				}

			}

			std::cout << "Invalid Target!\n";
			return false;

		case SDLK_4:

			if (_enemyCount == 4) {

				if (defenderStatus[3].isAlive) {

					defenderStatus[3].currentHP -= attacker->getComponent<StatusComponent>().damage;
					std::cout << "Enemy 4's HP: " << defenderStatus[2].currentHP << std::endl;

					attacker->getComponent<StatusComponent>().currentAP -= attackCost;
					std::cout << "Current AP: " << attacker->getComponent<StatusComponent>().currentAP << std::endl;

					if (defenderStatus[3].currentHP < 0) { defenderStatus[3].currentHP = 0; }

					return true;
				}

			}

			std::cout << "Invalid Target!\n";
			return false;
		}

		std::cout << "Not Enough AP!\n";
		return false;

	}

}

bool CombatFunctions::Defend(Entity* actor) {

	if (actor->getComponent<StatusComponent>().currentAP >= defendCost) {

		actor->getComponent<StatusComponent>().isDefending = true;

		std::cout << "Actor is defending!\n";
		return true;

	}

	std::cout << "Not enough AP!\n";
	return false;

}

bool CombatFunctions::Rest(Entity* actor) {

	if (actor->getComponent<StatusComponent>().currentAP != actor->getComponent<StatusComponent>().maxAP) {

		actor->getComponent<StatusComponent>().currentAP += restRefund;

		std::cout << "AP partially restored!\n";
		return true;

	}

	std::cout << "AP is already full!\n";
	return false;

}

unsigned short isOver() {

	return 0;
}