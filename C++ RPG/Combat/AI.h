#ifndef _AI_H_
#define _AI_H_

#include "combat.h"

class AI
{
private:
    /* data */
public:
    // utility functions to get the strongest (most attack power), tankiest (most HP), and weakest (least HP) characters to aid in the AI decision making
    unsigned char get_strongest(combat::tile[]); 
    unsigned char get_tankiest(combat::tile[]); 
    unsigned char get_weakest(combat::tile[]); 

    // utility function for the AI to decide based on the weights
    unsigned char get_decision(unsigned char, unsigned char, unsigned char, unsigned char);
    
    // AI subroutines for each enemy type to determine their decision
    unsigned char s_treeling(character, combat::tile[]);
    unsigned char l_treeling(character, combat::tile[]);
    unsigned char skeleton_head(character, combat::tile[]);
    unsigned char golem(character, combat::tile[]);

    // AI subroutines for each enemy type to determine their attack target on attack
    unsigned char s_treeling_target(character, combat::tile[]);
    unsigned char l_treeling_target(character, combat::tile[]);
    unsigned char skeleton_head_target(character, combat::tile[]);
    unsigned char golem_target(character, combat::tile[]);

    // AI subroutines for each enemy type to determine their attack type on attack
    unsigned char s_treeling_atype(character, combat::tile[]);
    unsigned char l_treeling_atype(character, combat::tile[]);
    unsigned char skeleton_head_atype(character, combat::tile[]);
    unsigned char golem_atype(character, combat::tile[]);

    AI(unsigned char, character, combat::tile[]); // the master AI function, may be unecessary
    ~AI();
};

unsigned char AI::get_strongest(combat::tile targets[])
{
    unsigned char strongest = 0;
    for (size_t i = 1; i < N; i++)
    {
        if (targets[i].atk_pwr >= targets[strongest].atk_pwr && !targets[i].is_enemy)
        {
            strongest = i;
        }
        
    }
    return strongest;
}

unsigned char AI::get_tankiest(combat::tile targets[])
{
    unsigned char tankiest = 0;
    for (size_t i = 1; i < N; i++)
    {
        if (targets[i].combatant.HP >= targets[tankiest].combatant.HP && !targets[i].is_enemy)
        {
            tankiest = i;
        }
        
    }
    return tankiest;
}

unsigned char AI::get_weakest(combat::tile targets[])
{
    unsigned char weakest = 0;
    for (size_t i = 1; i < N; i++)
    {
        if (targets[i].combatant.HP <= targets[weakest].combatant.HP && !targets[i].is_enemy)
        {
            weakest = i;
        }
        
    }
    return weakest;
}

unsigned char AI::get_decision(unsigned char a_weight, unsigned char d_weight, unsigned char r_weight, unsigned char irr_weight)
{
    if (a_weight > d_weight && a_weight > r_weight && a_weight > irr_weight)
    {
        return ATK;
    }

    else if (d_weight > a_weight && d_weight > r_weight && d_weight > irr_weight)
    {
        return DEF;
    }
    
    else if (r_weight > a_weight && r_weight > d_weight && r_weight > irr_weight)
    {
        return REST;
    }
    
    else
    {
        return (rand() % 3);
    }
}
/* So, the basic idea behind the AI decision making is that we have weights for atack, defend, rest, and irrational behavior. These weights are influenced by several
 factors, e.g., the enemy's stats and sometimes even the friendly's stats. The decision subroutine calculates the values of all the weights, and then at the end
 does a comparison, wherein the weight with the largest value leads to the decision. */

 // NOTE: There's stuff that's half assed (e.g., missing MAX_HP / MAX_AP) but that's because it depends on how the ECS works, so I left it until we work on the merge.
