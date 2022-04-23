#ifndef _COMBAT_H_
#define _COMBAT_H_

#include  <string.h>
#include  <iostream>
#include  <stdlib.h>
#include  <stdio.h>
#include "character.h"

#define N 	  6 // max no. of combatants
#define ATK   1
#define DEF   2
#define REST  3

class combat
{
private:
	uint8_t counter; // turn counter that indicates the combatant in this turn
	struct tile // fundamental combat unit
	{
		character combatant; // character standing on the tile
		bool is_enemy; // tile allegiance (i.e., 0 for friendly or 1 for enemy)
		bool alive; // tile state (i.e., 0 for dead or 1 for alive)
		uint8_t atk_pwr; // tile attack power
		uint8_t def; // tile defense rating
		tile(){};
	};
	tile combatants[N];
	tile friendly[N/2];
	tile enemy[N/2];
public:
	combat(character, character, character, character, character, character); // combat init function; heart of the combat loop
	~combat(); // combat end function 
	void assemble(character, character, character, character, character, character); // utility function to assemble all combatants in an array
	void sort(tile[]); // utility function to sort all combatants in descending order according to stamina
	void death(tile, tile); // utility function handling character death
	void attack(tile, tile, short); // function handling attacking
	void defend(tile); // function handling defense
	void rest(tile); // function to advance the turn and do nothing
};

void combat::assemble(character first, character second, character third, character fourth, character fifth, character sixth)
{
	// this smells, definitely needs changing. possibly replace by a constructor for tile?
	tile characters[N];
	characters[0].combatant = first; characters[0].alive = 1; characters[0].is_enemy = 0; characters[0].atk_pwr = 1; characters[0].def = 1;
	characters[1].combatant = second; characters[1].alive = 1; characters[1].is_enemy = 0; characters[1].atk_pwr = 1; characters[1].def = 1;
	characters[2].combatant = third; characters[2].alive = 1; characters[2].is_enemy = 0; characters[2].atk_pwr = 1; characters[2].def = 1;
	characters[3].combatant = fourth; characters[3].alive = 1; characters[3].is_enemy = 1; characters[3].atk_pwr = 1; characters[3].def = 1;
	characters[4].combatant = fifth; characters[4].alive = 1; characters[4].is_enemy = 1; characters[4].atk_pwr = 1; characters[4].def = 1;
	characters[5].combatant = sixth; characters[5].alive = 1; characters[5].is_enemy = 1; characters[5].atk_pwr = 1; characters[5].def = 1;

	for (size_t i = 0; i < N; i++)
	{
		combatants[i] = characters[i];
	}
	
	for (size_t i = 0; i < N/2; i++)
	{
		friendly[i] = characters[i];
	}
	
	for (size_t i = 0; i < N/2; i++)
	{
		enemy[i] = characters[i+3];
	}
	
}

void combat::sort(tile characters[])
{
	for (size_t i = 0; i < N; i++)
	{
		if (characters[i].combatant.FP <= characters[i+1].combatant.FP)
		{
			tile temp = characters[i+1];
			characters[i+1] = characters[i];
			characters[i] = temp;
		}
		
	}
	
}

void combat::death(tile attacker, tile target)
{
	if (target.combatant.HP <=0)
	{
		target.alive = 0;
		std::cout << target.combatant.name << " was killed by " << attacker.combatant.name << std::endl;
	}
}

void combat::attack(tile attacker, tile target, short attack_type)
{
	// float hit_percentage = 0.5; + switch statement for attack_type modifiers
	target.combatant.HP -= (attacker.atk_pwr * attacker.combatant.DAMAGE - target.def);
	std::cout << attacker.combatant.name << " hit " << target.combatant.name << " for " << (attacker.atk_pwr * attacker.combatant.DAMAGE - target.def) << "HP\n";
	death(attacker, target);
	counter++;
}

void combat::defend(tile current)
{
	current.def *= 1.2;
	std::cout << current.combatant.name << " defended\n" << current.combatant.name << "'s current HP: " << current.combatant.HP << " | " << current.combatant.name << "'s current FP: " << current.combatant.FP << "\n";
	counter++;
}

void combat::rest(tile current)
{
	std::cout << current.combatant.name << " did nothing\n" << current.combatant.name << "'s current HP: " << current.combatant.HP << " | " << current.combatant.name << "'s current FP: " << current.combatant.FP << "\n";
	counter++;
}

combat::combat(character first, character second, character third, character fourth, character fifth, character sixth) 
{
	assemble(first, second, third, fourth, fifth, sixth);
	sort(combatants);
	counter = 0;
	std::srand(static_cast<unsigned int>(std::time(NULL)));
	while (1)
	{
		short fselection = 0;
		short eselection = 0;
		if (combatants[counter].is_enemy == 0)
		{
			fselection = rand() % 3 + 1;
			switch (fselection)
			{
			case 1:
				attack(combatants[counter], enemy[rand() % 3 + 1] , 1);
				break;
			case 2:
				defend(combatants[counter]);
				break;
			case 3:
				rest(combatants[counter]);
				break;
			default:
				break;
			}
		}
		else
		{
			eselection = rand() % 3 + 1;
			switch (eselection)
			{
			case 1:
				attack(combatants[counter], friendly[rand() % 3 + 1] , 1);
				break;
			case 2:
				defend(combatants[counter]);
				break;
			case 3:
				rest(combatants[counter]);
				break;
			default:
				break;
			}
		}
		counter %= N;
	}
	
}

combat::~combat()
{
}


#endif