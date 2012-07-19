#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
/*
 * This Cygwin patch is due to Isaac Salzman
 */
#ifndef __CYGWIN
#include <errno.h>
#else
#include <sys/errno.h>
#define sys_errlist _sys_errlist
#endif /* __CYGWIN */
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "version.h"
#include "opts_parse.h"
#include "rom.h"
#include "os_structs.h"
#include "extract.h"
#include "assemble.h"
#include "output.h"
#include "utils.h"
#include "meta_info.h"


#define	SET_D(v)												\
	(v) = (opt_d.optidex != 0 || opt_V.optidex !=0) ?			\
					DETAIL_HEADER : DETAIL_NONE;				\
	if (opt_D.optidex)											\
	{															\
		/* Do we have an option modifier? */					\
		if (opt_D.optmod)										\
		{														\
			UInt32	Level = strtoul(opt_D.optmod, NULL, 0);		\
			if (Level)											\
				(v) = Level;									\
			else												\
				(v) = DETAIL_RECORD;							\
		}														\
		else													\
		{														\
			(v) = DETAIL_RECORD;								\
		}														\
																\
		if (! (v))												\
			(v) = DETAIL_HEADER;								\
	}

/******************************************************************************
 ******************************************************************************
 *** MAIN
 ******************************************************************************
 ******************************************************************************/
static char*	progName		= NULL;

static opt_info	opt_X			= EMPTY_OPT_INFO;
static opt_info	opt_B			= EMPTY_OPT_INFO;
static opt_info	opt_a			= EMPTY_OPT_INFO;
static opt_info	  opt_aR		= EMPTY_OPT_INFO;
static opt_info	  opt_ar		= EMPTY_OPT_INFO;
static opt_info	  opt_ab		= EMPTY_OPT_INFO;
static opt_info	  opt_al		= EMPTY_OPT_INFO;
static opt_info	    opt_alh		= EMPTY_OPT_INFO;
static opt_info	    opt_alp		= EMPTY_OPT_INFO;
static opt_info	  opt_as		= EMPTY_OPT_INFO;
static opt_info	    opt_ash		= EMPTY_OPT_INFO;
static opt_info	    opt_asp		= EMPTY_OPT_INFO;
static opt_info	  opt_ao		= EMPTY_OPT_INFO;
static opt_info	  opt_af		= EMPTY_OPT_INFO;
static opt_info	  opt_aF		= EMPTY_OPT_INFO;
static opt_info	  opt_an		= EMPTY_OPT_INFO;
static opt_info	  opt_aL		= EMPTY_OPT_INFO;
static opt_info	opt_r			= EMPTY_OPT_INFO;
static opt_info	opt_p			= EMPTY_OPT_INFO;
static opt_info	opt_x			= EMPTY_OPT_INFO;
static opt_info	  opt_xl		= EMPTY_OPT_INFO;
static opt_info	  opt_xs		= EMPTY_OPT_INFO;
static opt_info	  opt_xm		= EMPTY_OPT_INFO;
static opt_info	opt_v			= EMPTY_OPT_INFO;
static opt_info	  opt_vl		= EMPTY_OPT_INFO;
static opt_info	  opt_vs		= EMPTY_OPT_INFO;
static opt_info	opt_N			= EMPTY_OPT_INFO;
static opt_info	opt_h			= EMPTY_OPT_INFO;
static opt_info	opt_c			= EMPTY_OPT_INFO;
static opt_info	opt_d			= EMPTY_OPT_INFO;
static opt_info	opt_D			= EMPTY_OPT_INFO;
static opt_info	opt_H			= EMPTY_OPT_INFO;
static opt_info	opt_V			= EMPTY_OPT_INFO;
static opt_info	opt_version		= EMPTY_OPT_INFO;
static opt_info	opt_files		= EMPTY_OPT_INFO;

static parsed_opts		al_subopts[]= { {
								OPT_NOT_MODAL,
								"H", "Header",

								"Generate a large ROM with given card header version",
								{ "<version>", 1, 1, 1 },

								NULL,
								&opt_alh
							  },
							  { OPT_NOT_MODAL,
								"P", "Heap",

								"Generate a large ROM with given heap header version",
								{ "<version>", 1, 1, 1 },

								NULL,
								&opt_alp
							  },
							  NULL_OPTS	};

static parsed_opts		as_subopts[]= { {
								OPT_NOT_MODAL,
								"S", "header",

								"Generate a small ROM with given card header version",
								{ "<version>", 1, 1, 1 },

								NULL,
								&opt_ash
							  },
							  {
								OPT_NOT_MODAL,
								"p", "heap",

								"Generate a small ROM with given heap header version",
								{ "<version>", 1, 1, 1 },

								NULL,
								&opt_asp
							  },
							  NULL_OPTS	};



