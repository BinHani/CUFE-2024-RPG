#ifndef _COMBAT_H_
#define _COMBAT_H_

#include  <string.h>
#include  <iostream>
#include  <stdlib.h>
#include  <stdio.h>
#include  <time.h>
#include "character.h"

#define N 	  8 // max no. of combatants

enum decision {ATK, DEF, REST};
enum attacks {REG, MOD, STR};

// TODO: Hit % / Evasion mechanic, AI integration, add comments, and general cleanliness

class combat
{
private:
	uint8_t counter; // turn counter that indicates the combatant in this turn
	struct tile // fundamental combat unit
	{
		character combatant; // character standing on the tile
		bool is_enemy; // tile allegiance (i.e., 0 for friendly or 1 for enemy)
		bool* alive; // tile state (i.e., 0 for dead or 1 for alive)
		uint8_t atk_pwr; // tile attack power
		uint8_t def; // tile defense rating
		tile(){};
	};
	tile combatants[N];
	friend class AI;
public:
	combat(character&, character&, character&, character&, character&, character&, character&, character&); // combat init function; heart of the combat loop
	~combat(); // combat end function 
	void assemble(character&, character&, character&, character&, character&, character&, character&, character&); // utility function to assemble all combatants in an array
	void sort(tile[]); // utility function to sort all combatants in descending order according to stamina
	void death(tile*, tile*); // utility function handling character death
	void next_turn(); // utility function to advance to the next turn
	void attack(tile*, tile*, uint8_t); // function handling attacking
	void defend(tile*); // function handling defense
	void rest(tile*); // function to advance the turn and recover AP
};

void combat::assemble(character& first, character& second, character& third, character& fourth, character& fifth, character& sixth, character& seventh, character& eighth)
{
	// this smells, definitely needs changing. possibly replace by a constructor for tile?
	tile characters[N];
	characters[0].combatant = first; characters[0].alive = new bool; *characters[0].alive = 1; characters[0].is_enemy = 0; characters[0].atk_pwr = 1; characters[0].def = 1;
	characters[1].combatant = second; characters[1].alive = new bool; *characters[1].alive = 1; characters[1].is_enemy = 0; characters[1].atk_pwr = 1; characters[1].def = 1;
	characters[2].combatant = third; characters[2].alive = new bool; *characters[2].alive = 1; characters[2].is_enemy = 0; characters[2].atk_pwr = 1; characters[2].def = 1;
	characters[3].combatant = fourth; characters[3].alive = new bool; *characters[3].alive = 1; characters[3].is_enemy = 0; characters[3].atk_pwr = 1; characters[3].def = 1;
	characters[4].combatant = fifth; characters[4].alive = new bool; *characters[4].alive = 1; characters[4].is_enemy = 1; characters[4].atk_pwr = 1; characters[4].def = 1;
	characters[5].combatant = sixth; characters[5].alive = new bool; *characters[5].alive = 1; characters[5].is_enemy = 1; characters[5].atk_pwr = 1; characters[5].def = 1;
	characters[6].combatant = seventh; characters[6].alive = new bool; *characters[6].alive = 1; characters[6].is_enemy = 1; characters[6].atk_pwr = 1; characters[6].def = 1;
	characters[7].combatant = eighth; characters[7].alive = new bool; *characters[7].alive = 1; characters[7].is_enemy = 1; characters[7].atk_pwr = 1; characters[7].def = 1;

	for (size_t i = 0; i < N; i++)
	{
		combatants[i] = characters[i];
	}
}

void combat::sort(tile characters[]) // sorting implemented using the selection sort algorithm as the sample size is low
{
	uint8_t i, j, min;
	tile temp;
	for (i = 0; i < N - 1; i++)
	{
		min = i;
		for (j = i + 1; j < N; j++)
		{
			if (characters[j].combatant.AP <= characters[min].combatant.AP)
			{
				min = j;
			}
			temp = characters[i];
			characters[i] = characters[min];
			characters[min] = temp;
		}
	}
	
}

void combat::death(tile* attacker, tile* target)
{
	if (target->combatant.HP <=0)
	{
		*target->alive = 0;
		std::cout << target->combatant.name << " was killed by " << attacker->combatant.name << std::endl;
	}
}

void combat::next_turn()
{
	do
	{
		counter++;
		counter %= N;
	} while (!*combatants[counter].alive);
}

