PLATFORM = os4
# PLATFORM = win32
# PLATFORM = linux
# PLATFORM = aros
# PLATFORM = aros64
# PLATFORM = ppc-aros
# PLATFORM = morphos-cross
# PLATFORM = beos
# PLATFORM = ppc-beos-cross
# PLATFORM = gp2x
# PLATFORM = alpha-linux-cross
# PLATFORM = ia64-linux-cross
# PLATFORM = amd64-linux-cross
# PLATFORM = s390-linux-cross
# PLATFORM = s390x-linux-cross
# PLATFORM = arm-linux-cross
# PLATFORM = sparc-linux-cross
# PLATFORM = psp
# PLATFORM = hppa-linux-cross
# PLATFORM = ppc-linux-cross
# PLATFORM = m68k-linux-cross
# PLATFORM = mips-linux-cross
# PLATFORM = mipsel-linux-cross
# PLATFORM = sh3-linux-cross
# PLATFORM = sh4-linux-cross
# PLATFORM = ppc64-linux-cross
# PLATFORM = sparc64-linux-cross
# PLATFORM = avr32-linux-cross
# PLATFORM = bsdi
# PLATFORM = qnx6
# PLATFORM = solaris
# PLATFORM = skyos
# PLATFORM = openserver5
# PLATFORM = openserver6
# PLATFORM = unixware7
# PLATFORM = mint
# PLATFORM = i386-freebsd7
# PLATFORM = amd64-freebsd7-cross
# PLATFORM = sparc64-freebsd7-cross
# PLATFORM = ia64-freebsd6-cross
# PLATFORM = alpha-freebsd5-cross
# PLATFORM = riscos-cross
# PLATFORM = hppa-hpux
# PLATFORM = ia64-hpux
# PLATFORM = zaurus-cross
# PLATFORM = syllable
# PLATFORM = netbsd
# PLATFORM = alpha-netbsd4-cross
# PLATFORM = amd64-netbsd4-cross
# PLATFORM = hppa-netbsd4-cross
# PLATFORM = m68k-netbsd4-cross
# PLATFORM = mipseb-netbsd4-cross
# PLATFORM = mipsel-netbsd4-cross
# PLATFORM = ns32k-netbsd2-cross
# PLATFORM = ppc-netbsd4-cross
# PLATFORM = sh3eb-netbsd4-cross
# PLATFORM = sh3le-netbsd4-cross
# PLATFORM = sh5le-netbsd3-cross
# PLATFORM = sparc-netbsd4-cross
# PLATFORM = sparc64-netbsd4-cross
# PLATFORM = vax-netbsd4-cross
# PLATFORM = arm-netbsd4-cross
# PLATFORM = m68010-netbsd4-cross
# PLATFORM = xbox
# PLATFORM = i386-openbsd
# PLATFORM = alpha-openbsd-cross
# PLATFORM = amd64-openbsd-cross
# PLATFORM = arm-openbsd-cross
# PLATFORM = hppa-openbsd-cross
# PLATFORM = ppc-openbsd-cross
# PLATFORM = mips64-openbsd-cross
# PLATFORM = sh4-openbsd-cross
# PLATFORM = sparc64-openbsd-cross
# PLATFORM = m68k-openbsd-cross
# PLATFORM = sparc-openbsd-cross
# PLATFORM = aix
# PLATFORM = irix
# PLATFORM = irix_marq
# PLATFORM = pandora

####### DEFAULT SETTINGS HERE #######

CFLAGS = -Wall -O2
LFLAGS = 

CC = gcc
DEBUGLIB =
TARGET = planethively

FASTMATH = -ffast-math

####### PLATFORM SPECIFIC STUFF HERE #######

# Amiga OS4
ifeq ($(PLATFORM),os4)
CFLAGS += -mcrt=newlib -ggdb -D__SW_SURFACE__
LFLAGS += -use-dynld -lSDL -mcrt=newlib -ggdb -lm
endif

# I386 AROS
ifeq ($(PLATFORM),aros)
CFLAGS += -D__SW_SURFACE__
LFLAGS += -lSDL -lm
CC = i686-aros-gcc
endif

