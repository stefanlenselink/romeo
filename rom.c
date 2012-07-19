#include <stdio.h>			// For sprintf
#include <string.h>
#include <stdlib.h>


#include "os_structs.h"
#include "rom.h"
#include "utils.h"			// For LocateOverlayResource
#include "relocate.h"		// For Relocate_CardHeader

/**************************************************
 * Include the PRC/database ordering information
 */
#include "SystemResources.h"
#include "400.rom.h"
#include "350.rom.h"
#include "330.rom.h"
#include "325.rom.h"
#include "310.rom.h"
#include "300.rom.h"
#include "200.rom.h"
#include "100.rom.h"
/**************************************************/

/*
 * Defaults for different version of the PalmOS
 */
ROMVersion ROMVers[]	=	//numROMVers] = 
{
	{0x0400,								// PalmOS version
	 0x10c00000,							// ROM Base
	 0x00008000,							// Small ROM Size (default)
	 0x00004000,							// Small ROM card size (default)
	 small_PRC_ROMS_400_rom,
	 "D,dr+",								// Layout of PRCs in small ROM
	 "D,dr+",								// Layout of PRCs added to small ROM
	 0x00400000 - 0x8000,					// Big ROM Size (default)
	 0x00400000 - 0x10000,					// Big ROM card size (default)
	 large_PRC_ROMS_400_rom,
	 "D,dr+",								// Layout of PRCs in large ROM
	 "D,dr+",								// Layout of PRCs added to big ROM
	 DEF_CARD_NAME,							// Card Name
	 DEF_CARD_MANU,							// Card Manufacturer Name
	 0x3000,								// Card Init Stack
	 0x05,									// Card Header Version
	 memCardHeaderFlagEZ,					// Card Flags
	 0x01,									// Card Version
	 0x0,									// RW Parms Offset
	 0x0,									// RW Parms Size
	 0x06000,								// RO Params offset
	 0x08000,								// Big ROM offset
	 0x0,									// RW Working Offset
	 0x0,									// RW Working Offset
	 DEF_STORE_NAME,						// Store Name
	 0x01,									// Store Version
	 0x0,									// Store Flags
	 0x0,									// NVParams Locale Language
	 0x017,									// NVParams Locale Country (US?)
	 memHeapFlagVers4|memHeapFlagReadOnly,	// Heap Flags
	 "",									// RO Parms Data File
	 "",									// RW Parms Data File
	 ""										// RW Working Data File
	},
	{0x0350,								// PalmOS version
	 0x10c00000,							// ROM Base
	 0x00008000,							// Small ROM Size (default)
	 0x00004000,							// Small ROM card size (default)
	 small_PRC_ROMS_350_rom,
	 "D,dr+",								// Layout of PRCs in small ROM
	 "D,dr+",								// Layout of PRCs added to small ROM
	 0x00200000 - 0x8000,					// Big ROM Size (default)
	 0x00188000 - 0x8000,					// Big ROM card size (default)
	 large_PRC_ROMS_350_rom,
	 "D,dr+",								// Layout of PRCs in large ROM
	 "D,dr+",								// Layout of PRCs added to big ROM
	 DEF_CARD_NAME,							// Card Name
	 DEF_CARD_MANU,							// Card Manufacturer Name
	 0x3000,								// Card Init Stack
	 0x04,									// Card Header Version
	 memCardHeaderFlagEZ,					// Card Flags
	 0x01,									// Card Version
	 0x0,									// RW Parms Offset
	 0x0,									// RW Parms Size
	 0x06000,								// RO Params offset
	 0x08000,								// Big ROM offset
	 0x0,									// RW Working Offset
	 0x0,									// RW Working Offset
	 DEF_STORE_NAME,						// Store Name
	 0x01,									// Store Version
	 0x0,									// Store Flags
	 0x0,									// NVParams Locale Language
	 0x017,									// NVParams Locale Country (US?)
	 memHeapFlagVers4|memHeapFlagReadOnly,	// Heap Flags
	 "",									// RO Parms Data File
	 "",									// RW Parms Data File
	 ""										// RW Working Data File
	},
	{0x0330,								// PalmOS version
	 0x10c00000,							// ROM Base
	 0x00008000,							// Small ROM Size (default)
	 0x00004000,							// Small ROM card size (default)
	 small_PRC_ROMS_330_rom,
	 "r-,1532,d+,D",						// Layout of PRCs in small ROM
	 "r-,d+,D",								// Layout of PRCs added to small ROM
	 0x00200000 - 0x8000,					// Big ROM Size (default)
	 0x00188000 - 0x8000,					// Big ROM card size (default)
	 large_PRC_ROMS_330_rom,
	 "r-,15106,d+,D",						// Layout of PRCs in large ROM
	 "r-,d+,D",								// Layout of PRCs added to big ROM
	 DEF_CARD_NAME,							// Card Name
	 DEF_CARD_MANU,							// Card Manufacturer Name
	 0x3000,								// Card Init Stack
	 0x03,									// Card Header Version
	 memCardHeaderFlagEZ,					// Card Flags
	 0x01,									// Card Version
	 0x0,									// RW Parms Offset
	 0x0,									// RW Parms Size
	 0x06000,								// RO Params offset
	 0x08000,								// Big ROM offset
	 0x0,									// RW Working Offset
	 0x0,									// RW Working Offset
	 DEF_STORE_NAME,						// Store Name
	 0x01,									// Store Version
	 0x0,									// Store Flags
	 0x0,									// NVParams Locale Language
	 0x0,									// NVParams Locale Country
	 memHeapFlagVers3|memHeapFlagReadOnly,	// Heap Flags
	 "",									// RO Parms Data File
	 "",									// RW Parms Data File
	 ""										// RW Working Data File
	},
	/* This 3.25 version is for Palm VII - same for others?? */
	{0x0325,								// PalmOS version
	 0x10c00000,							// ROM Base
	 0x00010000,							// Small ROM Size (default)
	 0x00004000,							// Small ROM card size (default)
	 small_PRC_ROMS_325_rom,
	 "r-,2664,d+,D",						// Layout of PRCs in small ROM
	 "r-,d+,D",								// Layout of PRCs added to small ROM
	 0x00200000 - 0x10000,					// Big ROM Size (default)
	 0x00188000 - 0x10000,					// Big ROM card size (default)
	 large_PRC_ROMS_325_rom,
	 "r-,69440,d+,D",						// Layout of PRCs in large ROM
	 "r-,d+,D",								// Layout of PRCs added to big ROM
	 DEF_CARD_NAME,							// Card Name
	 DEF_CARD_MANU,							// Card Manufacturer Name
	 0x3000,								// Card Init Stack
	 0x03,									// Card Header Version
	 memCardHeaderFlagEZ,					// Card Flags
	 0x01,									// Card Version
	 0x0,									// RW Parms Offset
	 0x0,									// RW Parms Size
	 0x06000,								// RO Params offset
	 0x10000,								// Big ROM offset
	 0x0,									// RW Working Offset
	 0x0,									// RW Working Offset
	 DEF_STORE_NAME,						// Store Name
	 0x01,									// Store Version
	 0x0,									// Store Flags
	 0x0,									// NVParams Locale Language
	 0x0,									// NVParams Locale Country
	 memHeapFlagVers3|memHeapFlagReadOnly,	// Heap Flags
	 "",									// RO Parms Data File
	 "",									// RW Parms Data File
	 ""										// RW Working Data File
	},
	{0x0310,								// PalmOS version
	 0x10c00000,							// ROM Base
	 0x00008000,							// Small ROM Size (default)
	 0x00004000,							// Small ROM card size (default)
	 small_PRC_ROMS_310_rom,
	 "r-,d+,D",								// Layout of PRCs in small ROM
	 "r-,d+,D",								// Layout of PRCs added to small ROM
	 0x00200000 - 0x8000,					// Big ROM Size (default)
	 0x00130000 - 0x8000,					// Observed from 310.rom
	 large_PRC_ROMS_310_rom,
	 "r-,49866,d+,D",						// Layout of PRCs in large ROM
	 "r-,d+,D",								// Layout of PRCs added to big ROM
	 DEF_CARD_NAME,							// Card Name
	 DEF_CARD_MANU,							// Card Manufacturer Name
	 0x3000,								// Card Init Stack
	 0x03,									// Card Header Version
	 memCardHeaderFlagEZ,					// Card Flags
	 0x01,									// Card Version
	 0x0,									// RW Parms Offset
	 0x0,									// RW Parms Size
	 0x06000,								// RO Params offset
	 0x08000,								// Big ROM offset
	 0x0,									// RW Working Offset
	 0x0,									// RW Working Offset
	 DEF_STORE_NAME,						// Store Name
	 0x01,									// Store Version
	 0x0,									// Store Flags
	 0x0,									// NVParams Locale Language
	 0x0,									// NVParams Locale Country
	 memHeapFlagVers3|memHeapFlagReadOnly,	// Heap Flags
	 "",									// RO Parms Data File
	 "",									// RW Parms Data File
	 ""										// RW Working Data File
	},
	{0x0300,								// PalmOS version
	 0x00c00000,							// ROM Base
	 0x00008000,							// Small ROM Size (default)
	 0x00004000,							// Small ROM card size (default)
	 small_PRC_ROMS_300_rom,
	 "r+,d-,D",								// Layout of PRCs in small ROM
	 "r+,d-,D",								// Layout of PRCs added to small ROM
	 0x00200000 - 0x8000,					// Big ROM Size (default)
	 0x00188000 - 0x8000,					// Big ROM card size (default)
	 large_PRC_ROMS_300_rom,
	 "r+,40988,d-,D",						// Layout of PRCs in large ROM
	 "r+,d-,D",								// Layout of PRCs added to big ROM
	 DEF_CARD_NAME,							// Card Name
	 DEF_CARD_MANU,							// Card Manufacturer Name
	 0x3000,								// Card Init Stack
	 0x02,									// Card Header Version
	 0,										// Card Flags
	 0x01,									// Card Version
	 0x0,									// RW Parms Offset
	 0x0,									// RW Parms Size
	 0x06000,								// RO Params offset
	 0x08000,								// Big ROM offset
	 0x0,									// RW Working Offset
	 0x0,									// RW Working Offset
	 DEF_STORE_NAME,						// Store Name
	 0x01,									// Store Version
	 0x0,									// Store Flags
	 0x0,									// NVParams Locale Language
	 0x0,									// NVParams Locale Country
	 memHeapFlagVers2|memHeapFlagReadOnly,	// Heap Flags
	 "",									// RO Parms Data File
	 "",									// RW Parms Data File
	 ""										// RW Working Data File
	},
	{0x0200,								// PalmOS version
	 0x00c00000,							// ROM Base
	 0x00003000,							// Small ROM Size (default)
	 0x00003000,							// Small ROM card size (default)
	 small_PRC_ROMS_200_rom,
	 "r+,472,d+,D",							// Layout of PRCs in small ROM
	 "r+,d+,D",								// Layout of PRCs added to small ROM
	 0x00100000 - 0x3000,					// Big ROM Size (default)
	 0x000EE000 - 0x3000,					// Big ROM card size (default)
	 large_PRC_ROMS_200_rom,
	 "r+,d+,D",								// Layout of PRCs in large ROM
	 "r+,d+,D",								// Layout of PRCs added to big ROM
	 DEF_CARD_NAME,							// Card Name
	 DEF_CARD_MANU,							// Card Manufacturer Name
	 0x3000,								// Card Init Stack
	 0x01,									// Card Header Version
	 0,										// Card Flags
	 0x01,									// Card Version
	 0x0,									// RW Parms Offset
	 0x0,									// RW Parms Size
	 0x00000,								// RO Params offset
	 0x03000,								// Big ROM offset
	 0x0,									// RW Working Offset
	 0x0,									// RW Working Offset
	 DEF_STORE_NAME,						// Store Name
	 0x01,									// Store Version
	 0x0,									// Store Flags
	 0x0,									// NVParams Locale Language
	 0x0,									// NVParams Locale Country
	 memHeapFlagVers1|memHeapFlagReadOnly,	// Heap Flags
	 "",									// RO Parms Data File
	 "",									// RW Parms Data File
	 ""										// RW Working Data File
	},
	{0x0100,								// PalmOS version
	 0x00c00000,							// ROM Base
	 0x00003000,							// Small ROM Size (default)
	 0x00003000,							// Small ROM card size (default)
	 small_PRC_ROMS_100_rom,
	 "D,dr+",								// Layout of PRCs in small ROM
	 "D,dr+",								// Layout of PRCs added to small ROM
	 0x00080000 - 0x3000,					// Big ROM Size (default)
	 0x00080000 - 0x3000,					// Big ROM card size (default)
	 large_PRC_ROMS_100_rom,
	 "D,dr+",								// Layout of PRCs in large ROM
	 "D,dr+",								// Layout of PRCs added to big ROM
	 DEF_CARD_NAME,							// Card Name
	 DEF_CARD_MANU,							// Card Manufacturer Name
	 0x3000,								// Card Init Stack
	 0x01,									// Card Header Version
	 0,										// Card Flags
	 0x01,									// Card Version
	 0x0,									// RW Parms Offset
	 0x0,									// RW Parms Size
	 0x00000,								// RO Params offset
	 0x03000,								// Big ROM offset
	 0x0,									// RW Working Offset
	 0x0,									// RW Working Offset
	 DEF_STORE_NAME,						// Store Name
	 0x01,									// Store Version
	 0x0,									// Store Flags
	 0x0,									// NVParams Locale Language
	 0x0,									// NVParams Locale Country
	 memHeapFlagVers1|memHeapFlagReadOnly,	// Heap Flags
	 "",									// RO Parms Data File
	 "",									// RW Parms Data File
	 ""										// RW Working Data File
	}
};
UInt32	numROMVers	= (sizeof(ROMVers) / sizeof(ROMVers[0]));