// NOTE2: All the multiplicative factors are not set in stone
unsigned char AI::s_treeling(character actor, combat::tile targets[])
{
    unsigned char a_weight = 0; // attack decision weight
    unsigned char d_weight = 0; // defend decision weight
    unsigned char r_weight = 0; // rest decision weight
    unsigned char irr_weight = 0; // irrationality weight
    unsigned char weakest = get_weakest(targets);
    unsigned char strongest = get_strongest(targets);
    if (actor.AP <= 2)
    {
        return REST;
    }
    if (actor.HP >= 0.65 || targets[weakest].combatant.HP < 0.35) // greater than 65% of MAX_HP or weakest friendly is at less than 35% of MAX_HP
    {
        a_weight += 0.05 * actor.HP; 
        d_weight += 0.03 * (1 - actor.HP); // factor of the remaining part of MAX_HP
        r_weight += 0.01 * (1 - actor.AP); // factor of the remaining part of MAX_AP
        irr_weight += 0.01 * actor.HP;
    }
    if (actor.HP >= 0.35 || targets[strongest].combatant.DAMAGE >= 0.5) // greater than 35% of MAX_HP, or strongest friendly can do more damage than 50% of HP, (may be better as an else if? not sure)
    {
        a_weight += 0.02 * actor.HP;
        d_weight += 0.06 * (1 - actor.HP);
        r_weight += 0.03 * (1 - actor.AP);
        irr_weight += 0.04 * (1 - actor.HP); 
    }
    if (actor.HP < 0.35) // less than 35% of MAX_HP
    {
        a_weight += 0.01 * actor.HP;
        d_weight += 0.09 * (1 - actor.HP);
        r_weight += 0.03 * (1 - actor.AP);
        irr_weight += 0.06 * (1 - actor.HP); 
    }
    get_decision(a_weight, d_weight, r_weight, irr_weight);
}

// large treeling is largely the same as small treeling, except that it is more aggressive overall and also it prioritizes strongest / tankiest friendlies, while the small treeling prioritizes the weakest
unsigned char AI::l_treeling(character actor, combat::tile targets[])
{
    unsigned char a_weight = 0; // attack decision weight
    unsigned char d_weight = 0; // defend decision weight
    unsigned char r_weight = 0; // rest decision weight
    unsigned char irr_weight = 0; // irrationality weight
    unsigned char strongest = get_strongest(targets);
    if (actor.AP <= 2)
    {
        return REST;
    }
    if (actor.HP >= 0.65 || targets[strongest].combatant.HP < 0.35) // greater than 65% of MAX_HP or strongest friendly is at less than 35%
    {
        a_weight += 0.06 * actor.HP; // all the multiplicative factors are not set in stone
        d_weight += 0.02 * (1 - actor.HP); // factor of the remaining part of MAX_HP
        r_weight += 0.01 * (1 - actor.AP); // factor of the remaining part of MAX_AP
        irr_weight += 0.01 * actor.HP;
    }
    if (actor.HP >= 0.35) // greater than 35% of MAX_HP, (may be better as an else if? not sure)
    {
        a_weight += 0.03 * actor.HP;
        d_weight += 0.06 * (1 - actor.HP);
        r_weight += 0.03 * (1 - actor.AP);
        irr_weight += 0.04 * (1 - actor.HP); 
    }
    if (actor.HP < 0.35) // less than 35% of MAX_HP
    {
        a_weight += 0.02 * actor.HP;
        d_weight += 0.08 * (1 - actor.HP);
        r_weight += 0.03 * (1 - actor.AP);
        irr_weight += 0.08 * (1 - actor.HP); 
    }
    get_decision(a_weight, d_weight, r_weight, irr_weight);
}

unsigned char AI::skeleton_head(character actor, combat::tile targets[])
{
    unsigned char a_weight = 0; // attack decision weight
    unsigned char d_weight = 0; // defend decision weight
    unsigned char r_weight = 0; // rest decision weight
    unsigned char irr_weight = 0; // irrationality weight
    if (actor.AP <= 2)
    {
        return REST;
    }
    if (actor.HP >= 0.5) // greater than 50% of MAX_HP
    {
        a_weight += 0.07 * actor.HP; // all the multiplicative factors are not set in stone
        d_weight += 0.02 * (1 - actor.HP); // factor of the remaining part of MAX_HP
        r_weight += 0.01 * (1 - actor.AP); // factor of the remaining part of MAX_AP
        irr_weight += 0.01 * actor.HP;
    }
    if (actor.HP < 0.5) // less than 50% of MAX_HP, (currently better as an else, but leaving it as an if incase of further tweaks)
    {
        a_weight += 0.03 * actor.HP;
        d_weight += 0.05 * (1 - actor.HP);
        r_weight += 0.03 * (1 - actor.AP);
        irr_weight += 0.04 * (1 - actor.HP); 
    }
    get_decision(a_weight, d_weight, r_weight, irr_weight);
}

