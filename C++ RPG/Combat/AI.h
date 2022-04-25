#ifndef _AI_H_
#define _AI_H_

#include "combat.h"

class AI
{
private:
    /* data */
public:
    uint8_t get_strongest(combat::tile[]); // utility function to get the strongest (most attack power) character
    uint8_t get_tankiest(combat::tile[]); // utility function to get the tankiest (most HP) character
    uint8_t get_weakest(combat::tile[]); // utility function to get the weakest (least HP) character
    // AI subroutines for each enemy type
    uint8_t s_treeling_ai();
    uint8_t l_treeling_ai();
    uint8_t skeleton_head_ai();
    uint8_t wolf_ai();
    
    AI(uint8_t, character, combat::tile[]); // the master AI function
    ~AI();
};

AI::AI(uint8_t, character, combat::tile[])
{
}

AI::~AI()
{
}


#endif