/*
 * Known locales for PalmOS
 */
Locale Locales[romNumLocales] = {
	{ "US", 0x0000, 0x0017 },
	{ "JP", 0x0006, 0x000D },
	{ "DE", 0x0002, 0x0008 },
	{ "ES", 0x0004, 0x0013 },
	{ "FR", 0x0001, 0x0007 },
	{ "IT", 0x0003, 0x000C }
};

ROMVersion* LocateROMVerByOSVer (UInt16	osver)
{
	int numROMVers = sizeof (ROMVers) / sizeof (ROMVers[0]);
	int idex;
	for (idex = 0; idex < numROMVers; idex++)
	{
		if (ROMVers[idex].palmOSVer == osver)
			return &ROMVers[idex];
	}
	return NULL;
}

ROMVersion*		GuessVersion (ROMPtr pROM)
{
	UInt32	idex	= 0;

	while (idex < numROMVers && 
		   ROMVers[idex].card_hdrVersion > pROM->pCard->hdrVersion)
		idex++;

	while (idex < numROMVers &&
		   memUHeapVerFromFlags (ROMVers[idex].heap_flags) > 
		   memUHeapVer (pROM->pHeapList->heapOffset[0]))
		idex++;
		  
	if (ROMVers[idex].card_hdrVersion != pROM->pCard->hdrVersion ||
		memUHeapVerFromFlags (ROMVers[idex].heap_flags) !=
			memUHeapVer (pROM->pHeapList->heapOffset[0])		 ||
		idex == numROMVers)
		return NULL;

	return &ROMVers[idex];
}