unsigned char AI::golem(character actor, combat::tile targets[])
{
    unsigned char a_weight = 0; // attack decision weight
    unsigned char d_weight = 0; // defend decision weight
    unsigned char r_weight = 0; // rest decision weight
    unsigned char irr_weight = 0; // irrationality weight
    if (actor.AP <= 2)
    {
        return REST;
    }
    if (actor.HP >= 0.75) // greater than 75% of MAX_HP
    {
        a_weight += 0.05 * actor.HP; // all the multiplicative factors are not set in stone
        d_weight += 0.03 * (1 - actor.HP); // factor of the remaining part of MAX_HP
        r_weight += 0.01 * (1 - actor.AP); // factor of the remaining part of MAX_AP 
        irr_weight += 0.01 * actor.HP;
    }
    if (actor.HP >= 0.5) // greater than 50% of MAX_HP, (may be better as an else if? not sure)
    {
        a_weight += 0.02 * actor.HP;
        d_weight += 0.06 * (1 - actor.HP);
        r_weight += 0.03 * (1 - actor.AP);
        irr_weight += 0.04 * (1 - actor.HP); 
    }
    if (actor.HP < 0.25) // less than 25% of MAX_HP
    {
        a_weight += 0.01 * actor.HP;
        d_weight += 0.09 * (1 - actor.HP);
        r_weight += 0.03 * (1 - actor.AP);
        irr_weight += 0.06 * (1 - actor.HP); 
    }
    get_decision(a_weight, d_weight, r_weight, irr_weight);
}

unsigned char AI::l_treeling_target(character actor, combat::tile targets[])
{
    unsigned char strongest = get_strongest(targets);
    unsigned char tankiest = get_tankiest(targets);
    unsigned char weakest = get_weakest(targets);

    if (targets[strongest].combatant.HP <= 0.65)
    {
        return strongest;
    }

    else if (targets[weakest].combatant.HP <= 1.2 * actor.DAMAGE || actor.HP <= 0.4)
    {
        return weakest;
    }
    
    else if (targets[tankiest].combatant.DAMAGE >= actor.DAMAGE)
    {
        return tankiest;
    }

    else 
    {
        return rand() % N/2;
    }
}
unsigned char AI::skeleton_head_target(character actor, combat::tile targets[])
{
    unsigned char strongest = get_strongest(targets);
    unsigned char tankiest = get_tankiest(targets);
    unsigned char weakest = get_weakest(targets);

    if (targets[strongest].combatant.HP <= 0.65)
    {
        return strongest;
    }

    else if (targets[weakest].combatant.HP <= 1.2 * actor.DAMAGE)
    {
        return weakest;
    }
    
    else if (targets[tankiest].combatant.DAMAGE >= actor.DAMAGE)
    {
        return tankiest;
    }

    else 
    {
        return rand() % N/2;
    }
}
unsigned char AI::golem_target(character actor, combat::tile targets[])
{
    unsigned char strongest = get_strongest(targets);
    unsigned char tankiest = get_tankiest(targets);
    unsigned char weakest = get_weakest(targets);

    if (targets[strongest].combatant.HP <= 0.65)
    {
        return strongest;
    }

    else if (targets[weakest].combatant.HP <= 1.2 * actor.DAMAGE)
    {
        return weakest;
    }
    
    else if (targets[tankiest].combatant.DAMAGE >= actor.DAMAGE)
    {
        return tankiest;
    }

    else 
    {
        return rand() % N/2;
    }
}

AI::AI(unsigned char kind, character actor, combat::tile targets[])
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
    case GOLEM:
        golem(actor, targets);
        break;
    default:
        break;
    }
}

AI::~AI()
{
}


#endif