#ifndef __CONTROLLERS__
#define __CONTROLLERS__

#define CNTRL_COUNT                  (3)  // number of controllers
#define CUSTOM_INDEX   (CNTRL_COUNT - 1)  // index of the last controller -> custom controller

/* array of controllers */
int Controllers [CNTRL_COUNT][18] = {
    /* Power       Mute        1           2           3           4           5           6           7           8           9           0           *           #           Vol+        Vol-        Ch+         Ch- */       // Buttons
    { 0x20DF10EF, 0x20DF906F, 0x20DF8877, 0x20DF48B7, 0x20DFC837, 0x20DF28D7, 0x20DFA857, 0x20DF6897, 0x20DFE817, 0x20DF18E7, 0x20DF9867, 0x20DF08F7, 0x20DF14EB, 0x20DFDA25, 0x20DF40BF, 0x20DFC03F, 0x20DF00FF, 0x20DF807F},  // LG
    { 0x36876503, 0x219EB24D, 0x219E18E7, 0x219E9867, 0x219E708F, 0x219E38C7, 0x219EB847, 0x219E7A85, 0x219E3AC5, 0x0000ABCD, 0x219E3AC5, 0x219EFA05, 0x219E8877, 0x219ED827, 0x219E58A7, 0x219E00FF, 0x219EE01F, 0x219E906F},  // WD
    { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000}   // Custom
};

#endif
