#include <stdio.h>
#include <stdlib.h>	// For strtoul...
#include <unistd.h>	// For read...
#include <string.h>	// For strlen...

#include "types.h"
#include "rom.h"
#include "byte_swap.h"


static char		_buf [4096];
static char		_hold[4096];

// Defines for the different types of fields
#define INT				1
#define UINT			2
#define BOOL			3
#define CHAR			4
#define STRING			5
#define	PRCLIST			6

#define UNKNOWN_KEY		-1
#define INVALID_VALUE	-2
#define INVALID_TYPE	-1000
#define	INVALID_PARAMS	-2000
#define	SYSTEM_ERROR	-3000

typedef struct {
	char*	name;
	int		type;	// int, string, etc
	int		size;	// sizeof int, max length of string, etc
	int		offset;	// offset into structure to copy this value
} s_Field;

static ROMVersion	dummy;
#define offset_of(f,s)	(((void*)(&(s.f))) - ((void*)(&(s))))
#define	FIELD(n,t)		"" #n "", t, sizeof(dummy.n), offset_of(n, dummy)

//
// The fields of ROMVersion
//
static s_Field	romft[] = {
	{FIELD(palmOSVer,					UINT)},	// bIsVer
	{FIELD(ROM_base,					UINT)},

	{FIELD(small_ROMSize,				UINT)},
	{FIELD(small_cardSize,				UINT)},
	{FIELD(small_PRCList,				PRCLIST)},
	{FIELD(small_layout,				STRING)},
	{FIELD(small_addLayout,				STRING)},

	{FIELD(big_ROMSize,					UINT)},
	{FIELD(big_cardSize,				UINT)},
	{FIELD(big_PRCList,					PRCLIST)},
	{FIELD(big_layout,					STRING)},
	{FIELD(big_addLayout,				STRING)},

	{FIELD(card_name,					STRING)},
	{FIELD(card_manuf,					STRING)},
	{FIELD(card_initStack,				UINT)},
	{FIELD(card_hdrVersion,				UINT)},
	{FIELD(card_flags,					UINT)},
	{FIELD(card_version,				UINT)},
	{FIELD(card_readOnlyParmsOffset,	UINT)},
	{FIELD(card_readWriteParmsOffset,	UINT)},
	{FIELD(card_readWriteParmsSize,		UINT)},
	{FIELD(card_bigROMOffset,			UINT)},
	{FIELD(card_readWriteParmsOffset,	UINT)},
	{FIELD(card_readWriteParmsSize,		UINT)},
	
	{FIELD(store_name,					STRING)},
	{FIELD(store_version,				UINT)},
	{FIELD(store_flags,					UINT)},

	{FIELD(nvparams_localeLanguage,		UINT)},
	{FIELD(nvparams_localeCountry,		UINT)},
	
	{FIELD(heap_flags,					UINT)},

	{FIELD(readOnlyParmsDataFile,		STRING)},
	{FIELD(readWriteParmsDataFile,		STRING)},
	{FIELD(readWriteWorkingDataFile,	STRING)},
};
#define	N_FIELDS	sizeof(romft) / sizeof(romft[0])

//
// Locate the field with the given key/name
//
static
s_Field*	FieldFinder (char* key)
{
	UInt32	idex;
	for (idex = 0; idex < N_FIELDS; idex++)
	{
		if (! strcasecmp (key, romft[idex].name))
			return &(romft[idex]);
	}

	return NULL;
}

