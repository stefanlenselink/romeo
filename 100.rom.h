#ifndef	__ROMS_100_rom_H__
#define	__ROMS_100_rom_H__
PRCIDType	small_PRC_ROMS_100_rom[]	= { 
                           {NNAME,        sysFileTBoot,       sysFileCSystem,0},
                           {NNAME,0,0,0}
};
PRCIDType	large_PRC_ROMS_100_rom[]	= { 
                           {NNAME,      sysFileTSystem,       sysFileCSystem,0},
                           {NNAME, sysFileTApplication,      sysFileCAddress,0},
                           {NNAME,      sysFileTKernel,       sysFileCSystem,0},
                           {NNAME, sysFileTApplication,     sysFileCDatebook,0},
                           {NNAME, sysFileTApplication,   sysFileCCalculator,0},
                           {NNAME,  sysFileTUIAppShell,       sysFileCSystem,0},
                           {NNAME, sysFileTApplication,         sysFileCToDo,0},
                           {NNAME, sysFileTApplication,         sysFileCSync,0},
                           {NNAME, sysFileTApplication,     sysFileCSecurity,0},
                           {NNAME, sysFileTApplication,  sysFileCPreferences,0},
                           {NNAME, sysFileTApplication,       sysFileCMemory,0},
                           {NNAME, sysFileTApplication,         sysFileCMemo,0},
                           {NNAME,0,0,0}
};
#endif	/* __ROMS_100_rom_H__ */