void combat::attack(tile* attacker, tile* target, uint8_t attack_type)
{
	if (attacker->alive && target->alive)
	{
		float attack_modifier;
		switch (attack_type)
		{
		case REG:
			attack_modifier = 1;
			attacker->combatant.AP -= 2;
			target->combatant.HP -= (attack_modifier * attacker->atk_pwr * attacker->combatant.DAMAGE);
			std::cout << attacker->combatant.name << " attacked " << target->combatant.name << " for " << (attack_modifier * attacker->atk_pwr * attacker->combatant.DAMAGE) << "HP\n";
			std::cout << target->combatant.name << "'s current HP: " << target->combatant.HP << "\n";
			death(attacker, target);
			next_turn();
			break;
		case MOD:
			attack_modifier = 1.2;
			attacker->combatant.AP -= 4;
			target->combatant.HP -= (attack_modifier * attacker->atk_pwr * attacker->combatant.DAMAGE);
			std::cout << attacker->combatant.name << " moderately attacked " << target->combatant.name << " for " << (attack_modifier * attacker->atk_pwr * attacker->combatant.DAMAGE) << "HP\n";
			std::cout << target->combatant.name << "'s current HP: " << target->combatant.HP << "\n";
			death(attacker, target);
			next_turn();
			break;
		case STR:
			attack_modifier = 1.5;
			attacker->combatant.AP -= 6;
			target->combatant.HP -= (attack_modifier * attacker->atk_pwr * attacker->combatant.DAMAGE);
			std::cout << attacker->combatant.name << " strongly attacked " << target->combatant.name << " for " << (attack_modifier * attacker->atk_pwr * attacker->combatant.DAMAGE) << "HP\n";
			std::cout << target->combatant.name << "'s current HP: " << target->combatant.HP << "\n";
			death(attacker, target);
			next_turn();
			break;
		default:
			break;
		}
		// float hit_percentage = 0.5; 
	}
	else
	{
		next_turn();
		return;
	}
}

void combat::defend(tile* current)
{
	if (*current->alive)
	{
		current->combatant.AP -= 1;
		current->def *= 1.2;
		std::cout << current->combatant.name << " defended\n" << current->combatant.name << "'s current HP: " << current->combatant.HP << " | " << current->combatant.name << "'s current AP: " << current->combatant.AP << "\n";
		next_turn();
	}
	else
	{
		next_turn();
		return;
	}
}

void combat::rest(tile* current)
{
	if (*current->alive)
	{
		current->combatant.AP += 1;
		std::cout << current->combatant.name << " rested\n" << current->combatant.name << "'s current HP: " << current->combatant.HP << " | " << current->combatant.name << "'s current AP: " << current->combatant.AP << "\n";
		next_turn();
	}
	else
	{
		next_turn();
		return;
	}
}

combat::combat(character& first, character& second, character& third, character& fourth, character& fifth, character& sixth, character& seventh, character& eigth) 
{
	assemble(first, second, third, fourth, fifth, sixth, seventh, eigth);
	sort(combatants);
	counter = 0;
	std::srand(static_cast<unsigned int>(time(NULL)));
	short fdeath = 0;
	short edeath = 0;
	while (1)
	{
		uint8_t fselection = 0;
		uint8_t eselection = 0;
		uint8_t target = 0;
		uint8_t target2 = 0;
		uint8_t a_type;
		if (!combatants[counter].is_enemy)
		{
			fselection = rand() % 3;
			switch (fselection)
			{
			case ATK:
				target = rand() % N;
				a_type = rand() % 3;
				while (!*combatants[target].alive || !combatants[target].is_enemy)
				{
					target = rand() % N;
				}
				attack(&combatants[counter], &combatants[target], a_type);
				if (!*combatants[target].alive)
				{
					edeath++;
				}
				break;
			case DEF:
				defend(&combatants[counter]);
				break;
			case REST:
				rest(&combatants[counter]);
				break;
			default:
				break;
			}
		}
		else
		{
			eselection = rand() % 3;
			switch (eselection)
			{
			case ATK:
				target2 = rand() % N;
				while (!*combatants[target2].alive || combatants[target2].is_enemy)
				{
					target2 = rand() % N;
				}
				attack(&combatants[counter], &combatants[target2], 1);
				if (!*combatants[target2].alive)
				{
					fdeath++;
				}
				break;
			case DEF:
				defend(&combatants[counter]);
				break;
			case REST:
				rest(&combatants[counter]);
				break;
			default:
				break;
			}
		}
		if (fdeath == N/2 || edeath == N/2)
		{
			std::cout << "Combat ends.\n";
			break;
		}
	}
}

combat::~combat()
{
}


#endif