static parsed_opts		a_subopts[]	= { {
								OPT_NOT_MODAL,
								"R", "rom",

								"Read in a rom and modify it instead of creating a new one",
								{ "<file>", 1, 1, 1 },

								NULL,
								&opt_aR,
							  },
							  {
								OPT_NOT_MODAL,
								"r", "romBase",

								"Create a ROM image at the given base address.",
								{ "<base>", 1, 1, 1 },

								NULL,
								&opt_ar
						      },
							  {
								OPT_NOT_MODAL,
								"b", "bigRomOffset",

								"Place the big rom at the given offset from the rom base.",
								{ "<offset>", 1, 1, 1 },

								NULL,
								&opt_ab
						      },
							  {
								OPT_NOT_MODAL,
								"f", "format_small",

								"Generate the small ROM with the given format string",
								{ "<format>", 1, 1, 1 },

								NULL,
								&opt_af
						      },
							  {
								OPT_NOT_MODAL,
								"F", "format_big",

								"Generate the big ROM with the given format string",
								{ "<format>", 1, 1, 1 },

								NULL,
								&opt_aF
						      },
							  {
								OPT_NOT_MODAL,
								"o", "output",

								"Output the generated ROM image to the given <file>.",
								{ "<file>", 1, 1, 1 },

								NULL,
								&opt_ao
						      },
							  {
								OPT_NOT_MODAL,
								"n", "non-ez",

								"Generate a non-ez ROM",
								NULL_ARG_INFO,

								NULL,
								&opt_an
							  },
							  {
								OPT_NOT_MODAL,
								"L", "locale",

								"Generate a ROM for the given locale",
								{ "<country>", 1, 1, 1 },
								NULL,
								&opt_aL
							  },
							  {
								OPT_NOT_MODAL,
								"s", "small",

								"Include a small ROM composed of the given PRC files.  The size "
								"of the small ROM may be specified using the option modifier.  "
								"e.g. '-l:<size>' or '--small:<size>'.",
								{ "<file>", 1, 1, ARGS_INFINITE },

								as_subopts,
								&opt_as
						      },
							  {
								OPT_NOT_MODAL,
								"l", "large",

								"Include a large ROM composed of the given PRC files.  The size "
								"of the large ROM may be specified using the option modifier.  "
								"e.g. '-l:<size>' or '--large:<size>'.",
								{ "<file>", 1, 1, ARGS_INFINITE },

								al_subopts,
								&opt_al,
							  },
							  NULL_OPTS  };

static parsed_opts		x_subopts[]	= { {
								OPT_NOT_MODAL,
								"l", "large",

								"Extract PRC/databases from the large ROM.\nIf 1 or more "
								"<type.ctor> is provided, only extract PRC/databases which have "
								"a <type.ctor> in the provided list",
								{ "<type.ctor>", 1, 0, ARGS_INFINITE},

								NULL,
								&opt_xl,
							  },
							  {
								OPT_NOT_MODAL,
								"s", "small",

								"Extract PRC/databases from the small ROM.\nIf 1 or more "
								"<type.ctor> is provided, only extract PRC/databases which have "
								"a <type.ctor> in the provided list",
								{ "<type.ctor>", 1, 0, ARGS_INFINITE},

								NULL,
								&opt_xs,
						      },
							  {
								OPT_NOT_MODAL,
								"m", "meta-data",

								"Extract ROM metadata.  Files will be created with the "
								"given <basename>.",
								NULL_ARG_INFO,	//{ "<basename>", 1, 1, 1},

								NULL,
								&opt_xm,
						      },
						      NULL_OPTS  };

static parsed_opts		v_subopts[]	= { {
								OPT_NOT_MODAL,
								"l", "large",

								"View information about the large ROM.",
								NULL_ARG_INFO,

								NULL,
								&opt_vl,
							  },
							  {
								OPT_NOT_MODAL,
								"s", "small",

								"View information about the small ROM.",
								NULL_ARG_INFO,

								NULL,
								&opt_vs,
						      },
						      NULL_OPTS  };


static parsed_opts		opts[]		= {

							  { OPT_DEFAULT_ARGS,
								"i", "input",
								
								"Image(s) to process",
								{ "<image>", 1, 0, ARGS_INFINITE },

								NULL,
								&opt_files
							  },

							  /************************************************
							   * Modal options
		A				   */
							  {
								OPT_MODAL,
								"X", "rip",

								"RIP small and large PRCs from ROM and keep ROM metadata "
								"(useful for rebuilding a ROM with -B with a"
								" minimum of args, currently not implemented)",
								NULL_ARG_INFO,
								NULL,
								&opt_X
							  },
							  {
								OPT_MODAL,
								"B", "rebuild",

								"Rebuild a ROM from existing PRCs and metadata, "
								"currently not implemented",
								{ "<name of the original ROM>", 1, 1, 1 },
								NULL,
								&opt_B
							  },
							  {
								OPT_MODAL,
								"a", "assemble:create",

								"Assemble a new ROM from the given set of PRC database files "
								"(specified using the -l and/or -s suboption).  The ROM <version> "
								"may optionally be specified.",
								{ "<version>", 1, 0, 1 },

								a_subopts,
								&opt_a
						      },
							  {
								OPT_MODAL,
								"p", "PRC",

								"Read and interpret the given PRC/database image.",
								NULL_ARG_INFO,

								NULL,
								&opt_p
						      },
							  {
								OPT_MODAL,
								"r", "RAM",

								"Read and interpret the given RAM image (not fully implemented)",
								NULL_ARG_INFO,

								NULL,
								&opt_r
						      },
							  {
								OPT_MODAL,
								"v", "view",

								"View the given ROM image.",
								NULL_ARG_INFO,

								v_subopts,
								&opt_v
						      },
							  {
								OPT_MODAL,
								"x", "extractPRCs",

								"Extract all PRC/databases from the provided image <file>.",
								NULL_ARG_INFO,

								x_subopts,
								&opt_x
						      },

							  /************************************************
							   * Global/Non-modal options
							   */
							  {
								OPT_NOT_MODAL,
								"c", "showChunks",

								"Show all memory chunks",
								NULL_ARG_INFO,

								NULL,
								&opt_c
						      },
							  {
								OPT_NOT_MODAL,
								"d", "showPRC",

								"List all PRC/databases within the image",
								NULL_ARG_INFO,

								NULL,
								&opt_d
						      },
							  {
								OPT_NOT_MODAL,
								"D", "showPRCDetail",

								"Show the details of all PRC/databases within the image.  "
								"This option also accepts an option modifier which specifies the "
								"level of detail desired:"
								"\n.2:8.-D:1  is the same as '-d',"
								"\n.2:8.-D:2  will show the records/resources within a database,"
								"\n.2:8.-D:3  will display records/resources interpret the "
								"records/resources which are currently understood, dumping all "
								"others as RAW bytes,"
								"\n.2:8.-D:4  is the same as '-D:3' with a little more "
								"record/resource detail."
								"\n.2:8.-D:5  will dump all records/resources as RAW bytes."
								"\n.The default is '-D:2'.",
								NULL_ARG_INFO,

								NULL,
								&opt_D
						      },
							  {
								OPT_NOT_MODAL,
								"h", "showHeap",

								"Show the heap list and all heap headers",
								NULL_ARG_INFO,

								NULL,
								&opt_h
						      },
							  {
								OPT_NOT_MODAL,
								"H", "showHeadersOnly:headersOnly",

								"Show the card and storage headers only",
								NULL_ARG_INFO,

								NULL,
								&opt_H
						      },
							  {
								OPT_NOT_MODAL,
								"N", "showNV",

								"Show the System NV parameters (from the storage header)",
								NULL_ARG_INFO,

								NULL,
								&opt_N
						      },
							  {
								OPT_NOT_MODAL,
								"V", "Verbose",

								"Verbose output (equivalent to '-NDch')",
								NULL_ARG_INFO,

								NULL,
								&opt_V
						      },
							  {
								OPT_NOT_MODAL,
								NULL, "version",

								"Display the version of Romeo",
								NULL_ARG_INFO,

								NULL,
								&opt_version
						      },
						      NULL_OPTS  };