//
// Find the field in the field table, parse the value, and store it at the
// specified offset in the target
//
static
Int32 String2Field	(void*	target,
					 char*	key,
					 char*	val)
{
	s_Field*	pField;
	UInt32		ui32;
	Int32		i32;
	char*		ptr;
	
	if ((! target) || (! key) || (! val))
		return INVALID_PARAMS;

	pField = FieldFinder(key);
	if (! pField)
		return UNKNOWN_KEY;

	switch (pField->type)
	{
		case INT:
			i32 = strtol (val, &ptr, 0);
			// Make sure it was a valid integer
			if (! *val || *ptr)
				return INVALID_VALUE;

			// Make sure the value fits in the right size integer
			if (pField->size == 1 && (i32 > INT8_MAX || i32 < INT8_MIN))
				return INVALID_VALUE;
			if (pField->size == 2 && (i32 > INT16_MAX || i32 < INT16_MIN))
				return INVALID_VALUE;

			if (pField->size == 1)
				*((Int8*)(target + pField->offset)) = i32;
			if (pField->size == 2)
				*((Int16*)(target + pField->offset)) = i32;
			else
				*((Int32*)(target + pField->offset)) = i32;

			break;
		
		case UINT:
			ui32 = strtol (val, &ptr, 0);
			// Make sure it was a valid integer
			if (! *val || *ptr)
				return INVALID_VALUE;

			// Make sure the value fits in the right size integer
			if (pField->size == 1 && ui32 > UINT8_MAX)
				return INVALID_VALUE;
			if (pField->size == 2 && ui32 > UINT16_MAX)
				return INVALID_VALUE;

			if (pField->size == 1)
				*((UInt8*)(target + pField->offset)) = ui32;
			if (pField->size == 2)
				*((UInt16*)(target + pField->offset)) = ui32;
			else
				*((UInt32*)(target + pField->offset)) = ui32;

			break;
			
		case BOOL:
			if (*val == 'y' || *val == 't' || *val == '1')
				i32 = 1;
			else if (*val == 'n' || *val == 'f' || *val == '0')
				i32 = 0;
			else
				return INVALID_VALUE;

			if (pField->size == 1)
				*((Int8*)(target + pField->offset)) = i32;
			if (pField->size == 2)
				*((Int16*)(target + pField->offset)) = i32;
			else
				*((Int32*)(target + pField->offset)) = i32;

			break;
			
		case CHAR:
			if (strlen (val) != 1)
				return INVALID_VALUE;

			*((Int8*)(target + pField->offset)) = *val;

			break;

		case STRING:
			if (strlen (val) >= pField->size)
				return INVALID_VALUE;

			strncpy (target + pField->offset, val, pField->size);

			break;

		case PRCLIST:
			{
				UInt32		nPRC	= 0;
				UInt32		idex;
				char*		pPos	= val;
				PRCIDPtr	pPRCList	= NULL;
				UInt8		ID[5];

				/*
				 * First, how many PRCs are there
				 */
				nPRC = 1;
				while ((pPos) && (pPos = strchr(pPos, ' ')))
				{
					pPos++;
					nPRC++;
				}

				/*
				 * Allocate space for the PRC list
				 * (Don't forget room for the terminating NULL entry)
				 */
				if (nPRC > 0)
				{
					UInt32	bytes	= (nPRC+1) * sizeof(PRCIDType);
					pPRCList = (PRCIDPtr)malloc(bytes);
					if (! pPRCList)
						return SYSTEM_ERROR;

					memset(pPRCList, 0, bytes);
				}

				/*
				 * Now, parse the list:
				 * 	type.ctor/o/'dbName'
				 *
				 * 	Where 'type' and 'ctor' are 4-bytes of ASCIIized
				 * 	                            32-bit integer
				 *                              e.g. crsr == 0x72737263
				 *                                           (little-endian)
				 *	'o'      is 0 or 1
				 *	'dbName' is the name of the database (including an ending
				 *	                                      '.prc' or '.pdb')
				 */
				pPos = val;
				memset(ID, 0, sizeof(ID));
				for (idex = 0; idex < nPRC; idex++)
				{
					char*	pStart;
					UInt32	bytes	= sizeof(pPRCList[idex].dbName);
					memset(pPRCList[idex].dbName, 0, bytes);

					pPos[11] = '\0';

					memcpy(ID, pPos, 4);
					pPos += 5;
					memcpy(&(pPRCList[idex].type), ID,
					       sizeof(pPRCList[idex].type));
					pPRCList[idex].type = BYTE_SWAP_32(pPRCList[idex].type);
		
					memcpy(ID, pPos, 4);
					pPos += 5;
					memcpy(&(pPRCList[idex].creator), ID,
					       sizeof(pPRCList[idex].creator));
					pPRCList[idex].creator = BYTE_SWAP_32(pPRCList[idex].creator);

					if (*pPos == '1')
						pPRCList[idex].isOverlay = 1;
					else
						pPRCList[idex].isOverlay = 0;

					pPos += 3;

					/*
					 * Now, locate the file name if one is provided
					 */
					pStart = pPos;
					while ((*pPos != '\'') && ((pPos - pStart) < bytes))
						pPos++;

					if (pPos > pStart)
					{
						bytes = pPos - pStart;
						strncpy(pPRCList[idex].dbName, pStart, bytes);
					}
					pPos += 2;
				}

				*((PRCIDPtr*)(target + pField->offset)) = pPRCList;
			}
			break;

		default:
			return INVALID_TYPE;
	}
		
	return ((UInt8*)pField - (UInt8*)romft) / sizeof(romft[0]);
}

