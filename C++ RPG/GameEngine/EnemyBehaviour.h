#pragma once
#include "ECS\ECS.h"
#include "ECS\Components.h"
#include <random>

class EnemyBehaviour {

public:

	StatusComponent* status;
	float normalizedHP, normalizedAP;

	void init() {

		status = &entity->getComponent<StatusComponent>();

		attackWeight = 0;
		defendWeight = 0;
		restWeight = 0;
		irrationalWeight = 0;

		normalizedHP = status->maxHP;
		normalizedAP = status->maxAP;
	}

	void update() override {

		normalizedHP = static_cast<float>(status->currentHP) / status->maxHP;
		normalizedAP = static_cast<float>(status->currentAP) / status->maxAP;
	}



private:

	float attackWeight, defendWeight, restWeight, irrationalWeight;

	enum class decision { ATK, DEF, REST };

	Entity* get_strongest(Entity* playerCharacters[]) {

		Entity* strongest = playerCharacters[0];

		for (size_t i = 1; i < 4; i++) {

			if (playerCharacters[i]->getComponent<StatusComponent>().damage >= strongest->getComponent<StatusComponent>().damage) {

				strongest = playerCharacters[i];
			}

		}

		return strongest;

	}

	Entity* get_tankiest(Entity* playerCharacters[]) {

		Entity* tankiest = playerCharacters[0];

		for (size_t i = 1; i < 4; i++) {

			if (playerCharacters[i]->getComponent<StatusComponent>().currentHP >= tankiest->getComponent<StatusComponent>().currentHP) {

				tankiest = playerCharacters[i];
			}

		}

		return tankiest;

	}

	Entity* get_weakest(Entity* playerCharacters[]) {

		Entity* weakest = playerCharacters[0];

		for (size_t i = 1; i < 4; i++) {

			if (playerCharacters[i]->getComponent<StatusComponent>().currentHP <= weakest->getComponent<StatusComponent>().currentHP) {

				weakest = playerCharacters[i];
			}

		}

		return weakest;

	}

	decision AI(unsigned short a, unsigned short d, unsigned short r, unsigned short i) {

		ResetWeights();

		if (normalizedAP <= 0.15) { return decision::REST; }

		if (normalizedHP >= 0.5) {

			attackWeight += a * normalizedHP;
			defendWeight += d * (1 - normalizedHP);
			restWeight = r * (1 - normalizedAP);
			irrationalWeight += i * normalizedHP;

		}

		if (normalizedHP < 0.5) {

			attackWeight += a * normalizedHP;
			defendWeight += d * (1 - normalizedHP);
			restWeight = r * (1 - normalizedAP);
			irrationalWeight += i * (1 - normalizedHP);
		}

		return getDecision();

	}

	decision getDecision() {

		double interval[] = { 1, 2, 3, 4, 5 };
		double weights[] = { attackWeight, defendWeight, restWeight, irrationalWeight };
		std::piecewise_constant_distribution<> mainDist(std::begin(interval), std::end(interval), std::begin(weights));
		std::uniform_real_distribution<> subDist(1, 4);

		std::random_device rd;
		std::seed_seq sd{ rd(), rd(), rd(), rd(), rd(), rd(), rd(), rd() };

		std::mt19937 RNG(sd);

		double r = mainDist(RNG);

		if (r >= 1 && r < 2) return decision::ATK;

		if (r >= 2 && r < 3) return decision::DEF;

		if (r >= 3 && r < 4) return decision::REST;

		if (r >= 4 && r < 5) { // irrational behaviour

			r = subDist(RNG);

			if (r >= 1 && r < 2) return decision::ATK;

			if (r >= 2 && r < 3) return decision::DEF;

			if (r >= 3 && r < 4) return decision::REST;

		}
	}

	void ResetWeights() { attackWeight = defendWeight = restWeight = irrationalWeight = 0; }
};