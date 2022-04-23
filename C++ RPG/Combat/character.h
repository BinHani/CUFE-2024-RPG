#ifndef _CHAR_H_
#define _CHAR_H_

#include  <iostream>
#include  <stdlib.h>
#include  <stdio.h>
#include  <string>

using std::string;

class character
{
public:
	string name;                      
	string type;
	uint16_t  HP;
	uint16_t  FP;
	uint16_t  DAMAGE;
	uint16_t  RESISTANCE;
	character(){};
	character(string kind, short hp, short fp, short damage, short resistance)
	{
		type = kind;
		HP = hp;
		FP = fp;
		DAMAGE = damage;
		RESISTANCE = resistance;
	}
};

character fighter("fighter", 50, 15, 30, 40);                        
character assassin("assassin", 30, 40, 50, 15);
character mage("mage", 35, 50, 40, 15);




#endif