//
// Generate a string from the identified field
//
static
UInt32 Field2String	(ROMVersion*	pInfo,
					 s_Field*		pField,
					 char*			buf)
{
	char*	pData;

	if ((! pInfo) || (! pField) || (! buf))
		return INVALID_PARAMS;

	pData = (char*)pInfo + pField->offset;

	switch (pField->type)
	{
	case INT:
	case UINT:
	case BOOL:
	case CHAR:
		if (pField->size == 1)
		{
			sprintf(_buf,"%s: 0x%02X\n", pField->name, *((UInt8*)pData));
		}
		else if (pField->size == 2)
		{
			sprintf(_buf,"%s: 0x%04X\n", pField->name, *((UInt16*)pData));
		}
		else
		{
			sprintf(_buf,"%s: 0x%08lX\n", pField->name, *((UInt32*)pData));
		}
		break;

	case STRING:
		sprintf(_buf,"%s: %s\n", pField->name, ((UInt8*)pData));
		break;

	case PRCLIST:
		{
			PRCIDPtr*	ppPRCList	= ((PRCIDPtr*)pData);
			PRCIDPtr	pPRCList	= NULL;
			UInt32		col			= 0;
			UInt32		indent		= 0;
			UInt32		idex;

			sprintf(_buf,"%s: ", pField->name);

			indent = strlen(_buf);
			col    = indent;
			if ((ppPRCList) && (*ppPRCList))
			{
				UInt32	swap;
				char	ID[5];
				pPRCList = *ppPRCList;

				memset(ID, 0, sizeof(ID));
				for (idex = 0; (pPRCList[idex].type    != 0) &&
				               (pPRCList[idex].creator != 0); idex++)
				{
					if (idex)
					{
						strcat(_buf, " ");
						col++;
					}

					if (col > (78 - (14 + strlen(pPRCList[idex].dbName))))
					{
						strcat(_buf, "\\\n");
						col = indent;
						while (col-- > 0)
							strcat(_buf, " ");
						col = indent;
					}

					/*
					 * Our output will be:
					 * 	type.ctor/o/dbName
					 */
					swap = BYTE_SWAP_32(pPRCList[idex].type);
					memcpy(ID, &(swap), 4);
					strcat(_buf, ID); strcat(_buf, ".");

					swap = BYTE_SWAP_32(pPRCList[idex].creator);
					memcpy(ID, &(swap), 4);
					strcat(_buf, ID); strcat(_buf, "/");

					if (pPRCList[idex].isOverlay)
						strcat(_buf, "1");
					else
						strcat(_buf, "0");

					strcat(_buf, "/'");
					strcat(_buf, pPRCList[idex].dbName);
					strcat(_buf, "'");

					col += 14 + strlen(pPRCList[idex].dbName);
				}
			}
			strcat(_buf, "\n");
		}
		break;

	default:
		return INVALID_TYPE;
	}

	return 0;
}

