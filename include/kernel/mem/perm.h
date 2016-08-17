#ifndef __PERM_H__
#define __PERM_H__

//Format PERM_P(privileged permissions)_U(user permissions)
//Bits [3] NX, [2]APX, [1:0] AP
#define PERM_PNA_UNA 0
#define PERM_PRW_UNA 1
#define PERM_PRW_UR 2
#define PERM_PRW_URW 3

#define PERM_PR_UNA 5 //(101)
#define PERM_PR_UR 6 //(110)

#define PERM_SET_NX(u) u|=0x08
#define PERM_CLEAR_NX(u) u&=0x07

#define PERM_NX(u) ((u&8)>>3)
#define PERM_AP(u) (u&3)
#define PERM_APX(u) ((u&4)>>2)

#endif