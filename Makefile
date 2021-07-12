.PHONY = all clean install uninstall

#定义项目路径（主Makefile路径）
PRJ_DIR = $(shell pwd)
#定义头文件路径
DIR_INC = $(PRJ_DIR)
#定义源文件路径
DIR_SRC = $(PRJ_DIR)
#定义库文件路径
DIR_LIB = $(PRJ_DIR)
#定义编译中间文件输出路径
DIR_OBJ = $(PRJ_DIR)/obj
#定义编译最终目标文件输出路径
DIR_BUILD = $(PRJ_DIR)/build
#定义编译最终输出目标
TARGET          = s_ring_buffer_test
BUILD_TARGET    = ${DIR_BUILD}/${TARGET}
#定义交叉编译链
CROSS_COMPILE   =
AS              = $(CROSS_COMPILE)as
LD              = $(CROSS_COMPILE)ld
CC              = $(CROSS_COMPILE)gcc
CPP 	        = $(CROSS_COMPILE)g++
AR              = $(CROSS_COMPILE)ar
NM              = $(CROSS_COMPILE)nm
STRIP 	        = $(CROSS_COMPILE)strip
RANLIB 	        = $(CROSS_COMPILE)ranlib
#定义本地工具
CP              := cp
RM              := rm
MKDIR           := mkdir
SED             := sed
FIND            := find
MKDIR           := mkdir
XARGS           := xargs
#定义源文件和对应的.o文件
C_SRC           = $(foreach subdir,$(DIR_SRC),$(wildcard $(subdir)/*.c))
vpath %.c $(sort $(dir $(C_SRC)))
CPP_SRC         = $(foreach subdir,$(DIR_SRC),$(wildcard $(subdir)/*.cpp))
vpath %.cpp $(sort $(dir $(CPP_SRC)))
BUILD_OBJ       = $(subst $(PRJ_DIR),$(DIR_OBJ),$(patsubst %.c,%.o,$(C_SRC)))
BUILD_OBJ       += $(subst $(PRJ_DIR),$(DIR_OBJ),$(patsubst %.cpp,%.o,$(CPP_SRC)))
#所有.d依赖文件放入BUILD_DEPS
BUILD_DEPS = $(patsubst %.o,%.d,$(BUILD_OBJ))
#定义编译符号
FLAGS_BASE      = -O2 -g -fno-omit-frame-pointer -Wall -rdynamic -MD $(foreach dir,$(DIR_INC),-I$(dir))
C_FLAGS_BASE    = -std=c11
CPP_FLAGS_BASE  = -std=c++11
C_FLAGS         += $(C_FLAGS_BASE) $(FLAGS_BASE)
CPP_FLAGS       += $(CPP_FLAGS_BASE) $(FLAGS_BASE)
ARFLAGS         = rc
LDFLAGS         = -ldl -lrt -lm -lpthread
LDFLAGS         += -Wl,--gc-sections
XLDFLAGS        = -Xlinker "-(" $(LDFLAGS) -Xlinker "-)"
LDLIBS          += -L$(DIR_LIB)

all: $(BUILD_TARGET)
-include $(BUILD_DEPS)

#编译最终目标对象
$(BUILD_TARGET): $(BUILD_OBJ)
	@[ ! -d $(dir $(subst $(PRJ_DIR),$(DIR_BUILD),$@)) ] & $(MKDIR) -p $(dir $(subst $(PRJ_DIR),$(DIR_BUILD),$@))
	@$(CPP) $(subst $(PRJ_DIR),$(DIR_OBJ),$^) $(CPP_FLAGS) $(XLDFLAGS) -o $@ $(LDLIBS)
	@$(STRIP) --strip-unneeded $(BUILD_TARGET)
	@echo "	"
	@echo "make done."
	@echo "	"

#编译.o到DIR_OBJ路径
$(DIR_OBJ)%.o: $(PRJ_DIR)%.c
	@[ ! -d $(dir $(subst $(PRJ_DIR),$(DIR_OBJ),$@)) ] & $(MKDIR) -p $(dir $(subst $(PRJ_DIR),$(DIR_OBJ),$@))
	@$(CC) $(C_FLAGS) -o $(subst $(PRJ_DIR),$(DIR_OBJ),$@) -c $<
$(DIR_OBJ)%.o: $(PRJ_DIR)%.cpp
	@[ ! -d $(dir $(subst $(PRJ_DIR),$(DIR_OBJ),$@)) ] & $(MKDIR) -p $(dir $(subst $(PRJ_DIR),$(DIR_OBJ),$@))
	@$(CPP) $(CPP_FLAGS) -o $(subst $(PRJ_DIR),$(DIR_OBJ),$@) -c $<

clean:
	@$(RM) -rf $(DIR_OBJ)/*
	@$(RM) -f $(BUILD_TARGET)