/*
 * Trim a string - strip leading and trailing white-space
 */
static
char*	_trim	(char*	str)
{
	char*	pStart;
	char*	pEnd;

	if (! str)
		return NULL;
	
	pStart = str;
	pEnd   = pStart + (strlen(pStart)) - 1;
	
	// Strip off leading white-space
	while (*pStart && (pStart < pEnd) &&
	       ((*pStart == ' ') || (*pStart == '\t')))
	{
		pStart++;
	}

	// Empty string == NULL
	if (! *pStart)
		return NULL;

	// Strip off trailing white-space
	while (*pEnd && (pEnd > pStart) &&
	       ((*pEnd == ' ') || (*pEnd == '\t')))
	{
		pEnd--;
	}

	// Empty string == NULL
	if (pStart == pEnd)
		return NULL;
	pEnd++;

	// NULL terminate the end of the string
	*pEnd = '\0';

//fprintf (stderr, "str: [%s] -- pStart[%s], pEnd[%s]\n", str, pStart, pEnd);
	return pStart;
}

static
Int32	_readln	(int	hIn,
				 char*	buf,
				 UInt32	bufSize)
{
	char*	pCur	= buf;
	ssize_t	bytes;

	if ((hIn < 0) || (! buf))
		return -1;

	while (((pCur - buf) < bufSize) && ((bytes = read(hIn, pCur, 1)) == 1))
	{
		if (*pCur == '\n')
			break;

		pCur++;
	}

	if (bytes <= 0)
		return bytes;


	if (*pCur == '\n')
		*pCur = '\0';

	return pCur - buf + 1;
}

/*************************************************************************
 * Exported routines
 *
 * Read in ROM Version meta information from the incoming file handle
 */
