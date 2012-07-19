CC		= gcc
CFLAGS	= -Wall -g -pg -Wno-multichar -m32
LDFLAGS	= -L. -lromeo -pg -m32
AR		= ar crv

LIB		= libromeo.a
LIBOBJS	= utils.o byte_swap.o relocate.o translate.o Crc.o rom.o \
		  extract.o assemble.o meta_info.o

all: romeo

romeo: $(LIB) romeo.o output.o output_resource.o opts_parse.o

read_prc_data: $(LIB) read_prc_data.o

$(LIB): $(LIBOBJS)
	$(AR) $(LIB) $(LIBOBJS)

meta_info: meta_info.c rom.o utils.o
	$(CC) $(CFLAGS) -c -DTEST meta_info.c -o meta_info-t.o
	$(CC) $(CFLAGS) meta_info-t.o rom.o utils.o -o meta_info

depend: FORCE
	@makedepend -Y *.c > /dev/null 2>/dev/null

tags: FORCE
	ctags-exuberant    *.[chCH] *.cpp 2>/dev/null
	ctags-exuberant -e *.[chCH] *.cpp 2>/dev/null

clean: FORCE
	rm -f romeo *.[oa] *~ *.exe

FORCE:

opts_parse: opts_parse.c opts_parse.h
	$(CC) $(CFLAGS) -c -DTEST opts_parse.c -o opts_parse-t.o
	$(CC) $(CFLAGS) opts_parse-t.o -o opts_parse

################################################################################
## Following are dependencies (created via the 'depend' rule)
# DO NOT DELETE


assemble.o: assemble.h byte_swap.h Crc.h DataMgr.h DataPrv.h extract.h
assemble.o: MemoryPrv.h os_structs.h OverlayMgr.h rom.h SystemPrv.h
assemble.o: SystemResources.h translate.h types.h
byte_swap.o: byte_swap.h DataMgr.h DataPrv.h MemoryPrv.h os_structs.h
byte_swap.o: OverlayMgr.h rom.h SystemPrv.h types.h
Crc.o: Crc.h DataMgr.h DataPrv.h MemoryPrv.h os_structs.h OverlayMgr.h
Crc.o: SystemPrv.h types.h
extract.o: byte_swap.h Crc.h DataMgr.h DataPrv.h extract.h MemoryPrv.h
extract.o: os_structs.h OverlayMgr.h rom.h SystemPrv.h translate.h types.h
meta_info.o: DataMgr.h DataPrv.h MemoryPrv.h os_structs.h OverlayMgr.h rom.h
meta_info.o: SystemPrv.h types.h
opts_parse.o: opts_parse.h
output.o: byte_swap.h DataMgr.h DataPrv.h MemoryPrv.h os_structs.h output.h
output.o: output_resource.h OverlayMgr.h rom.h SystemPrv.h SystemResources.h
output.o: types.h
output_resource.o: byte_swap.h DataMgr.h DataPrv.h MemoryPrv.h os_structs.h
output_resource.o: output.h output_resource.h OverlayMgr.h rom.h SystemPrv.h
output_resource.o: SystemResources.h types.h
output_resource.o: utils.h
read_prc_data.o: byte_swap.h DataMgr.h DataPrv.h extract.h MemoryPrv.h
read_prc_data.o: os_structs.h OverlayMgr.h rom.h SystemPrv.h SystemResources.h
read_prc_data.o: types.h
read_prc_data.o: utils.h
relocate.o: byte_swap.h DataMgr.h DataPrv.h MemoryPrv.h os_structs.h
relocate.o: OverlayMgr.h relocate.h rom.h SystemPrv.h types.h
romeo.o: assemble.h DataMgr.h DataPrv.h extract.h MemoryPrv.h meta_info.h
romeo.o: opts_parse.h os_structs.h output.h OverlayMgr.h rom.h SystemPrv.h
romeo.o: types.h
romeo.o: version.h
rom.o: 100.rom.h 200.rom.h 300.rom.h 310.rom.h 325.rom.h 330.rom.h 350.rom.h
rom.o: DataMgr.h DataPrv.h MemoryPrv.h os_structs.h OverlayMgr.h relocate.h
rom.o: rom.h SystemPrv.h SystemResources.h types.h
sizes.o: DataMgr.h DataPrv.h MemoryPrv.h os_structs.h OverlayMgr.h rom.h
sizes.o: SystemPrv.h types.h
translate.o: byte_swap.h DataMgr.h DataPrv.h MemoryPrv.h os_structs.h
translate.o: OverlayMgr.h relocate.h rom.h SystemPrv.h translate.h types.h
utils.o: byte_swap.h DataMgr.h DataPrv.h MemoryPrv.h os_structs.h OverlayMgr.h
utils.o: rom.h SystemPrv.h SystemResources.h types.h
