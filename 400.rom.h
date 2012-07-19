/*
 * Hacked together ordering file for 4.0 roms.  This isn't a complete
 * ordering, but since this stuff will all probably go away eventually, I'm
 * not going to work too hard on it.
 */

#ifndef	__ROMS_400_rom_H__
#define	__ROMS_400_rom_H__
PRCIDType	small_PRC_ROMS_400_rom[]	= {
                           {NNAME,        sysFileTBoot,       sysFileCSystem,0},
                           {NNAME,    sysFileTSmallHal,       sysFileCSystem,0},
                           {NNAME,0,0,0}
};
PRCIDType	large_PRC_ROMS_400_rom[]	= {
                           {NNAME,      sysFileTSystem,       sysFileCSystem,0},
                           {NNAME,      sysFileTBigHal,       sysFileCSystem,0},
                           {NNAME,      sysFileTSplash,       sysFileCSystem,0},
                           {NNAME,      sysFileTSystem,       sysFileCSystem,1},
                           {NNAME,      sysFileTSplash,       sysFileCSystem,1},
						   
						   /* 
							* For EZ based machines
							*/
                           {NNAME,  sysFileTUartPlugIn,    sysFileCUart328EZ,0},
                           {NNAME,  sysFileTUartPlugIn,    sysFileCUart328EZ,1},
						   
						   /* 
							* For nonEZ based machines
							*/
                           {NNAME,  sysFileTUartPlugIn,      sysFileCUart328,0},
                           {NNAME,  sysFileTUartPlugIn,      sysFileCUart328,1},
                           {NNAME,  sysFileTUartPlugIn,      sysFileCUart650,0},
                           {NNAME,  sysFileTUartPlugIn,      sysFileCUart650,1},
                           
						   {NNAME,     sysFileTDefault,   sysFileCModemPanel,0},
                           {NNAME,     sysFileTDefault,   sysFileCModemPanel,0},
                           {NNAME,      sysFileTKernel,       sysFileCSystem,0},
                           {NNAME,     sysFileTLibrary,sysFileCSerialWrapper,0},
                           {NNAME,  sysFileTUIAppShell,       sysFileCSystem,0},
#if 0
						   {NNAME, sysFileTApplication,     sysFileCLauncher,0},
                           {NNAME, sysFileTApplication,     sysFileCLauncher,1},
                           {NNAME, sysFileTApplication,  sysFileCPreferences,0},
                           {NNAME, sysFileTApplication,  sysFileCPreferences,1},
                           {NNAME, sysFileTApplication,     sysFileCSecurity,0},
                           {NNAME, sysFileTApplication,     sysFileCSecurity,1},
                           {NNAME, sysFileTApplication,      sysFileCAddress,0},
                           {NNAME, sysFileTApplication,      sysFileCAddress,1},
                           {NNAME, sysFileTApplication,     sysFileCDatebook,0},
                           {NNAME, sysFileTApplication,     sysFileCDatebook,1},
                           {NNAME, sysFileTApplication,         sysFileCMemo,0},
                           {NNAME, sysFileTApplication,         sysFileCMemo,1},
                           {NNAME, sysFileTApplication,         sysFileCToDo,0},
                           {NNAME, sysFileTApplication,         sysFileCToDo,1},
                           {NNAME, sysFileTApplication,   sysFileCCalculator,0},
                           {NNAME, sysFileTApplication,   sysFileCCalculator,1},
                           {NNAME, sysFileTApplication,      sysFileCExpense,0},
                           {NNAME, sysFileTApplication,      sysFileCExpense,1},
                           {NNAME, sysFileTApplication,         sysFileCMail,0},
                           {NNAME, sysFileTApplication,         sysFileCMail,1},
                           {NNAME,       sysFileTPanel,    sysFileCDigitizer,0},
                           {NNAME,       sysFileTPanel,    sysFileCDigitizer,1},
                           {NNAME,       sysFileTPanel,      sysFileCGeneral,0},
                           {NNAME,       sysFileTPanel,      sysFileCGeneral,1},
                           {NNAME,       sysFileTPanel,      sysFileCButtons,0},
                           {NNAME,       sysFileTPanel,      sysFileCButtons,1},
                           {NNAME,       sysFileTPanel,   sysFileCModemPanel,0},
                           {NNAME,       sysFileTPanel,   sysFileCModemPanel,1},
                           {NNAME,       sysFileTPanel,      sysFileCFormats,0},
                           {NNAME,       sysFileTPanel,      sysFileCFormats,1},
                           {NNAME,       sysFileTPanel,        sysFileCOwner,0},
                           {NNAME,       sysFileTPanel,        sysFileCOwner,1},
                           {NNAME,       sysFileTPanel,    sysFileCShortCuts,0},
                           {NNAME,       sysFileTPanel,    sysFileCShortCuts,1},
                           {NNAME, sysFileTApplication,         sysFileCSync,0},
                           {NNAME, sysFileTApplication,         sysFileCSync,1},
                           {NNAME,     sysFileTHtalLib,      sysFileCPADHtal,0},
                           {NNAME,      sysFileTExgLib,        sysFileCIrLib,0},
                           {NNAME,  sysFileTVirtPlugin,   sysFileCVirtIrComm,0},
                           {NNAME,  sysFileTVirtPlugin,   sysFileCVirtIrComm,1},
                           {NNAME,     sysFileTLibrary,          sysFileCNet,0},
                           {NNAME,        sysFileTneti,          sysFileCppp,0},
                           {NNAME,        sysFileTneti,          sysFileCppp,1},
                           {NNAME,        sysFileTneti,         sysFileCslip,0},
                           {NNAME,        sysFileTneti,         sysFileCslip,1},
                           {NNAME,        sysFileTneti,         sysFileCloop,0},
                           {NNAME,       sysFileTPanel, sysFileCNetworkPanel,0},
                           {NNAME,       sysFileTPanel, sysFileCNetworkPanel,1},
                           {NNAME,        sysFileTsdio,         sysFileCping,0},
                           {NNAME,        sysFileTsdio,         sysFileCnett,0},
                           {NNAME,     sysFileTHtalLib,      sysFileCTCPHtal,0},
                           {NNAME, sysFileTApplication,        sysFileCSetup,0},
                           {NNAME, sysFileTApplication,        sysFileCSetup,1},
#endif
                           {NNAME,0,0,0}
};
#endif	/* __ROMS_400_rom_H__ */