ROMVersion*	Read_MetaInfo	(int	hInfo)
{
	ROMVersion	Info;
	ROMVersion*	pInfo		= NULL;
	ROMVersion*	pVers		= NULL;
	char *		pKey		= NULL;
	char *		pVal		= NULL;
	UInt32		line		= 0;
	Int32		bytes		= 0;
	UInt32		res			= 0;
	UInt8		bSomeFilled	= 0;
	char *		pBuf		= _buf;
	char *		pHold		= _hold;
	char *		pTmp		= NULL;


	memset(&Info, 0, sizeof(Info));

	*pHold = '\0';
	while ((bytes = _readln(hInfo, pBuf, sizeof(_buf))) > 0)
	{
		line++;

		if (bytes < 1)
			continue;

		// Trim the line
		pKey = _trim(pBuf);

		// Ignore comments and empty lines
		if ((! pKey) || (*pKey == '#'))
			continue;

		/*****************************************************
		 * Handle continuation lines (lines that end in '\')
		 *
		 * pKey now points to the first non-whitespace, key-related
		 * character in pBuf
		 */
		if (pHold[0])
		{
			if (strlen(pKey) + strlen(pHold) > sizeof(_buf))
			{
				fprintf (stderr, "*** Line too long (> %u characters) on line %ld\n",
						 sizeof(_buf), line);

				// Don't clear it out just yet so we can identify and ignore
				// ALL overflow lines.
				continue;
			}

			strcat (pHold, " ");
			strcat (pHold, pKey);

			// Swap pBuf and pHold
			pTmp  = pBuf;
			pBuf  = pHold;
			pHold = pTmp;

			pKey  = pBuf;
		}

		if (pBuf[strlen(pBuf)-1] == '\\')
		{
			/*
			 * If this is the FIRST continuation, save from pBuf
			 * (which contains the full KEY), otherwise, ignore
			 * the leading white-space by using pKey
			 */
			char*	pEnd	= &(pBuf[strlen(pBuf)-1]);
			pEnd--;
			while ((pEnd > pBuf) && ((*pEnd == ' ') || (*pEnd == '/t')))
				pEnd--;
			pEnd++;
			*pEnd = '\0';

			// Swap pBuf and pHold
			pTmp  = pBuf;
			pBuf  = pHold;
			pHold = pTmp;
			continue;
		}

		pHold[0] = '\0';
		/*****************************************************/

		// Locate the divider
		pVal = strchr(pKey, ':');
		if (! pVal)
		{
			fprintf (stderr, "*** No 'key: value' on line %ld\n", line);
			continue;
		}
		*pVal = '\0';

		// Trim the key & value
		pKey = _trim(pKey);
		pVal = _trim(pVal+1);
		if (! pVal)
		{
			//fprintf (stderr, "*** No 'value' for key[%s] on line %ld\n",
			//		 pKey, line);
			continue;
		}

		//
		// Store the value into the appropriate Version information field
		//
		res = String2Field(&Info, pKey, pVal);
		if (res < 0)
		{
			switch (res)
			{
			case INVALID_PARAMS:
				fprintf (stderr, "*** Invalid String2Field Parameters\n");
				break;

			case SYSTEM_ERROR:
				fprintf (stderr, "*** System error processing line %ld\n",
						 line);
				break;

			case UNKNOWN_KEY:
				fprintf (stderr, "*** Invalid Key Name[%s] on line %ld\n",
						 pKey, line);
				break;

			case INVALID_VALUE:
				fprintf (stderr, "*** Invalid Key Value[%s] for Key[%s] on line %ld\n",
						 pVal, pKey, line);
				break;

			case INVALID_TYPE:
				fprintf (stderr, "*** Invalid Key Type for Key[%s] on line %ld\n",
						 pKey, line);
				break;
			}

			continue;
		}

		if (res == 0)	// Version Number is the first element
		{
			/*
			 * If the ROM version is FIRST, we can fill in defaults, otherwise
			 * it'd be too much work to figure out which elements have already
			 * been filled in so we don't overwrite them...
			 */
			if (bSomeFilled)
			{
				fprintf (stderr, "*** Version is specified AFTER other settings: line %ld\n"
						         "***   If version was first, we could use it to set defaults.\n",
				         line);
			}
			else
			{
				/*
				 * Locate the default version information and
				 * fill it in.
				 */
				pVers = LocateROMVerByOSVer(Info.palmOSVer);

				if (! pVers)
				{
					fprintf (stderr, "*** Unknown ROM version %d on line %ld\n",
					         Info.palmOSVer, line);
							 
				}
				else
				{
					memcpy(&Info, pVers, sizeof(Info));
				}
			}
		}

		bSomeFilled = 1;
	}

	/*
	 * Now, create a new version instance and copy in the data we've read
	 */
	pInfo = (ROMVersion*)malloc (sizeof(ROMVersion));
	if (! pInfo)
		return NULL;
	
	memcpy(pInfo, &Info, sizeof(Info));

	/*
	 * If necessary, make a copy of the PRC lists
	 */
	if (pVers)
	{
		UInt32		idex;
		UInt32		nPRCsmall	= 0;
		UInt32		nPRCbig		= 0;
		UInt32		nBytes		= 0;
		PRCIDPtr	pPRCList	= NULL;

		if (pInfo->small_PRCList == pVers->small_PRCList)
		{
			/*
			 * How many items are in our small PRC list?
			 */
			pPRCList = pVers->small_PRCList;
			for (idex = 0; pPRCList                      &&
			               (pPRCList[idex].type    != 0) &&
			               (pPRCList[idex].creator != 0); idex++)
			{
				nPRCsmall++;
			}

			if (nPRCsmall > 0)
			{
				/*
				 * Don't forget the terminating NULL entry!
				 */
				nBytes               = (nPRCsmall+1) * sizeof(PRCIDType);
				pInfo->small_PRCList = (PRCIDPtr)malloc(nBytes);
				if (! pInfo->small_PRCList)
				{
					free(pInfo);
					return NULL;
				}

				memcpy(pInfo->small_PRCList, pVers->small_PRCList, nBytes);
			}
		}

		if (pInfo->big_PRCList == pVers->big_PRCList)
		{
			/*
			 * How many items are in our big PRC list?
			 */
			pPRCList = pVers->big_PRCList;
			for (idex = 0; pPRCList                      &&
			               (pPRCList[idex].type    != 0) &&
			               (pPRCList[idex].creator != 0); idex++)
			{
				nPRCbig++;
			}
	
			if (nPRCbig > 0)
			{
				/*
				 * Don't forget the terminating NULL entry!
				 */
				nBytes             = (nPRCbig+1) * sizeof(PRCIDType);
				pInfo->big_PRCList = (PRCIDPtr)malloc(nBytes);
				if (! pInfo->big_PRCList)
				{
					if (pInfo->small_PRCList)	free(pInfo->small_PRCList);
	
					free(pInfo);
					return NULL;
				}
	
				memcpy(pInfo->big_PRCList, pVers->big_PRCList, nBytes);
			}
		}
	}

	return pInfo;
}

