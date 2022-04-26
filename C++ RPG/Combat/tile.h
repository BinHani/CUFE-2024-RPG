#pragma once
#include "character.h"

struct tile // fundamental combat unit
{
	character combatant; // character standing on the tile
	bool is_enemy; // tile allegiance (i.e., 0 for friendly or 1 for enemy)
	bool* alive; // tile state (i.e., 0 for dead or 1 for alive)
	uint8_t atk_pwr; // tile attack power
	uint8_t def; // tile defense rating
	tile() {};
};