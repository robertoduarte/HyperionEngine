ifeq ($(strip $(YAUL_INSTALL_ROOT)),)
  $(error Undefined YAUL_INSTALL_ROOT (install root directory))
endif

include $(YAUL_INSTALL_ROOT)/share/build.pre.mk

# Required for library usage
include $(YAUL_INSTALL_ROOT)/share/build.mic3d.mk

SH_PROGRAM:= hyperion_engine_demo
SH_SRCS:= main.cxx 
# $(patsubst ./%,%,$(shell find src/meshes/ -name '*.c')) 

SH_CXXFLAGS+= -std=c++23 -O3 -I.
SH_LDFLAGS+= $(MIC3D_LDFLAGS)

IP_VERSION:= V1.000
IP_RELEASE_DATE:= 20160101
IP_AREAS:= JTUBKAEL
IP_PERIPHERALS:= JAMKSTD
IP_TITLE:= MIC 3D
IP_MASTER_STACK_ADDR:= 0x06004000
IP_SLAVE_STACK_ADDR:= 0x06001E00
IP_1ST_READ_ADDR:= 0x06004000
IP_1ST_READ_SIZE:= 0

include $(YAUL_INSTALL_ROOT)/share/build.post.iso-cue.mk