/*
 * Display a system error message of the form:
 * 	<user output> <system errno>: <system error message>
 */
void	SysError	(char*	fmt, ...)
{
	//extern const char*	sys_errlist[];
	//extern int			sys_nerr;
	//extern int			errno;
	va_list	ap;
	int		err	= errno;

	fprintf (stderr, "*** ");
	if (err != 0)
		fprintf (stderr, "ERROR: ");

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);

	if (err != 0)
	{
		char* errmsg = strerror (err);
		if ((err >= 0) && errmsg)
			fprintf (stderr, " %d: %s", err, errmsg);
		else
			fprintf (stderr, " %d: error number OUT OF RANGE", err);
	}

	fprintf (stderr, "\n");

	va_end(ap);
}

int	Version(FILE*	pOut)
{
	fprintf (pOut, "%s\n", VERSION);
	fflush (pOut);

	return 0;
}

void	Usage(char*	stat)
{
	int		idex;
	char*	prog	= strrchr(progName, '/');
	char*	rest	= "[options] <image>...";
	FILE*	pOut	= stdout;

	if (! prog)
		prog = progName;
	else
		prog++;

	if (stat && *stat)
		pOut = stderr;

	fprintf(pOut, "\nRomeo ");
	Version(pOut);

	if (stat && *stat)
		opts_fprintf(pOut, opts,
		             "\n*** ERROR: %s\n\nUsage: %s %s\n",
		             stat, prog, rest);
	else
		opts_fprintf(pOut, opts,
		             "\nUsage: %s %s\n", prog, rest);

	fprintf (pOut, " Available PalmOS versions are:\n   ");
	for (idex = 0; idex < numROMVers; idex++)
	{
		UInt16	major	= (ROMVers[idex].palmOSVer >> 8) & 0xFF;
		UInt16	minor	= ROMVers[idex].palmOSVer & 0xFF;

		fprintf (pOut, "%s%x.%x", (idex ? ", " : ""), major, minor);
	}
	fprintf (pOut, "\n\n");

	fprintf (pOut, " Available locales are:\n   ");
	for (idex = 0; idex < romNumLocales; idex++)
	{
		fprintf (pOut, "%s%s", (idex ? ", " : ""), Locales[idex].name);
	}
	fprintf (pOut, "\n\n");
	
	fflush (pOut);
}