# PPC AROS
ifeq ($(PLATFORM),ppc-aros)
CFLAGS += -D__SW_SURFACE__
LFLAGS += -lSDL -lm
CC = ppc-linux-aros-gcc
endif

# AROS64
ifeq ($(PLATFORM),aros64)
CFLAGS += -D__SW_SURFACE__
LFLAGS += -lSDL -lm
CC = x86_64-pc-aros-gcc
endif

# PPC MORPHOS cross compile
ifeq ($(PLATFORM),morphos-cross)
CFLAGS += -noixemul -D__SW_SURFACE__
LFLAGS += -noixemul -lSDL -lm
CC = ppc-morphos-gcc
endif

# PPC MORPHOS compile
ifeq ($(PLATFORM),morphos)
CFLAGS += -noixemul -D__SW_SURFACE__
LFLAGS += -noixemul -lSDL -lm
CC = gcc
endif

# RISCOS cross compile
ifeq ($(PLATFORM),riscos-cross)
CFLAGS += -D__SW_SURFACE__ -D__EMBED_TUNES__
LFLAGS += -lSDL -lm
CC = $(GCCSDKDIR)/bin/gcc
endif

# Windows 32bit
ifeq ($(PLATFORM),win32)
CFLAGS += -Dmain=SDL_main -D__HW_ONLY_WINDOWED__ -D__EMBED_TUNES__
LFLAGS += -mwindows -lmingw32 -lSDLmain -lSDL -lm
TARGET = planethively.exe
endif

# HPPA HPUX
ifeq ($(PLATFORM),hppa-hpux)
LFLAGS += -lSDL -lm
endif

# IA64 HPUX
ifeq ($(PLATFORM),ia64-hpux)
LFLAGS += -lSDL -lm -lrt
endif

# Zaurus cross compile
ifeq ($(PLATFORM),zaurus-cross)
LFLAGS += -lSDL -lm -lqpe -lqte -lpthread
endif

# BSDI
ifeq ($(PLATFORM),bsdi)
LFLAGS += -lSDL -lm -lXdpms -lpthread
endif

# Solaris
ifeq ($(PLATFORM),solaris)
LFLAGS += -lSDL -lm
endif

# SkyOS
ifeq ($(PLATFORM),skyos)
LFLAGS += -lSDL -lm
endif

# Syllable
ifeq ($(PLATFORM),syllable)
LFLAGS += -lSDL -lm
endif

# Xbox
ifeq ($(PLATFORM),xbox)
CFLAGS = -g -D__NO_ARGS__ -D__EMBED_TUNES__ -D__NO_MOUSE__ -D__XBOX__ -DENABLE_XBOX -DDISABLE_CDROM -std=gnu99 -ffreestanding -nostdlib -fno-builtin -fno-exceptions -mno-cygwin -march=i386 -I/usr/local/openxdk/i386-pc-xbox/include -I/usr/local/openxdk/include
LFLAGS += -nostdlib -Wl,--file-alignment,0x20 -Wl,--section-alignment,0x20 -shared --Wl,--entry,_WinMainCRTStartup -Wl,--strip-all -L/usr/local/openxdk/i386-pc-xbox/lib -L/usr/local/openxdk/lib -lSDL -lopenxdk -lhal -lc -lm -lhal -lusb -lc -lxboxkrnl
endif

# Openserver 5.x
ifeq ($(PLATFORM),openserver5)
LFLAGS += -lSDL -lm -lX11 -lXext -lsocket
endif

# Openserver 6.x
ifeq ($(PLATFORM),openserver6)
LFLAGS += -lSDL -lm -lthread
endif

# Unixware 7
ifeq ($(PLATFORM),unixware7)
LFLAGS += -lSDL -lm -lX11 -lXext -lsocket
endif

# I386 FreeBSD 7.x
ifeq ($(PLATFORM),i386-freebsd7)
LFLAGS += -lSDL -lm
endif

# AMD64 FreeBSD 7.x cross compile
ifeq ($(PLATFORM),amd64-freebsd7-cross)
LFLAGS += -lSDL -lm
CC = amd64-freebsd7-gcc
endif

# Sparc64 FreeBSD 7.x cross compile
ifeq ($(PLATFORM),sparc64-freebsd7-cross)
LFLAGS += -lSDL -lm
CC = sparc64-freebsd7-gcc
endif

# IA64 FreeBSD 6.x cross compile
ifeq ($(PLATFORM),ia64-freebsd-cross)
LFLAGS += -lSDL -lm -lpthread
CC = ia64-freebsd6-gcc
endif

# Alpha FreeBSD 5.x cross compile
ifeq ($(PLATFORM),alpha-freebsd-cross)
LFLAGS += -lSDL-1.1 -lm -lthr
CC = alpha-freebsd5-gcc
endif

# I386 NetBSD 4.x
ifeq ($(PLATFORM),netbsd)
LFLAGS += -lSDL -lm
endif

# Alpha NetBSD 4.x cross compile
ifeq ($(PLATFORM),alpha-netbsd4-cross)
LFLAGS += -lSDL -lm
CC = alpha-netbsd4-gcc
endif

# AMD64 NetBSD 4.x cross compile
ifeq ($(PLATFORM),amd64-netbsd4-cross)
LFLAGS += -lSDL -lm
CC = amd64-netbsd4-gcc
endif

# HPPA NetBSD 4.x cross compile
ifeq ($(PLATFORM),hppa-netbsd4-cross)
LFLAGS += -lSDL -lm
CC = hppa-netbsd4-gcc
endif

# M68K NetBSD 4.x cross compile
ifeq ($(PLATFORM),m68k-netbsd4-cross)
LFLAGS += -lSDL -lm
CC = m68k-netbsdelf4-gcc
endif

# MipsEB NetBSD 4.x cross compile
ifeq ($(PLATFORM),mipseb-netbsd4-cross)
LFLAGS += -lSDL -lm
CC = mipseb-netbsd4-gcc
endif

# MipsEL NetBSD 4.x cross compile
ifeq ($(PLATFORM),mipsel-netbsd4-cross)
LFLAGS += -lSDL -lm
CC = mipsel-netbsd4-gcc
endif

# NS32K NetBSD 2.x cross compile
ifeq ($(PLATFORM),ns32k-netbsd2-cross)
CFLAGS += __SW_SURFACE__
LFLAGS += -lSDL -lpthread -laa -lncurses -lm
CC = ns32k-pc532k-netbsd-gcc
endif

# PPC NetBSD 4.x cross compile
ifeq ($(PLATFORM),ppc-netbsd4-cross)
LFLAGS += -lSDL -lm
CC = ppc-netbsd4-gcc
endif

# SH3EB NetBSD 4.x cross compile
ifeq ($(PLATFORM),sh3eb-netbsd4-cross)
LFLAGS += -lSDL -lm
CC = sh3eb-netbsdelf4-gcc
endif

# SH3LE NetBSD 4.x cross compile
ifeq ($(PLATFORM),sh3le-netbsd4-cross)
LFLAGS += -lSDL -lm
CC = sh3le-netbsdelf4-gcc
endif

# SH5LE NetBSD 3.x cross compile
ifeq ($(PLATFORM),sh5le-netbsd3-cross)
LFLAGS += -lSDL -lm -lrt -lpthread -lusbhid -lossaudio
CC = sh5le-netbsdelf3-gcc
endif

# Sparc NetBSD 4.x cross compile
ifeq ($(PLATFORM),sparc-netbsd4-cross)
LFLAGS += -lSDL -lm
CC = sparc-netbsdelf4-gcc
endif

# Sparc64 NetBSD 4.x cross compile
ifeq ($(PLATFORM),sparc64-netbsd4-cross)
LFLAGS += -lSDL -lm
CC = sparc64-netbsd4-gcc
endif

# Vax NetBSD 4.x cross compile
ifeq ($(PLATFORM),vax-netbsd4-cross)
LFLAGS += -lSDL -lm -lX11 -lXext -lrt -lpthread -lusbhid -lossaudio
CC = vax-netbsdelf4-gcc
endif

# Arm NetBSD 4.x cross compile
ifeq ($(PLATFORM),arm-netbsd4-cross)
LFLAGS += -lSDL -lm
CC = arm-netbsdelf4-gcc
endif

# M68010 NetBSD 4.x cross compile
ifeq ($(PLATFORM),m68010-netbsd4-cross)
LFLAGS += -lSDL -lrt -lpthread -laa -lusbhid -lncurses -lossaudio -lm
CC = m68010--netbsdelf-gcc
endif

# I386 OpenBSD
ifeq ($(PLATFORM),i386-openbsd)
LFLAGS += -SDL -lm -lX11 -lpthread
endif

# Alpha OpenBSD cross compile
ifeq ($(PLATFORM),alpha-openbsd-cross)
LFLAGS += -lSDL -lm -lX11 -lXext -lXau -lXdmcp -lusbhid -lpthread
CC = /usr/cross/alpha/usr/bin/gcc
endif

# AMD64 OpenBSD cross compile
ifeq ($(PLATFORM),amd64-openbsd-cross)
LFLAGS += -lSDL -lm -lX11 -lXext -lXau -lXdmcp -lusbhid -lpthread
CC = /usr/cross/amd64/usr/bin/gcc
endif

# Arm OpenBSD cross compile
ifeq ($(PLATFORM),arm-openbsd-cross)
LFLAGS += -lSDL -lm -lX11 -lXext -lXau -lXdmcp -lusbhid -lpthread
CC = /usr/cross/arm/usr/bin/gcc
endif

# HPPA OpenBSD cross compile
ifeq ($(PLATFORM),hppa-openbsd-cross)
LFLAGS += -lSDL -lm -lX11 -lXext -lXau -lXdmcp -lusbhid -lpthread
CC = /usr/cross/hppa/usr/bin/gcc
endif

# PPC OpenBSD cross compile
ifeq ($(PLATFORM),ppc-openbsd-cross)
LFLAGS += -lSDL -lm -lX11 -lXext -lXau -lXdmcp -lusbhid -lpthread
CC = /usr/cross/ppc/usr/bin/gcc
endif

# Mips64 OpenBSD cross compile
ifeq ($(PLATFORM),mips64-openbsd-cross)
LFLAGS += -lSDL -lm -lX11 -lXext -lXau -lXdmcp -lusbhid -lpthread
CC = /usr/cross/mips64/usr/bin/gcc
endif

# SH4 OpenBSD cross compile
ifeq ($(PLATFORM),sh4-openbsd-cross)
LFLAGS += -lSDL -lm -lX11 -lXext -lXau -lXdmcp -lusbhid -lpthread
CC = /usr/cross/sh4/usr/bin/gcc
endif

# Sparc64 OpenBSD cross compile
ifeq ($(PLATFORM),sparc64-openbsd-cross)
LFLAGS += -lSDL -lm -lX11 -lXext -lXau -lXdmcp -lusbhid -lpthread
CC = /usr/cross/sparc64/usr/bin/gcc
endif

# M68K OpenBSD cross compile
ifeq ($(PLATFORM),m68k-openbsd-cross)
LFLAGS += -static -lSDL -lm -lX11 -lXext -lpthread -lusbhid
CC = /usr/cross/m68k/usr/bin/gcc
endif

# Sparc OpenBSD cross compile
ifeq ($(PLATFORM),sparc-openbsd-cross)
LFLAGS += -lSDL -lm -lpthread
CC = sparc--openbsd-gcc
endif

# AIX
ifeq ($(PLATFORM),aix)
LFLAGS += -lSDL -lm -lstdc++
endif

# IRIX
ifeq ($(PLATFORM),irix)
LFLAGS += -lSDL -lm
endif

# SGI IRIX
ifeq ($(PLATFORM),irix_marq)
CFLAGS += `sdl-config --cflags` -D__SURFACE32__ -ffast-math
LFLAGS += `sdl-config --libs` -lm
endif

# I386 Linux
ifeq ($(PLATFORM),linux)
LFLAGS += -lSDL -lm
endif

# QNX 6.x
ifeq ($(PLATFORM),qnx6)
LFLAGS += -lSDL -lm -lph -lasound
endif

# Atari Mint
ifeq ($(PLATFORM),mint)
LFLAGS += -lSDL -lm -lgem
endif

# Alpha Linux cross compile
ifeq ($(PLATFORM),alpha-linux-cross)
LFLAGS += -lSDL -lm -lX11 -lXext -lpthread -ldl -lasound
CC = alpha-linux-gnu-gcc
endif

# IA64 Linux cross compile
ifeq ($(PLATFORM),ia64-linux-cross)
LFLAGS += -lSDL -lm -lX11 -lXext -lpthread -ldl -lasound
CC = ia64-linux-gnu-gcc
endif

# AMD64/X86_64 Linux cross compile
ifeq ($(PLATFORM),amd64-linux-cross)
LFLAGS += -lSDL -lm -lX11 -lXext -lpthread -ldl -lasound
CC = amd64-linux-gnu-gcc
endif

# S390 Linux cross compile
ifeq ($(PLATFORM),s390-linux-cross)
LFLAGS += -lSDL -lm -lX11 -lXext -lpthread -ldl -lasound
CC = s390-linux-gnu-gcc
endif

# S390x Linux cross compile
ifeq ($(PLATFORM),s390x-linux-cross)
LFLAGS += -lSDL -lm -lX11 -lXext -lpthread -ldl -lasound
CC = s390x-linux-gnu-gcc
endif

# ARM Linux cross compile
ifeq ($(PLATFORM),arm-linux-cross)
LFLAGS += -lSDL -lm -lX11 -lXext -lpthread -ldl
CC = arm-linux-gnu-gcc
endif

# Sparc Linux cross compile
ifeq ($(PLATFORM),sparc-linux-cross)
LFLAGS += -lSDL -lm -lX11 -lXext -lpthread -ldl -lasound
CC = sparc-linux-gnu-gcc
endif

# Sparc64 Linux cross compile
ifeq ($(PLATFORM),sparc64-linux-cross)
LFLAGS += -lSDL -lm -lX11 -lXext -lpthread -ldl -lasound
CC = sparc64-linux-gnu-gcc
endif

# SH3 Linux cross compile
ifeq ($(PLATFORM),sh3-linux-cross)
LFLAGS += -lSDL -lm -lX11 -lXext -lpthread -ldl -lasound -lgcc
CC = sh3-linux-gnu-gcc
endif

# SH4 Linux cross compile
ifeq ($(PLATFORM),sh4-linux-cross)
LFLAGS += -lSDL -lm -lX11 -lXext -lpthread -ldl -lasound -lgcc
CC = sh4-linux-gnu-gcc
endif

# HPPA Linux cross compile
ifeq ($(PLATFORM),hppa-linux-cross)
LFLAGS += -lSDL -lm
CC = hppa-linux-gnu-gcc
endif

# PPC Linux cross compile
ifeq ($(PLATFORM),ppc-linux-cross)
LFLAGS += -lSDL -lm
CC = ppc-linux-gnu-gcc
endif

# PPC64 Linux cross compile
ifeq ($(PLATFORM),ppc64-linux-cross)
LFLAGS += -lSDL -lm -lX11 -lXext -lpthread -ldl -lasound -lX11-xcb
CC = ppc64-linux-gnu-gcc
endif

# M68K Linux cross compile
ifeq ($(PLATFORM),m68k-linux-cross)
LFLAGS += -lSDL -lm
CC = m68k-linux-gnu-gcc
endif

# MIPS Linux cross compile
 ifeq ($(PLATFORM),mips-linux-cross)
LFLAGS += -lSDL -lm
CC = mips-linux-gnu-gcc
endif

# MIPSEL Linux cross compile
ifeq ($(PLATFORM),mipsel-linux-cross)
LFLAGS += -lSDL -lm
CC = mipsel-linux-gnu-gcc
endif

# AVR32 Linux cross compile
ifeq ($(PLATFORM),avr32-linux-cross)
LFLAGS += -lSDL -lm
CC = avr32-linux-gcc
endif

