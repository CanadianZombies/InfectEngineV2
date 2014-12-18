#####################################################################################
#                    Infected City powered by InfectEngine                          #
#            InfectEngine is powered by CombatMUD Core Infrastructure               #
#####################################################################################
# InfectEngine Copyright (c) 2010-2014 David Simmerson                              #
# CombatMUD Copyright (c) 2007-2014 David Simmerson                                 #
#                                                                                   #
# Permission is hereby granted, A, to any person obtaining a copy                   #
# of this software and associated documentation files (the "Software"), to deal     #
# in the Software without restriction, including without limitation the rights      #
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell         #
# copies of the Software, and to permit persons to whom the Software is             #
# furnished to do so, subject to the following conditions:                          #
#                                                                                   #
# The above copyright notice and this permission notice shall be included in        #
# all copies or substantial portions of the Software.                               #
#                                                                                   #
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR        #
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,          #
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE       #
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER            #
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,     #
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN         #
# THE SOFTWARE.                                                                     #
#####################################################################################

#####################################################################################
# Pretty makefile colours (yeah, cheesy, but I like pretty things)
# we must define MK_COLOURS, else wise, we won't use our colours
# if your system does not accept colour-codes, then change MK_COLOURS To 0.
MK_COLOURS      =       0

ifeq ($(MK_COLOURS), 1)
COLOUR_NORMAL       = \e[0;00m
COLOUR_DRED         = \e[0;31m
COLOUR_DGREEN       = \e[0;32m
COLOUR_DBROWN       = \e[0;33m
COLOUR_DBLUE        = \e[0;34m
COLOUR_DPURPLE      = \e[0;35m
COLOUR_DCYAN        = \e[0;36m
COLOUR_LRED         = \e[1;31m
COLOUR_LGREEN       = \e[1;32m
COLOUR_LYELLOW      = \e[1;33m
COLOUR_LBLUE  	    = \e[1;34m
COLOUR_LMAGENTA     = \e[1;35m
COLOUR_LGREY        = \e[0;37m
COLOUR_LWHITE       = \e[1;37m
COLOUR_LCYAN 	    = \e[1;36m
else
COLOUR_NORMAL       =
COLOUR_DRED         =
COLOUR_DGREEN       =
COLOUR_DBROWN       =
COLOUR_DBLUE        =
COLOUR_DPURPLE      =
COLOUR_DCYAN        =
COLOUR_LRED         =
COLOUR_LGREEN       =
COLOUR_LYELLOW	    =
COLOUR_LBLUE        =
COLOUR_LMAGENTA     =
COLOUR_LGREY        =
COLOUR_LWHITE	    =
COLOUR_LCYAN        =
endif

#####################################################################################
# DIRECTORIES
BIN_DIR    = Bin
BACKUP_DIR = Backups
CMD_DIR    = Code/commands
H_DIR      = Code/headers
O_DIR      = Code/obj

#####################################################################################
# COMPILER SPECIFICS
COMPILER      = g++

#####################################################################################
# FLAGS and LIBRARIES
PROFILER      = -O -ggdb
COMPILE_FLAGS = -Wall -Wwrite-strings ${PROFILER} -I$(H_DIR)
LIBRARY_FLAGS = ${PROFILER}
LIBRARIES     = -lcrypt -ldl

#####################################################################################
# VERSION SUPPORT: allow us to control our revisions of the executable being built.
# name of our version file
VERSION_FILE            = Code/.version

# Ensure we are built properly with the correct data
BUILD_DATE              = $(shell date +'%Y%m%d')
BUILD_NUM               = $(shell cat $(VERSION_FILE))
BUILD_VERSION		= "ALPHA"

# define our version (within the compiler flags)
COMPILE_FLAGS          += -DBUILD_DATE=$(BUILD_DATE) -DBUILD_NUM=$(BUILD_NUM)

#####################################################################################
# define our backup filename : this is broken down into multiple blocks as there was an issue with
# using long names / groups : this error may of been compiler specific, but this was the fix.
# so to retain functionality of the backup, this is our goal
BACKUP_NAME_PT1        = "backup-$(BUILD_DATE)"
BACKUP_NAME_PT2        = "-$(BUILD_NUM).tgz"

# put both groups together in our final attempt to create a unified backup
BACKUP_NAME            = "$(BACKUP_NAME_PT1)$(BACKUP_NAME_PT2)"

#####################################################################################
#So we can see our defines hopefully within our crash log
COMPILED_STR            = '"${COMPILE_FLAGS}"'
CMP_STR                 = -DCOMPILED_WITH=${COMPILED_STR}

#####################################################################################
# Name our Engine
ENGINE = InfectEngine

#####################################################################################
# Generate a list of our required OBJECT files (.o)
C_FILES                         = ${wildcard Code/*.c}
CPP_FILES                       = ${wildcard Code/*.cpp}
CMD_FILES			= ${wildcard $(CMD_DIR)/*.cpp}
H_FILES 			= $(wildcard $(H_DIR)/*.h)

OBJECT_FILES			= $(addprefix $(O_DIR)/, $(patsubst %.c, %.o, ${C_FILES}))
OBJECT_FILES			+= $(addprefix $(O_DIR)/, $(patsubst %.cpp, %.o, ${CPP_FILES}))
OBJECT_FILES			+= $(addprefix $(O_DIR)/, $(patsubst %.cpp, %.o, ${CMD_FILES}))

## OLD STYLE
#OBJECT_FILES                    = $(patsubst %.c, $(O_DIR)/%.o, ${C_FILES})
#OBJECT_FILES                    += $(patsubst %.cpp, $(O_DIR)/%.o, ${CPP_FILES})
#OBJECT_FILES			+= $(patsubst %.cpp, $(O_DIR)/%.o, ${CMD_FILES})


#####################################################################################
# Graphical Display of options to the user so that the user can make the best possible
# choice as to what to do with the makefile : some options are valgrind, (builds grind
# file) as well as other options that are pretty neato to the mudding scene.
all:
	clear
	@echo "+================================================INFECT ENGINE===============================================+"
	@echo " Build                           -- Generates $(ENGINE) executable file from Source files (updates version)"
	@echo " Depend                          -- Generates $(DEPEND) file for inclusion in make process"
	@echo " Style                           -- Formats the source files like a boss then moves the .orig files"
	@echo " Clean                           -- Cleans the source of all backup files (~) and object(.o) files"
	@echo " Backup                          -- Creates a backup of the days work and moves it to the backup directory"
	@echo " Version                         -- Takes our old .version file and generates Version.cpp"
	@echo " Count                           -- Counts all lines of code associated with .hpp and .cpp files and Makefile"
	@echo " Grind                           -- Generate our grind script (execute valgrind)"
	@echo "+===========================================================================================================+"
	@echo " Commit                          -- Makes a generic github commit; nothing special                           "
	@echo " Push                            -- Submits newest source code to github revision system                     "
	@echo " Pull                            -- Downloads source from github revision system                             "
	@echo "+===========================================================================================================+"


######################################################################################################
#Format the files like a boss!
.PHONY: style
style:
	@astyle --style=kr --indent=force-tab --indent-namespaces --indent-preprocessor --indent-col1-comments --indent-classes --indent-switches --indent-cases --pad-paren --pad-oper --add-one-line-brackets Code/*.cpp Code/commands/*.cpp ${H_DIR}/*.h
	@if [[ -n "$(shopt -s nullglob; cd Code; echo *.orig)" ]]; then mv Code/*.orig Code/orig ; fi
	@if [[ -n "$(shopt -s nullglob; cd ${CMD_DIR}; echo *.orig)" ]]; then mv ${CMD_DIR}/*.orig Code/orig ; fi
	@if [[ -n "$(shopt -s nullglob; cd ${H_DIR}; echo *.orig)" ]]; then mv ${H_DIR}/*.orig Code/orig ; fi
	@echo "Files have been styled and moved like a boss!"

######################################################################################################
#Retrieve our commited files from github - updating our local copy to the global git-copy.
.PHONY: commit
commit:
	@git commit -a -v -m "Compiler auto-update for successful release build."
	@echo "$(COLOUR_LRED)Done commiting files to github (requires push).$(COLOUR_NORMAL)"

#####################################################################################################
#Pushes any updated files to github (any local changes will be seen on github moments later)
.PHONY: push
push:
	@git push origin master
	@echo "$(COLOUR_LRED)Done pushing files to github.$(COLOUR_NORMAL)"

######################################################################################################
#Retrieve our commited files from github - updating our local copy to the global git-copy.
.PHONY: pull
pull:
	@git pull
	@echo "$(COLOUR_LRED)Done retrieving files from github.$(COLOUR_NORMAL)"


#####################################################################################
# Generate tell our system to build our executable (initiation step for the next step 
# in the makefile clears the .o files, increments the version, and builds our engine.
.PHONY: build
build: checkdirs style version ${ENGINE}

#####################################################################################
# Ensure our directories exist so that we can build properly
.PHONY: checkdirs
checkdirs:
	@if [ ! -d ${H_DIR} ]; then mkdir ${H_DIR}; fi
	@if [ ! -d ${O_DIR} ]; then mkdir ${O_DIR}; fi
	@if [ ! -d ${CMD_DIR} ]; then mkdir ${CMD_DIR}; fi	
	@if [ ! -d ${BIN_DIR} ]; then mkdir ${BIN_DIR}; fi
	@if [ ! -d ${BACKUP_DIR} ]; then mkdir ${BACKUP_DIR}; fi

#####################################################################################
#Count and report the total lines for the Mud!
.PHONY: count
count:
	@wc -l ${H_DIR}/*.h Code/*.cpp  ${CMD_DIR}/*.cpp Makefile

#####################################################################################
#Generate our version file (generates a .version file, and a .hpp file)
.PHONY: version
version:
	@if ! test -f $(VERSION_FILE); then echo 0 > $(VERSION_FILE); fi
	@echo $$(($$(cat $(VERSION_FILE)) + 1)) > $(VERSION_FILE)
	@echo "#include \"Engine.h\"" > Code/Version.cpp
	@echo "const unsigned long mudVersion = $(shell cat $(VERSION_FILE))+1;" >> Code/Version.cpp
	@echo "const char *mudBuildType = \"${BUILD_VERSION}\";" >> Code/Version.cpp
	@echo "const char *mudEngineName = \"${ENGINE}\";" >> Code/Version.cpp
	@echo "const char *mudBackupName = \"${BACKUP_NAME}\";" >> Code/Version.cpp
	@echo "const char *mudCompiler = \"${COMPILER}\";" >> Code/Version.cpp
	@echo "#ifndef __VERSION_H" > Code/Version.h
	@echo "#define __VERSION_H" >> Code/Version.h
	@echo "extern const unsigned long mudVersion;" >> Code/Version.h
	@echo "extern const char *mudBuildType;" >> Code/Version.h
	@echo "extern const char *mudEngineName;" >> Code/Version.h
	@echo "extern const char *mudBackupName;" >> Code/Version.h
	@echo "extern const char *mudCompiler;" >> Code/Version.h
	@echo "#endif" >> Code/Version.h
	@mv Code/Version.h $(H_DIR)/Version.h
	@echo "$(COLOUR_LRED)New build number assigned, attempting to build: $(COLOUR_NORMAL)"

#####################################################################################################
#Generate our engine for execution, this will build, and force the system into the next steps listed
#below in the .cpp.o section.  Note that this is epic build state.
.PHONY: $(ENGINE)
$(ENGINE): ${sort ${OBJECT_FILES} }
	@rm -f ${ENGINE}
	@$(COMPILER) ${COMPILE_FLAGS} ${CMP_STR} -o $(ENGINE) $(OBJECT_FILES) $(LIBRARIES)
	@if test -x $(BIN_DIR)/$(ENGINE) ; then mv -f $(BIN_DIR)/$(ENGINE) $(BIN_DIR)/$(ENGINE).previous ; fi
	@mv $(ENGINE) $(BIN_DIR)
	@make backup
	@echo "$(COLOUR_LRED)Compiling Completed for build $(BUILD_NUM) at $(COLOUR_LWHITE)`date +"%y-%m-%d @ %X"`$(COLOUR_LRED) and copied to ${BIN_DIR}."

#####################################################################################
#Generate our .o (object) files based off of our .cpp files
.PHONY: .cpp.o
$(O_DIR)/%.o: %.cpp
	@echo "       $(COLOUR_LWHITE)`date +"%X"`$(COLOUR_LRED)  >>  Building $(COLOUR_DBLUE)$<$(COLOUR_LRED)..."
	@${COMPILER} ${INCLUDE_FLAGS} -c $(COMPILE_FLAGS) ${CMP_STR} $<
	@mv *.o $(O_DIR)/Code/

#####################################################################################
# Generate a backup of the source code for today's date and move it to the backup folder!
# we will also email the backup to ourselves so that there will be no issues and we keep
# a good copy for ourselves
.PHONY: backup
backup:
	@tar -czf $(BACKUP_NAME) Code/*.cpp ${CMD_DIR}/*.cpp ${H_DIR}/*.h Makefile
	@uuencode $(BACKUP_NAME) $(BACKUP_NAME) |  mail -s "Compiler Backup ${BUILD_NUM}" theinfectedcity@gmail.com
	@mv *.tgz ${BACKUP_DIR}
	@echo "$(COLOUR_LRED)Backup completed.$(COLOUR_NORMAL)"

######################################################################################
# clean out our folder of any/all un-necessary files allowing us to ensure our directory
# is clean
.PHONY: clean
clean:
	@rm -rf ${O_DIR}/Code/*.o ${O_DIR}/Code/Commands/*.o *~ ${H_DIR}/*~
	@echo "$(COLOUR_LRED)Done Cleaning.$(COLOUR_NORMAL)"


