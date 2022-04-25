// a test program to verify the combat is working correctly

#include "combat.h"

int main()
{
    character fight1; character fight2; character fight3; character fight4; character fight5; character fight6;
    fight1.name = "fighter1"; fight1.type = WARRIOR; fight1.HP = 80; fight1.AP = 30; fight1.DAMAGE = 30; fight1.RESISTANCE = 40;
    fight2.name = "fighter2"; fight2.type = WARRIOR; fight2.HP = 40; fight2.AP = 25; fight2.DAMAGE = 30; fight2.RESISTANCE = 40;
    fight3.name = "fighter3"; fight3.type = WARRIOR; fight3.HP = 60; fight3.AP = 20; fight3.DAMAGE = 30; fight3.RESISTANCE = 40;
    fight4.name = "fighter4"; fight4.type = WARRIOR; fight4.HP = 70; fight4.AP = 15; fight4.DAMAGE = 30; fight4.RESISTANCE = 40;
    fight5.name = "fighter5"; fight5.type = WARRIOR; fight5.HP = 30; fight5.AP = 10; fight5.DAMAGE = 30; fight5.RESISTANCE = 40;
    fight6.name = "fighter6"; fight6.type = WARRIOR; fight6.HP = 50; fight6.AP = 5; fight6.DAMAGE = 30; fight6.RESISTANCE = 40;      
    combat(fight1, fight2, fight3, fight4, fight5, fight6);
    return EXIT_SUCCESS;
}