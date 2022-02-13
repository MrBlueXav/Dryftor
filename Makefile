# Project Name
TARGET = Dryftor

# Sources
CPP_SOURCES = Dryftor.cpp
CPP_SOURCES += \
chorusFD.cpp \
delay.cpp \
drifter.cpp \
phaser.cpp \
random.cpp \
sinetable.cpp \
podController.cpp

# Library Locations
LIBDAISY_DIR = ../../libDaisy
DAISYSP_DIR = ../../DaisySP

# Core location, and generic makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile

