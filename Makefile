#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#
PROJECT_NAME := template

EXTRA_COMPONENT_DIRS := $(CURDIR)/../../../components

EXCLUDE_COMPONENTS := max7219 mcp23x17 led_strip max31865 ls7366r max31855

include $(IDF_PATH)/make/project.mk