int	Assemble	()
{
	/************************************************************
	 * Assemble a new ROM
	 */
	ROMPtr		pROM;
	int			hROMOut		= fileno(stdout);
	ROMVersion*	pVersion	= &(ROMVers[0]);
	char		error[128];
	int hROMIn = 0;
	
	if (opt_a.optarg_cnt > 0)
	{
		UInt16	major;
		UInt16	minor;
		UInt16	osver;

		sscanf(opt_a.optarg[0], "%hx.%hx", &major, &minor);
		osver = (major << 8) | minor;

		pVersion = LocateROMVerByOSVer(osver);
		if (! pVersion)
		{
			sprintf (error, "Unsupported PalmOS Version [%hx.%hx]",
					 major, minor);
			Usage(error);
			return(1);
		}
	}

	if (opt_aL.optidex)
	{
		Locale* pLocale = &(Locales[0]);
		pLocale = LocateLocaleByName (opt_aL.optarg[0]);
		pVersion->nvparams_localeLanguage = pLocale->language;
		pVersion->nvparams_localeCountry = pLocale->country;
	}

	if ((opt_al.optidex) && (opt_al.optmod))
	{
		// strtoul can automatically deal with
		// hex '0xa', octal '012', or decimal '123'
		//
		// (We could also do special processing and provide any base
		//  between 2 and 36 to strtoul...)
		UInt32	Size = strtoul(opt_al.optmod, NULL, 0);
		if (Size)
		{
			pVersion->big_cardSize = Size;
		}
		else
		{
			sprintf (error, "Please specify the large ROM size (-l:<size>) as an integer");
			Usage (error);
			return 1;
		}
	}

	if ((opt_as.optidex) && (opt_as.optmod))
	{
		// strtoul can automatically deal with
		// hex '0xa', octal '012', or decimal '123'
		//
		// (We could also do special processing and provide any base
		//  between 2 and 36 to strtoul...)
		UInt32	Size = strtoul(opt_as.optmod, NULL, 0);
		if (Size)
		{
			pVersion->small_cardSize = Size;
		}
		else
		{
			sprintf (error, "Please specify the small ROM size (-s:<size>) as an integer");
			Usage (error);
			return 1;
		}
	}

	if (opt_ar.optidex)
	{
		UInt32 romBase = strtoul (opt_ar.optarg[0], NULL, 0);
		if (romBase)
			pVersion->ROM_base = romBase;
		else
		{
			sprintf (error, "Please specify the rom base (-r <base>) as an integer");
			Usage(error);
			return 1;
		}
	}
	
	if (opt_ab.optidex)
	{
		UInt32 bigRomOffset = strtoul (opt_ab.optarg[0], NULL, 0);
		if (bigRomOffset)
			pVersion->card_bigROMOffset = bigRomOffset;
		else
		{
			sprintf (error, "Please specify the big rom offset (-b <offset>) as an integer");
			Usage(error);
			return 1;
		}
	}
		
	if (opt_af.optidex)
	{
		strncpy (pVersion->small_layout, opt_af.optarg[0], MAX_LAYOUT_LEN);
		strncpy (pVersion->small_addLayout, opt_af.optarg[0], MAX_LAYOUT_LEN);
	}

	if (opt_aF.optidex)
	{
		strncpy (pVersion->big_layout, opt_aF.optarg[0], MAX_LAYOUT_LEN);
		strncpy (pVersion->big_addLayout, opt_aF.optarg[0], MAX_LAYOUT_LEN);
	}

	if (opt_ao.optidex)
	{
		if ((hROMOut = open(opt_ao.optarg[0], O_RDWR | O_CREAT | O_TRUNC | O_BINARY, 0666)) < 0)
		{
			SysError("Cannot open ROM output file '%s'", opt_ao.optarg[0]);
			return(0);
		}
	}

	if (opt_aR.optidex)
	{
		/*
		 * Open the ROM file
		 */
		hROMIn = open(opt_aR.optarg[0], O_RDONLY | O_BINARY);
		if (hROMIn < 0)
		{
			SysError("Cannot open ROM file '%s'", opt_aR.optarg[0]);
			return(1);
		}
	}

	if (opt_as.optidex || hROMIn)
	{
		UInt16 flags = RT_SMALL;
		flags = flags | (opt_an.optidex ? RT_NON_EZ : 0);

		if (hROMIn)
		{
			pROM = ReadROM (hROMIn, flags);
		}
		else
		{
			UInt16 old_flags = 0;
			if (opt_asp.optidex)
			{
				old_flags = pVersion->heap_flags;
				pVersion->heap_flags = atoi (opt_asp.optarg[0]);
			}
			pROM = InitializeROM(pVersion, flags);
			if (old_flags)
				pVersion->heap_flags = old_flags;
		}
			
		if (pROM)
		{
			if (AddPRCs (pROM, opt_as.optarg_cnt, opt_as.optarg, 0) != opt_as.optarg_cnt)
			{
				SysError("Cannot add PRCs to small ROM");
				FreeROM (pROM);
				return (0);
			}

			//Output_ROM(pROM, (UInt32)(pROM->pROM),
			//           bShowNV, bShowHeap, bShowChunks,bShowDB);

			fprintf (stderr, "\n");

			if (opt_ash.optidex)
				pROM->pCard->hdrVersion = atoi (opt_ash.optarg[0]);
			if (! SetSystem(pROM))
			{
				SysError("Cannot set up small ROM boot code initialization vectors");
				FreeROM (pROM);
				return (0);
			}

			if (! WriteROM(pROM, hROMOut))
			{
				SysError("Cannot write small ROM");
			}
			fprintf (stderr, "\n");

			FreeROM(pROM);
		}
		else if (! hROMIn)
		{
			SysError("Cannot create small ROM");
		}
	}
		
	if (opt_al.optidex || hROMIn)
	{
		UInt16 flags = RT_LARGE;
		flags = flags | (opt_an.optidex ? RT_NON_EZ : 0);

		if (hROMIn)
		{
			pROM = ReadROM (hROMIn, flags);
		}
		else
		{
			if (opt_alp.optidex)
				pVersion->heap_flags = atoi (opt_alp.optarg[0]);
			pROM = InitializeROM(pVersion, flags);
		}

		if (pROM)
		{
			if (opt_as.optidex)
			{
				pROM->File_base = pROM->pVersion->card_bigROMOffset;
			}

			if (AddPRCs (pROM, opt_al.optarg_cnt, opt_al.optarg, 0) != opt_al.optarg_cnt)
			{
				SysError("Cannot add PRCs to large ROM\n"
				         "Try using -l:<size> to increase the available space");
				FreeROM (pROM);
				return (0);
			}
	
			//Output_ROM(pROM, (UInt32)(pROM->pROM),
			//           bShowNV, bShowHeap, bShowChunks,bShowDB);

			fprintf (stderr, "\n");

			if (opt_alh.optidex)
				pROM->pCard->hdrVersion = atoi (opt_alh.optarg[0]);
			if (opt_alp.optidex)
				pROM->pVersion->heap_flags = atoi (opt_alp.optarg[0]);
			if (! SetSystem(pROM))
			{
				SysError("Cannot set up large ROM boot code initialization vectors");
				FreeROM (pROM);
				return (0);
			}

			if (! WriteROM(pROM, hROMOut))
			{
				SysError("Cannot write large ROM");
			}
			fprintf (stderr, "\n");

			FreeROM(pROM);
		}
		else if (hROMIn)
		{
			SysError("Cannot create large ROM");
		}
	}

	return(0);
}

#if 0
int	ViewRAM()
{
	int		idex;
	UInt32	RRFlags			= 0;
	char	error[128];
	UInt16	bShowNV			= (UInt16)(opt_N.optidex !=0||opt_V.optidex!=0);
	UInt16	bShowHeap		= (UInt16)(opt_h.optidex !=0||opt_V.optidex!=0);
	UInt16	bShowChunks		= (UInt16)(opt_c.optidex !=0||opt_V.optidex!=0);
	UInt16	bShowDB			= 0;

	SET_D(bShowDB);


	if (! opt_files.optidex)
	{
		sprintf (error,"You must provide at least 1 file for processing.");
		Usage(error);
		return(1);

	}

	for (idex = 0; idex < opt_files.optarg_cnt; idex++)
	{
		ROMPtr	pRAM;
		int		hRAM;
		char*	pRAMName		= opt_files.optarg[idex];
		/*
		 * Open the RAM file
		 */
		hRAM = open(pRAMName, O_RDONLY | O_BINARY);
		if (hRAM < 0)
		{
			SysError("Cannot open RAM file '%s'", pRAMName);
			continue;
		}

		fprintf (stdout, "Processing '%s'\n", pRAMName);

		/************************************************************
		 * View a RAM image
		 */
		fprintf (stdout, "\n======= RAM Image =======\n");
		if ((pRAM = ReadRAM(hRAM, RRFlags)))
		{
			fprintf (stdout, "   PTR_base  [0x%08lX]\n", (UInt32)pRAM->pROM);
			fprintf (stdout, "   ROM_base  [0x%08lX]\n", pRAM->ROM_base);
			fprintf (stdout, "   Card_base [0x%08lX]\n", pRAM->Card_base);
			fprintf (stdout, "   File_base [0x%08lX]\n", pRAM->File_base);
			Output_ROM(pRAM, (UInt32)(pRAM->pROM),
			           bShowNV, bShowHeap, bShowChunks, bShowDB);

#if	0
			if (bExtract)
				WritePRCs(pROM, NULL, 0);
#endif

			FreeROM(pRAM);
		}
		else
		{
			SysError("Cannot read the RAM image from '%s'",
			         pRAMName);
		}

		close(hRAM);
	}

	return (0);
}
#endif

int	ViewPRC()
{
	/************************************************************
	 * View a PRC/database file
	 */
	PRCPtr		pPRC;
	int			idex;
	char		error[128];
	UInt16		bShowDB			= 0;

	SET_D(bShowDB);
	if (bShowDB)	bShowDB++;

	if (! opt_files.optidex)
	{
		sprintf (error,"You must provide at least 1 file for processing.");
		Usage(error);
		return(1);
	}

	for (idex = 0; idex < opt_files.optarg_cnt; idex++)
	{
		if ((pPRC = ReadPRC(opt_files.optarg[idex])))
		{
			Output_DatabaseHdr(NULL, (UInt32)pPRC->pDB + pPRC->nBytes, 
							   pPRC->pDB, (UInt32)pPRC->pDB, 0, bShowDB);
		}
		else
		{
			SysError("Cannot reading PRC '%s'", opt_files.optarg[idex]);
		}

		FreePRC(pPRC);
	}

	return(0);
}

int	Extract (int hROMIn, int bVerbose)
{
	int			idex;
	UInt32		RRFlags			= 0;
	UInt16		bShowNV			= (UInt16)(opt_N.optidex != 0 || opt_V.optidex !=0);
	UInt16		bShowHeap		= (UInt16)(opt_h.optidex != 0 || opt_V.optidex !=0);
	UInt16		bShowChunks		= (UInt16)(opt_c.optidex != 0 || opt_V.optidex !=0);
	UInt16		bExtract		= (UInt16)(opt_x.optidex != 0);
	UInt16		bShowDB			= 0;
	UInt16		bSmallROM		= 0;
	UInt16		bLargeROM		= 0;
	UInt16		bMetaData		= 0;
	ROMVersion*	smallVers		= NULL;
	ROMVersion*	largeVers		= NULL;
	ROMVersion*	mergeVers		= NULL;
	char*		metaBaseName	= NULL;
	char		error[128];

	SET_D(bShowDB);

	if (! opt_files.optidex)
	{
		sprintf (error, "You must provide at least 1 file for processing.");
		Usage(error);
		return(1);

	}

	if (opt_x.optidex)
	{
		bSmallROM = (UInt16)(opt_xs.optidex != 0);
		bLargeROM = (UInt16)(opt_xl.optidex != 0);
		bMetaData = (UInt16)(opt_xm.optidex != 0);
		if (bMetaData)
		{
			if (opt_xm.optarg)
				metaBaseName = opt_xm.optarg[0];
		}

#if	0
		if ((! bSmallROM) && (! bLargeROM))
		{
			bSmallROM = 1;
			bLargeROM = 1;
		}
#endif
	}
	else if (opt_v.optidex)
	{
		bSmallROM = (UInt16)(opt_vs.optidex != 0);
		bLargeROM = (UInt16)(opt_vl.optidex != 0);
	}

	if (! bSmallROM && ! bLargeROM && ! bMetaData)
	{
		bSmallROM = 1;
		bLargeROM = 1;
	}

	for (idex = 0; idex < opt_files.optarg_cnt; idex++)
	{
		ROMPtr	pROM;
		int		hROM;
		char*	pROMName		= opt_files.optarg[idex];

		/*
		 * Open the ROM file
		 */
		if (hROMIn >= 0)
			hROM = hROMIn;
		else
			hROM = open(pROMName, O_RDONLY | O_BINARY);
		if (hROM < 0)
		{
			SysError("Cannot open ROM file '%s'", pROMName);
			return(1);
		}

		fprintf (stdout, "Processing '%s'\n", pROMName);

		/*
		 * Process the Small ROM if requested.
		 */
		if (bSmallROM || bMetaData)
		{
			/************************************************************
			 * View/Extract a small ROM
			 */
			if (bVerbose)
				fprintf (stdout, "\n======= Small ROM =======\n");
			if ((pROM = ReadROM(hROM, RRFlags | RT_SMALL)))
			{
				if (bVerbose)
				{
					//fprintf (stdout, "   PTR_base  [0x%08lX]\n", (UInt32)pROM->pROM);
					fprintf (stdout, "   ROM_base  [0x%08lX]\n", pROM->ROM_base);
					fprintf (stdout, "   Card_base [0x%08lX]\n", pROM->Card_base);
					fprintf (stdout, "   File_base [0x%08lX]\n", pROM->File_base);
					Output_ROM(pROM, (UInt32)(pROM->pROM),
					           bShowNV, bShowHeap, bShowChunks, bShowDB);
				}

				if (bExtract && bSmallROM)
				{
					TypeCtorPtr	pTCList		= NULL;
					UInt32		nEntries	= 0;

					if ((opt_xs.optarg_cnt > 0) &&
						((pTCList = StrList2TypeCtorList(opt_xs.optarg,
														 opt_xs.optarg_cnt))
						            != NULL))
					{
						nEntries = opt_xs.optarg_cnt;
					}

					WritePRCs(pROM, pTCList, nEntries);

					if (nEntries)
						free(pTCList);
				}

				if (bMetaData)
				{
					// Save the version data
					smallVers = CollectVersion(pROM);
				}

				FreeROM(pROM);
			}
			else
			{
				SysError("No Small ROM exists in '%s'", pROMName);
			}
		}


		if (bLargeROM || bMetaData)
		{
			/************************************************************
			 * View/Extract a large ROM
			 */
			if (bVerbose)
				fprintf (stdout, "\n======= Large ROM =======\n");
			if ((pROM = ReadROM(hROM, RRFlags | RT_LARGE)))
			{
				if (bVerbose)
				{
					//fprintf (stdout, "   PTR_base  [0x%08lX]\n", (UInt32)pROM->pROM);
					fprintf (stdout, "   ROM_base  [0x%08lX]\n", pROM->ROM_base);
					fprintf (stdout, "   Card_base [0x%08lX]\n", pROM->Card_base);
					fprintf (stdout, "   File_base [0x%08lX]\n", pROM->File_base);

					//Card_base - ROM_base == 0x8000

					Output_ROM(pROM, (UInt32)(pROM->pROM) - pROM->File_base,
									         //(pROM->Card_base - pROM->ROM_base),
					           bShowNV, bShowHeap, bShowChunks, bShowDB);
				}

				if (bExtract && bLargeROM)
				{
					TypeCtorPtr	pTCList		= NULL;
					UInt32		nEntries	= 0;

					if ((opt_xl.optarg_cnt > 0) &&
						((pTCList = StrList2TypeCtorList(opt_xl.optarg,
														 opt_xl.optarg_cnt))
						            != NULL))
					{
						nEntries = opt_xl.optarg_cnt;
					}

					WritePRCs(pROM, pTCList, nEntries);

					if (nEntries)
						free(pTCList);
				}

				if (bMetaData)
				{
					// Save the version data
					largeVers = CollectVersion(pROM);
				}
				
				FreeROM(pROM);
			}
			else
			{
				SysError("No Large ROM exists in '%s'",
				         pROMName);
			}
		}

		close(hROM);

		if (bMetaData)
		{
			char	filename[MAX_FILENAME_LEN];
			int		hMeta			= 0;

			if (! metaBaseName)
			{
				metaBaseName = strrchr(pROMName, '/');
				if (! metaBaseName)
					metaBaseName = pROMName;
				else
					metaBaseName++;
			}
			
			// Merge the version information we collected and save it off
			mergeVers = MergeVersions (smallVers, largeVers);
			if (! mergeVers)
			{
				SysError("Failed to merge version information.");
				return (1);
			}

			// Write out the params data to the files...

			
			// Write out the meta-data file
			sprintf (filename, "%s.romeo", metaBaseName);

			hMeta = open (filename, O_RDWR | O_CREAT | O_TRUNC | O_BINARY, 0644);
			if (hMeta < 0)
			{
				SysError("Failed to open metadata file %s.", filename);
				return (1);
			}
			if (! Write_MetaInfo (hMeta, mergeVers))
			{
				SysError("Error writing metadata file %s.", filename);
				return (1);
			}
			close (hMeta);

			FreeROMVersion(mergeVers);
			FreeROMVersion(smallVers);
			FreeROMVersion(largeVers);
		}
		
	}

	return (0);
}

/*
 * RIP a ROM - extract all PRCs along with META information that may be
 *             used to properly reconstruct the ROM
 */
int RIP()
{
	char		absoluteROMPath[1024];
	char		pwd[1024];
	int			hIn;
	char*		ROMShortName	= NULL;
	UInt32		idex;
	struct stat	Stat;

	getcwd (pwd, sizeof (pwd));

	snprintf (absoluteROMPath, sizeof (absoluteROMPath),
	          "%s/.romeo", getenv ("HOME"));

	memset(&Stat, 0, sizeof(Stat));
	if (stat(absoluteROMPath, &Stat) || (! S_ISDIR(Stat.st_mode)))
	{
		/*
		 * Missing .romeo directory - try to create it!
		 */
		if (mkdir ((const char*)absoluteROMPath, O_CREAT | 0700))
		{
			SysError("Error creating directory '%s'", absoluteROMPath);
			return (-1);
		}
	}

	for (idex = 0; idex < opt_files.optarg_cnt; idex++)
	{
		ROMShortName = strrchr (opt_files.optarg[idex], '/');
		if (! ROMShortName)
			ROMShortName = opt_files.optarg[idex];
		else
			ROMShortName++;

		snprintf (absoluteROMPath, sizeof (absoluteROMPath),
		          "%s/.romeo/%s", getenv ("HOME"), ROMShortName);
		if (mkdir ((const char*) absoluteROMPath, O_CREAT | 0700))
		{
			if (errno != EEXIST)
			{
				SysError("Error creating directory '%s'", absoluteROMPath);
				continue;
			}
		}

		/********************************************************************
		 * Process ROM
		 */
		hIn = open (opt_files.optarg[idex], O_RDONLY | O_BINARY);
		if (hIn < 0)
		{
			SysError("Error opening ROM file '%s'", opt_files.optarg[idex]);
			continue;
		}
	
		opt_x.optidex  = 1;		// Extract
		opt_xs.optidex = 1;		//     small ROM
		opt_xl.optidex = 1;		//     large ROM
		opt_xm.optidex = 1;		//     meta data

		if (chdir (absoluteROMPath))
		{
			SysError("Cannot chdir into the holding directory '%s'",
					 absoluteROMPath);
		}
		else
		{
			Extract (hIn, 0);
		}

		close   (hIn);
	}

	if (chdir (pwd))
	{
		SysError("Cannot chdir to '%s'", pwd);
		return (-1);
	}

	return (0);
}

/*
 * Given a ROM name, look for the META data and PRC files generated by RIP
 * and rebuild that ROM.
 */
int	Rebuild	()
{
	char	ROMPath[1024];
	char	pwd[1024];
	int		hIn;
	int		idex;
	int		jdex;
	int		nPRC;
	int		hROMOut		= fileno(stdout);
	char*	pHOME		= getenv("HOME");

	if (opt_B.optarg_cnt < 1)
	{
		Usage("You must provide a ROM image name.");
		return(-1);
	}

	getcwd (pwd, sizeof (pwd));

	for (idex = 0; idex < opt_B.optarg_cnt; idex++)
	{
		char**		pPRCList;
		ROMVersion*	pVersion;
		ROMPtr		pROM;
		char*		ROMShortName	= strrchr (opt_B.optarg[idex], '/');
		if (! ROMShortName)
			ROMShortName = opt_B.optarg[idex];
		else
			ROMShortName++;

		pPRCList = NULL;
		pVersion = NULL;
		pROM     = NULL;

		/*
		 * Change to the META directory...
		 */
		snprintf (ROMPath, sizeof (ROMPath), "%s/.romeo/%s",
		          pHOME, ROMShortName);
		if (chdir (ROMPath))
		{
			SysError("Cannot chdir to '%s'", pwd);
			continue;
		}

		/*******************************************************************
		 * 1) make sure that we have everything we need in
		 *    the target directory
		 */
		snprintf (ROMPath,sizeof(ROMPath), "%s.romeo", ROMShortName);
		if ((hIn = open(ROMPath, O_RDONLY | O_BINARY)) < 0)
		{
			SysError("Cannot open META data file '%s'", ROMShortName);
			continue;
		}

		pVersion = Read_MetaInfo(hIn);
		close(hIn);
		if (! pVersion)
		{
			SysError("Cannot read META data file '%s'", ROMShortName);
			continue;
		}

		/*******************************************************************
		 * 2) Create the ROM output file
		 */
		snprintf (ROMPath,sizeof(ROMPath), "%s/%s", pwd, ROMShortName);
		if ((hROMOut = open(ROMPath, O_RDWR | O_CREAT | O_TRUNC | O_BINARY, 0666)) < 0)
		{
			SysError("Cannot open ROM output file '%s'", ROMShortName);
			goto skip;
		}

		/*******************************************************************
		 * 3) Construct and output the small-ROM
		 */
		pROM = InitializeROM(pVersion, RT_SMALL | pVersion->heap_flags);
		if (! pROM)
		{
			SysError("Cannot create small ROM");
			goto skip;
		}

		/* Place the names of our PRCs in a character array */
		nPRC = 0;
		for (jdex = 0; (pVersion->small_PRCList[jdex].type    != 0) &&
		               (pVersion->small_PRCList[jdex].creator != 0); jdex++)
		{
			nPRC++;
		}

		pPRCList = (char**)malloc(nPRC * sizeof(char*));
		if (! pPRCList)
		{
			SysError("Cannot allocate space for the PRC list");
			goto skip;
		}

		for (jdex = 0; jdex < nPRC; jdex++)
		{
			pPRCList[jdex] = pVersion->small_PRCList[jdex].dbName;
		}

		if (AddPRCs (pROM, nPRC, pPRCList, 0) != nPRC)
		{
			SysError("Cannot add PRCs to small ROM");
			goto skip;
		}

		if (! SetSystem(pROM))
		{
			SysError("Cannot set up small ROM boot code initialization vectors");
			goto skip;
		}

		if (! WriteROM(pROM, hROMOut))
		{
			SysError("Cannot write small ROM");
		}

		/* Make sure we don't free our version information... */
		pROM->pVersion = NULL;
		FreeROM(pROM);

		/*******************************************************************
		 * 4) Construct and output the large-ROM
		 * 
		 * Place the names of our PRCs in a character array
		 */
		pROM = InitializeROM(pVersion, RT_LARGE | pVersion->heap_flags);
		if (! pROM)
		{
			SysError("Cannot create small ROM");
			goto skip;
		}

		/* Place the names of our PRCs in a character array */
		nPRC = 0;
		for (jdex = 0; (pVersion->big_PRCList[jdex].type    != 0) &&
		               (pVersion->big_PRCList[jdex].creator != 0); jdex++)
		{
			nPRC++;
		}

		pPRCList = (char**)malloc(nPRC * sizeof(char*));
		if (! pPRCList)
		{
			SysError("Cannot allocate space for the PRC list");
			goto skip;
		}

		for (jdex = 0; jdex < nPRC; jdex++)
		{
			pPRCList[jdex] = pVersion->big_PRCList[jdex].dbName;
		}

		if (AddPRCs (pROM, nPRC, pPRCList, 0) != nPRC)
		{
			SysError("Cannot add PRCs to large ROM");
			goto skip;
		}

		if (! WriteROM(pROM, hROMOut))
		{
			SysError("Cannot write large ROM");
		}

skip:
		if (pROM)
		{
			if (pROM->pVersion == pVersion)
				pVersion = NULL;
			FreeROM(pROM);
		}
		if (pVersion)	FreeROMVersion(pVersion);
		if (pPRCList)	free(pPRCList);
	}

	/*
	 * Return to the original directory
	 */
	if (chdir (pwd))
	{
		SysError("Cannot chdir to '%s'", pwd);
		return (-1);
	}

	return (0);

#if	0
	fprintf (stderr, "*** Rebuild is not yet implemented\n");
	return (-1);

	//
	//-----------------------------------------------------------------------
	//
	char		absoluteROMPath[1024];

	for (idex = 0; idex < opt_files.optarg_cnt; idex++)
	{
		ROMShortName = strrchr (opt_files.optarg[idex], '/');
		if (! ROMShortName)
			ROMShortName = opt_files.optarg[idex];
		else
			ROMShortName++;

		snprintf (absoluteROMPath, sizeof (absoluteROMPath),
		          "%s/.romeo/%s", getenv ("HOME"), ROMShortName);
		if (mkdir ((const char*) absoluteROMPath, O_CREAT | 0700))
		{
			if (errno != EEXIST)
			{
				SysError("Error creating directory '%s'", absoluteROMPath);
				continue;
			}
		}

		/********************************************************************
		 * Process ROM
		 */
		hIn = open (opt_files.optarg[idex], O_RDONLY | O_BINARY);
		if (hIn < 0)
		{
			SysError("Error opening ROM file '%s'", opt_files.optarg[idex]);
			continue;
		}
	
		opt_x.optidex  = 1;		// Extract
		opt_xs.optidex = 1;		//     small ROM
		opt_xl.optidex = 1;		//     large ROM
		opt_xm.optidex = 1;		//     meta data

		if (chdir (absoluteROMPath))
		{
			SysError("Cannot chdir into the holding directory '%s'",
					 absoluteROMPath);
		}
		else
		{
			Extract (hIn, 0);
		}

		close   (hIn);
	}

	//
	//-----------------------------------------------------------------------
	//
	if (opt_as.optidex || hROMIn)
	{
		UInt16 flags = RT_SMALL;
		flags = flags | (opt_an.optidex ? RT_NON_EZ : 0);

		if (hROMIn)
		{
			pROM = ReadROM (hROMIn, flags);
		}
		else
		{
			UInt16 old_flags = 0;
			if (opt_asp.optidex)
			{
				old_flags = pVersion->heap_flags;
				pVersion->heap_flags = atoi (opt_asp.optarg[0]);
			}
			pROM = InitializeROM(pVersion, flags);
			if (old_flags)
				pVersion->heap_flags = old_flags;
		}
			
		if (pROM)
		{
			if (AddPRCs (pROM, opt_as.optarg_cnt, opt_as.optarg, 0) != opt_as.optarg_cnt)
			{
				SysError("Cannot add PRCs to small ROM");
				FreeROM (pROM);
				return (0);
			}

			//Output_ROM(pROM, (UInt32)(pROM->pROM),
			//           bShowNV, bShowHeap, bShowChunks,bShowDB);

			fprintf (stderr, "\n");

			if (opt_ash.optidex)
				pROM->pCard->hdrVersion = atoi (opt_ash.optarg[0]);
			if (! SetSystem(pROM))
			{
				SysError("Cannot set up small ROM boot code initialization vectors");
				FreeROM (pROM);
				return (0);
			}

			if (! WriteROM(pROM, hROMOut))
			{
				SysError("Cannot write small ROM");
			}
			fprintf (stderr, "\n");

			FreeROM(pROM);
		}
		else if (! hROMIn)
		{
			SysError("Cannot create small ROM");
		}
	}
		
	if (opt_al.optidex || hROMIn)
	{
		UInt16 flags = RT_LARGE;
		flags = flags | (opt_an.optidex ? RT_NON_EZ : 0);

		if (hROMIn)
		{
			pROM = ReadROM (hROMIn, flags);
		}
		else
		{
			if (opt_alp.optidex)
				pVersion->heap_flags = atoi (opt_alp.optarg[0]);
			pROM = InitializeROM(pVersion, flags);
		}

		if (pROM)
		{
			if (opt_as.optidex)
			{
				pROM->File_base = pROM->pVersion->card_bigROMOffset;
			}

			if (AddPRCs (pROM, opt_al.optarg_cnt, opt_al.optarg, 0) != opt_al.optarg_cnt)
			{
				SysError("Cannot add PRCs to large ROM\n"
				         "Try using -l:<size> to increase the available space");
				FreeROM (pROM);
				return (0);
			}
	
			//Output_ROM(pROM, (UInt32)(pROM->pROM),
			//           bShowNV, bShowHeap, bShowChunks,bShowDB);

			fprintf (stderr, "\n");

			if (opt_alh.optidex)
				pROM->pCard->hdrVersion = atoi (opt_alh.optarg[0]);
			if (opt_alp.optidex)
				pROM->pVersion->heap_flags = atoi (opt_alp.optarg[0]);
			if (! SetSystem(pROM))
			{
				SysError("Cannot set up large ROM boot code initialization vectors");
				FreeROM (pROM);
				return (0);
			}

			if (! WriteROM(pROM, hROMOut))
			{
				SysError("Cannot write large ROM");
			}
			fprintf (stderr, "\n");

			FreeROM(pROM);
		}
		else if (hROMIn)
		{
			SysError("Cannot create large ROM");
		}
	}
#endif
}

int	ViewROM()
{
	return Extract (-1, 1);
}

int	main	(int	argc,
			 char*	argv[])
{
	char*	stat;

	progName = argv[0];

	if (argc < 2)
	{
		Usage(NULL);
		return(1);
	}

	if ((stat = opts_parse(opts, argc, argv)) != NULL)
	{
		Usage(stat);
		return(1);
	}

	if (opt_version.optidex)
		return Version(stdout);
	else if (opt_a.optidex)
		return Assemble();
	else if (opt_X.optidex)
		return RIP();
	else if (opt_B.optidex)
		return Rebuild();

#if 0
	else if (opt_r.optidex)
		return ViewRAM();
#endif
	else if (opt_p.optidex)
		return ViewPRC();
	else if (opt_x.optidex)
		return Extract (-1, 1);
	else
	{
		if (! opt_v.optidex)
			opt_v.optidex = -1;

		return ViewROM();
	}

	return(1);
}
