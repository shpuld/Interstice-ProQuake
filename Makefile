# Build type.
# Must be one of the types in VALID_VIDEOS.
VALID_VIDEOS	= HARDWARE
VIDEO			= HARDWARE

VAILD_MP3LIBS   = MP3HARDWARE
MP3LIB          = MP3HARDWARE

PSP_FW_VERSION  = 400

# If GPROF is used it builds an ELF, hence the EBOOT needs to be in GAME150
#USE_GPROF		= 1

# Site specific variables.
INSTALL_DIR		= $(PSP_MOUNT)/PSP/GAME/Interstice

# Compiler specific variables.
PSPSDK		= $(shell psp-config --pspsdk-path)
ifeq ($(PSPSDK),)
$(error PSPSDK wasn't set)
endif
PSPLIBSDIR		= $(PSPSDK)/..

# Project specific variables.
VERSION		= 5.10 UNIFIED
SRC_DIR		= source
OBJ_DIR		= build
TARGET		= $(OBJ_DIR)/Quake
PSP_EBOOT_TITLE	= Interstice ProQuake v$(VERSION)
PSP_EBOOT_ICON	= assets/eboot/icon.png
PSP_EBOOT_PIC1	= assets/eboot/pic1.png
PSP_EBOOT_SFO	= $(OBJ_DIR)/PARAM.SFO
PSP_EBOOT		= $(OBJ_DIR)/EBOOT.PBP
DIST_DIR		= dist
DIST_FILES		= readme.html gpl.txt Quake/EBOOT.PBP
ZIP_FILE		= Interstice_v$(VERSION).zip

ifeq ($(USE_GPROF),1)
GPROF_LIBS      = -lpspprof
GPROF_FLAGS		= -pg -DPROFILE
else
BUILD_PRX		= 1
endif

# Object files used regardless of video back end.
COMMON_OBJS = \
	$(OBJ_DIR)/psp/battery.o \
	$(OBJ_DIR)/psp/VramExt.o \
	$(OBJ_DIR)/psp/input.o \
	$(OBJ_DIR)/psp/main.o \
	$(OBJ_DIR)/psp/math.o \
	$(OBJ_DIR)/psp/sound.o \
	$(OBJ_DIR)/psp/system.o \
	$(OBJ_DIR)/psp/sysmem_module.o \
	$(OBJ_DIR)/psp/network.o \
	$(OBJ_DIR)/psp/network_psp.o \
	$(OBJ_DIR)/psp/network_gethost.o \
	$(OBJ_DIR)/psp/fs_fnmatch.o \
	\
	$(OBJ_DIR)/cl_chase.o \
	$(OBJ_DIR)/cl_demo.o \
	$(OBJ_DIR)/cl_input.o \
	$(OBJ_DIR)/cl_main.o \
	$(OBJ_DIR)/cl_parse.o \
	$(OBJ_DIR)/cl_sbar.o \
	$(OBJ_DIR)/cl_tent.o \
	$(OBJ_DIR)/cl_view.o \
	$(OBJ_DIR)/cmd.o \
	$(OBJ_DIR)/common.o \
	$(OBJ_DIR)/console.o \
	$(OBJ_DIR)/crc.o \
	$(OBJ_DIR)/cvar.o \
	$(OBJ_DIR)/compat.o \
	$(OBJ_DIR)/host.o \
	$(OBJ_DIR)/host_cmd.o \
	$(OBJ_DIR)/keys.o \
	$(OBJ_DIR)/mathlib.o \
	$(OBJ_DIR)/menu.o \
	$(OBJ_DIR)/net_dgrm.o \
	$(OBJ_DIR)/net_loop.o \
	$(OBJ_DIR)/net_main.o \
	$(OBJ_DIR)/net_vcr.o \
	$(OBJ_DIR)/pr_cmds.o \
	$(OBJ_DIR)/pr_edict.o \
	$(OBJ_DIR)/pr_exec.o \
	$(OBJ_DIR)/r_part.o \
	$(OBJ_DIR)/snd_dma.o \
	$(OBJ_DIR)/snd_mem.o \
	$(OBJ_DIR)/snd_mix.o \
	$(OBJ_DIR)/sv_main.o \
	$(OBJ_DIR)/sv_move.o \
	$(OBJ_DIR)/sv_phys.o \
	$(OBJ_DIR)/sv_user.o \
	$(OBJ_DIR)/sv_world.o \
	$(OBJ_DIR)/version.o \
	$(OBJ_DIR)/wad.o \
	$(OBJ_DIR)/zone.o

# Object files used only under hardware video.
HARDWARE_VIDEO_ONLY_OBJS = \
	$(OBJ_DIR)/psp/clipping.o \
	$(OBJ_DIR)/psp/gu_psp.o \
	$(OBJ_DIR)/psp/gu_draw.o \
	$(OBJ_DIR)/psp/gu_entity_fragment.o \
	$(OBJ_DIR)/psp/gu_fog.o \
	$(OBJ_DIR)/psp/gu_light.o \
	$(OBJ_DIR)/psp/gu_main.o \
	$(OBJ_DIR)/psp/gu_mesh.o \
	$(OBJ_DIR)/psp/gu_misc.o \
	$(OBJ_DIR)/psp/gu_model.o \
	$(OBJ_DIR)/psp/gu_screen.o \
	$(OBJ_DIR)/psp/gu_surface.o \
	$(OBJ_DIR)/psp/gu_warp.o \
	$(OBJ_DIR)/psp/gu_vertex_lighting.o \
	$(OBJ_DIR)/psp/vram.o \
	$(OBJ_DIR)/psp/pspDveManager.o
HARDWARE_VIDEO_ONLY_FLAGS = -DPSP_HARDWARE_VIDEO

# Object files used only under hardware mp3.
MP3HARDWARE_MP3LIB_ONLY_OBJS = \
	$(OBJ_DIR)/psp/cd.o \
	$(OBJ_DIR)/psp/mp3.o
MP3HARDWARE_MP3LIB_ONLY_FLAGS = -DPSP_MP3HARDWARE_MP3LIB

# Set up the object files depending on the build.
ifeq ($($(VIDEO)_VIDEO_ONLY_OBJS),)
$(error Unrecognised VIDEO type '$(VIDEO)'. It must be one of $(VALID_VIDEOS))
endif

ifeq ($($(MP3LIB)_MP3LIB_ONLY_OBJS),)
$(error Unrecognised MP3LIB type '$(MP3LIB)'. It must be one of $(VALID_MP3LIBS))
endif

OBJS	= $($(VIDEO)_VIDEO_ONLY_OBJS) $($(MP3LIB)_MP3LIB_ONLY_OBJS) $(COMMON_OBJS) $(GPROF_OBJS)

# Compiler flags.
CFLAGS	= -ffast-math -Ofast -G0 $(GPROF_FLAGS) -Wall $(GPROF_FLAGS) -Wno-trigraphs -Winline -DPSP $($(VIDEO)_VIDEO_ONLY_FLAGS) $($(MP3LIB)_MP3LIB_ONLY_FLAGS) -g
CXXFLAGS = -fno-rtti -Wcast-qual
ASFLAGS = $(CFLAGS) -c

# Libs.
GU_LIBS 	= -lpspgum_vfpu -lpspvfpu -lpspgu
AUDIO_LIBS	= -lpspaudiolib -lpspaudio -lpspaudiocodec source/psp/m33libs/libpspkubridge.a
MISC_LIBS	= -lpsprtc -lpsppower source/psp/misclibs/libpspmath.a
STD_LIBS	= -lstdc++ -lm -lc
LIBS		= $(GPROF_LIBS) $(GU_LIBS) $(AUDIO_LIBS) $(MISC_LIBS) $(STD_LIBS) -lpspwlan -lpspnet_adhoc -lpspnet_adhocctl

# What else to clean.
EXTRA_CLEAN	= $(foreach FILE,$(ZIP_FILE) Quake/EBOOT.PBP,$(DIST_DIR)/$(FILE))

# All target.
all: $(PSP_EBOOT)

# What is an install?
install: $(INSTALL_DIR)/EBOOT.PBP

# How to install.
$(INSTALL_DIR)/EBOOT.PBP: $(PSP_EBOOT)
	@echo Installing $(PSP_EBOOT) to $@...
	@-mkdir -p $(dir $@)
	@cp $(PSP_EBOOT) $@

# How to build the distribution.
distro: clean $(ZIP_FILE)
	@echo Distribution prepared.

$(DIST_DIR)/Quake/EBOOT.PBP: $(PSP_EBOOT)
	@echo Copying $< to $@...
	@-mkdir -p $(dir $@)
	@cp $< $@

$(ZIP_FILE): $(foreach FILE,$(DIST_FILES),$(DIST_DIR)/$(FILE))
	@echo Creating $(DIST_DIR)/$(ZIP_FILE)...
	@-rm -rf $(ZIP_FILE)
	@cd $(DIST_DIR) && zip -r -q -9 -o $(ZIP_FILE) $(DIST_FILES)

# How to compile an S file.
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.S
	@echo $(notdir $<)
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@

# How to compile a C file.
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@echo $(notdir $<)
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@

# How to compile a C++ file.
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo $(notdir $<)
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) -c $< -o $@

# Use the standard PSPSDK build stuff.
include $(PSPSDK)/lib/build.mak

ifeq ($(BUILD_PRX),1)
OBJ_TYPE	= prx
else
OBJ_TYPE	= elf
endif
