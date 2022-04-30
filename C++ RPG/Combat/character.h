#ifndef _CHAR_H_
#define _CHAR_H_

#include  <iostream>
#include  <stdlib.h>
#include  <stdio.h>
#include  <string>

// may be better to change these to an enum
/*#define WARRIOR 10
#define MAGE 20
#define CLERIC 30
#define SMALL_TREELING 40
#define LARGE_TREELING 50
#define SKELETON_HEAD 60
#define WOLF 70
*/

enum types { WARRIOR, MAGE, CLERIC, SMALL_TREELING, LARGE_TREELING, SKELETON_HEAD, WOLF };

using std::string;

class character
{
public:
	string name;
	uint8_t type;
	short  HP;
	uint16_t  AP;
	uint16_t  DAMAGE;
	uint16_t  RESISTANCE;
	character() {};
	character(uint8_t kind, short hp, short AP, short damage, short resistance)
	{
		type = kind;
		HP = hp;
		AP = AP;
		DAMAGE = damage;
		RESISTANCE = resistance;
	}
};

/*character fighter("fighter", 50, 15, 30, 40);
character assassin("assassin", 30, 40, 50, 15);
character mage("mage", 35, 50, 40, 15);*/




#endif