/*
 * Write ROM Version meta information to the incoming file handle
 */
int	Write_MetaInfo	(int			hInfo,
					 ROMVersion*	pInfo)
{
	UInt32	idex;
	UInt32	res;

	if (! pInfo)
		return 0;

	for (idex = 0; idex < N_FIELDS; idex++)
	{
		res   = Field2String(pInfo, &romft[idex], _buf);
		if (res < 0)
		{
			switch (res)
			{
			case INVALID_PARAMS:
				fprintf (stderr, "*** Invalid String2Field Parameters\n");
				break;

			case SYSTEM_ERROR:
				fprintf (stderr, "*** System error\n");
				break;

			case INVALID_TYPE:
				fprintf (stderr, "*** Invalid Key Type\n");
				break;

			}

			continue;
		}

		if (write(hInfo, _buf, strlen(_buf)) != strlen(_buf))
		{
			fprintf (stdout, "*** Error writing\n");
			return 0;
		}
	}


	return 1;
}

#ifdef	TEST
#include <fcntl.h>
#include "utils.h"

int	main	(int	argc,
			 char*	argv[])
{
	int			out;
	int			in;
	ROMVersion*	pVersion	= &(ROMVers[0]);

#if	0
	out = open("test.out", O_CREAT | O_WRONLY | O_TRUNC, 0644);
	if (out < 0)
	{
		fprintf (stderr, "*** Cannot create output file\n");
		return -1;
	}

	Write_MetaInfo	(out, pVersion);
	close(out);
#endif

#if	0
	{
		//char*	name;
		//int		type;	// int, string, etc
		//int		size;	// sizeof int, max length of string, etc
		//int		offset;	// offset into structure to copy this value
		int	idex;

		for (idex = 0; idex < N_FIELDS; idex++)
		{
			fprintf (stdout, "%2d: %-32s type[%d], size[%d]/offset[%d]\n",
					 idex,
					 romft[idex].name,
					 romft[idex].type,
					 romft[idex].size,
					 romft[idex].offset);
		}
	}
#endif

	fprintf (stdout, "test.out\n");
	in  = open("test.out", O_RDONLY);
	if (in  < 0)
	{
		fprintf (stderr, "*** Cannot create input  file\n");
		return -1;
	}
	pVersion = Read_MetaInfo(in);
	close(in);

	if (! pVersion)
	{
		fprintf (stderr, "*** Cannot read meta info\n");
		return -1;
	}

	fprintf (stdout, "test.out1\n");
	out = open("test.out1", O_CREAT | O_WRONLY | O_TRUNC, 0644);
	if (out < 0)
	{
		fprintf (stderr, "*** Cannot create output file (2)\n");
		return -1;
	}

	Write_MetaInfo	(out, pVersion);
	close(out);

	FreeROMVersion(pVersion);

	return 0;
}
#endif	// TEST
