#ifndef _AI_H_
#define _AI_H_

#include "combat.h"

class AI
{
private:
    /* data */
public:
    // utility functions to get the strongest (most attack power), tankiest (most HP), and weakest (least HP) characters to aid in the AI decision making
    uint8_t get_strongest(combat::tile[]); 
    uint8_t get_tankiest(combat::tile[]); 
    uint8_t get_weakest(combat::tile[]); 
    
    // AI subroutines for each enemy type
    uint8_t s_treeling(character, combat::tile[]);
    uint8_t l_treeling(character, combat::tile[]);
    uint8_t skeleton_head(character, combat::tile[]);
    uint8_t wolf(character, combat::tile[]);
    
    AI(uint8_t, character, combat::tile[]); // the master AI function
    ~AI();
};

uint8_t AI::get_strongest(combat::tile targets[])
{
}

uint8_t AI::get_tankiest(combat::tile targets[])
{
}

uint8_t AI::get_weakest(combat::tile targets[])
{
}

uint8_t AI::s_treeling(character actor, combat::tile targets[])
{
}

uint8_t AI::l_treeling(character actor, combat::tile targets[])
{
}

uint8_t AI::skeleton_head(character actor, combat::tile targets[])
{
}

uint8_t AI::wolf(character actor, combat::tile targets[])
{
}

AI::AI(uint8_t kind, character actor, combat::tile targets[])
{
    switch (kind)
    {
    case SMALL_TREELING:
        s_treeling(actor, targets);
        break;
    case LARGE_TREELING:
        l_treeling(actor, targets);
        break;
    case SKELETON_HEAD:
        skeleton_head(actor, targets);
        break;
    case WOLF:
        wolf(actor, targets);
        break;
    default:
        break;
    }
}

AI::~AI()
{
}


#endif