// Extract all the version info from the rom itself
ROMVersion*		CollectVersion (ROMPtr pROM)
{
	ROMVersion* guess		= GuessVersion (pROM);	
	ROMVersion*	vers		= (ROMVersion*) malloc (sizeof (ROMVersion));
	PRCIDPtr	pPRCList	= NULL;
	UInt32		idex;
	
	if (! vers)
		return NULL;

	memset (vers, 0, sizeof (*vers));
	
	if (guess)
	{
		vers->palmOSVer = guess->palmOSVer;

		strcpy(vers->small_layout,    guess->small_layout);
		strcpy(vers->small_addLayout, guess->small_addLayout);
		strcpy(vers->big_layout,      guess->big_layout);
		strcpy(vers->big_addLayout,   guess->big_addLayout);
	}

	/*
	 * Allocate space for our database list information
	 */
	if (pROM->pDatabaseList->numDatabases > 0)
	{
		UInt32	bytes	= (pROM->pDatabaseList->numDatabases+1) *
						  sizeof(PRCIDType);
		pPRCList = (PRCIDPtr)malloc(bytes);
		if (! pPRCList)
		{
			free(vers);
			return NULL;
		}

		memset(pPRCList, 0, bytes);
	}

	vers->ROM_base				= pROM->ROM_base;

	// if this is a small rom
	if (pROM->flags == RT_SMALL)
	{
		vers->small_ROMSize		= pROM->ROMSize;
		vers->small_cardSize	= pROM->CardSize;
		vers->small_PRCList     = pPRCList;
	}
	// large rom
	else
	{
		vers->big_ROMSize		= pROM->ROMSize;
		vers->big_cardSize		= pROM->CardSize;
		vers->big_PRCList       = pPRCList;
	}

	// Translate the Card Info into ROM space...
	Relocate_CardHeader(pROM->pCard, (UInt32)pROM->pROM, pROM->Card_base);

	strncpy (vers->card_name, pROM->pCard->name, sizeof (vers->card_name));
	strncpy (vers->card_manuf, pROM->pCard->manuf, sizeof (vers->card_manuf));
	if (pROM->pCard->version <= 3)
		vers->card_initStack = pROM->pCard->initStack;
	vers->card_hdrVersion				= pROM->pCard->hdrVersion;
	vers->card_flags					= pROM->pCard->flags;
	vers->card_version					= pROM->pCard->version;
	vers->card_readWriteParmsOffset		= pROM->pCard->readWriteParmsOffset;
	vers->card_readWriteParmsSize		= pROM->pCard->readWriteParmsSize;
	vers->card_readOnlyParmsOffset		= pROM->pCard->readOnlyParmsOffset -
	                                      pROM->ROM_base;
	vers->card_bigROMOffset				= pROM->pCard->bigROMOffset -
	                                      pROM->ROM_base;
	vers->card_readWriteWorkingOffset	= pROM->pCard->readWriteWorkingOffset;
	vers->card_readWriteWorkingSize		= pROM->pCard->readWriteWorkingSize;

	// Translate it back...
	Relocate_CardHeader(pROM->pCard, pROM->Card_base, (UInt32)pROM->pROM);

	
	strncpy (vers->store_name, pROM->pStore->name, sizeof (vers->store_name));
	vers->store_version					= pROM->pStore->version;
	vers->store_flags					= pROM->pStore->flags;

	vers->nvparams_localeLanguage		= pROM->pStore->nvParams.localeLanguage;
	vers->nvparams_localeCountry		= pROM->pStore->nvParams.localeCountry;


	vers->heap_flags = ((MemHeapHeaderType*)pROM->pHeapList->heapOffset[0])->flags;

	vers->readOnlyParmsDataFile[0]		= '\0';
	vers->readWriteParmsDataFile[0]		= '\0';
	vers->readWriteWorkingDataFile[0]	= '\0';

	/*
	 * Finally, grab all PRC information.
	 */
	for (idex = 0; idex < pROM->pDatabaseList->numDatabases; idex++)
	{
#if	1
		DatabaseHdrPtr	pDB			= pROM->pSortedDBList[idex];
		UInt32			ItemIdex	= LocateItemIdex((void**)pROM->pDatabaseList->databaseOffset,
									                 (void*)pDB,
													 pROM->pDatabaseList->numDatabases,
													 sizeof(pDB));

		if (ItemIdex >= pROM->pDatabaseList->numDatabases)
		{
			pDB      = (DatabaseHdrPtr)(pROM->pDatabaseList->databaseOffset[idex]);
			ItemIdex = idex;
		}
#else
		DatabaseHdrPtr	pDB	= (DatabaseHdrPtr)
							        pROM->pDatabaseList->databaseOffset[idex];
#endif

		sprintf(pPRCList[idex].dbName, "%s.%s", pDB->name,
		                                       (IsResource(pDB) ?"prc" :"pdb"));
		pPRCList[idex].type      = pDB->type;
		pPRCList[idex].creator   = pDB->creator;
		pPRCList[idex].isOverlay = (LocateOverlayResource(pDB) ? 1 : 0);
	}

	return vers;
}

// Take version info gathered from a small and large rom and merge them

ROMVersion* MergeVersions (ROMVersion* smallVers, 
						   ROMVersion* bigVers)
{
	ROMVersion* vers = (ROMVersion*)malloc (sizeof (*vers));

	if (! vers)
		return NULL;

	// They are the same except the small_* and big_* fields
	if (bigVers)
		memcpy (vers, bigVers, sizeof (*vers));
	else if(smallVers)
		memcpy (vers, smallVers, sizeof (*vers));
	else
		return NULL;

	if (smallVers)
	{
		vers->small_ROMSize		= smallVers->small_ROMSize;
		vers->small_cardSize	= smallVers->small_cardSize;
		vers->small_PRCList     = smallVers->small_PRCList;
	}
	
	// FIXME: Is this right?
	if (bigVers)
		bigVers->big_PRCList     = NULL;
	if (smallVers)
		smallVers->small_PRCList = NULL;

	return vers;
}

Locale*		LocateLocaleByName (char* name)
{
	int idex;

	for (idex = 0; idex < romNumLocales; idex++)
	{
		if (! strncmp (Locales[idex].name, name, sizeof (Locales[idex].name)))
			return &Locales[idex];
	}

	return &Locales[0];
}

