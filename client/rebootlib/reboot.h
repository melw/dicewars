/**********************************
  Copyright (C) Rick Wong (Lick)
  http://lickr.org
  http://licklick.wordpress.com

  See license in readme.txt
***********************************/
#ifndef __REBOOT_H
#define __REBOOT_H

#include <nds.h>


#ifdef __cplusplus
extern "C" {
#endif

#ifndef BOOL
#define BOOL int
#endif

//====================================
#ifdef ARM9
//====================================
    BOOL can_reboot();

    void reboot();                          // reboot to device-menu

    void reboot_file(const char *ndsfile);  // reboot and run NDS file
                                            // ONLY WORKS WITH DLDI !
#endif


//====================================
#ifdef ARM7
//====================================
    BOOL need_reboot();

    void reboot();
#endif


#ifdef __cplusplus
}
#endif

#endif