# PSP
ifeq ($(PLATFORM),psp)
CFLAGS += -D__EMBED_TUNES__ -D__NO_MOUSE__ -DDEFAULT_WIDTH=480 -DDEFAULT_HEIGHT=272 -D__FIXED_RES__
LFLAGS += -L/usr/local/pspdev/psp/sdk/lib -lSDL  -lm -lpspdebug -lpspgu -lpspctrl -lpspge -lpspdisplay -lpsphprm -lpspsdk -lpsprtc -lpspaudio -lc -lpspuser -lpsputility -lpspkernel -lpspnet_inet
CC = psp-gcc
endif

# I386 BeOS
ifeq ($(PLATFORM),beos)
CFLAGS += -D__SW_SURFACE__
LFLAGS += -lSDL
endif

# PPC BeOS
ifeq ($(PLATFORM),ppc-beos-cross)
CFLAGS = -inclpaths nosys -O2 -D__SW_SURFACE__
LFLAGS += -L/boot/develop/lib/ppc /boot/develop/lib/ppc/glue-noinit.a /boot/develop/lib/ppc/init_term_dyn.o /boot/develop/lib/ppc/start_dyn.o -lbe -lroot -lSDL
CC = mwccppc
endif

# GP2X (untested for now)
ifeq ($(PLATFORM),gp2x)
CFLAGS += -static -DDEFAULT_WIDTH=320 -D__GP2X__ -DDEFAULT_HEIGHT=240 -D__FIXED_RES__ -D__SW_SURFACE__ -D__NO_MOUSE__ -fomit-frame-pointer -mcpu=arm920t -msoft-float
LFLAGS += -static -lSDL -lpthread -lm -msoftfloat -mcpu=arm920t
CC=arm-linux-gcc
endif

# Pandora with ptitSeb's Code::Blocks
ifeq ($(PLATFORM),pandora)
CFLAGS = -Ofast -pipe -mcpu=cortex-a8 -mfpu=neon -ftree-vectorize -mfloat-abi=softfp -fsingle-precision-constant -fuse-ld=gold -fuse-linker-plugin -fdiagnostics-color=auto -fsigned-char -mneon-for-64bits -mvectorize-with-neon-double -D__EMBED_TUNES__
LFLAGS = -lSDL -lm
CC = /mnt/utmp/codeblocks/usr/bin/gcc
endif

####### SHOULDN'T HAVE TO CHANGE THIS STUFF #######

all: $(TARGET)

# To strip the exe under os4 when using dynamically linked SDL
# use "make strip_os4"
strip_os4:
	strip `nm -u planethively | sed "s/U /-K/"` planethively -R.comment

strip_aros:
	i686-aros-strip --strip-unneeded --remove-section .comment planethively

strip_ppc_aros:
	ppc-linux-aros-strip --strip-unneeded --remove-section .comment planethively

xbox_xbe:
	cxbe -OUT:planethively.xbe -DUMPINFO:hively.txt -TITLE:PlanetHively planethively.exe

$(TARGET): main.o hvl_replay.o shapes.o render.o fonts.o text.o scrollbox.o
	$(CC) -o $(TARGET) main.o hvl_replay.o shapes.o render.o fonts.o text.o scrollbox.o $(LFLAGS)

main.o: main.c main.h hvl_replay.h render.h shapes.h text.h
	$(CC) -c main.c -o main.o $(CFLAGS)

shapes.o: shapes.c shapes.h
	$(CC) -c shapes.c -o shapes.o $(CFLAGS) $(FASTMATH)

render.o: render.c render.h shapes.h hvl_replay.h types.h text.h
	$(CC) -c render.c -o render.o $(CFLAGS) $(FASTMATH)

text.o: text.c text.h shapes.h
	$(CC) -c text.c -o text.o $(CFLAGS) $(FASTMATH)

hvl_replay.o: hvl_replay.c hvl_replay.h types.h
	$(CC) -c hvl_replay.c -o hvl_replay.o $(CFLAGS)

fonts.o: fonts.c
	$(CC) -c fonts.c -o fonts.o $(CFLAGS)

scrollbox.o: scrollbox.c
	$(CC) -c scrollbox.c -o scrollbox.o $(CFLAGS)

clean:
	rm -f *.o
