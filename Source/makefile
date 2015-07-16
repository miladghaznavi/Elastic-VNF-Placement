##############################################################################
#
#  Sample Makefile for C++ applications
# 
##############################################################################

##############################################################################
# Application-specific variables
# EXEC is the name of the executable file
# SRC_FILES is a list of all source code files that must be linked
#           to create the executable
##############################################################################

EXEC      = vnfplacement
SRC_FILES = algorithm.cpp demand.cpp graphio.cpp simulator.cpp assignment.cpp event.cpp main.cpp slfl.cpp basicalg.cpp facility.cpp slflsim.cpp basicalgsim.cpp fattree.cpp randalg.cpp substrategraph.cpp basics.cpp randalgsim.cpp vl2.cpp cost.cpp flowgraph.cpp report.cpp resource.cpp


##############################################################################
# Where to find course related files

# for acpub machines
COURSE_DIR = /usr/local/lib
LIB_DI     = /usr/local/lib

##############################################################################
# Compiler specifications
# gmake prefers CXX and CXXFLAGS for c++ programs
##############################################################################
# Which compiler should be used
CXX = g++
CC = $(CXX)
# What flags should be passed to the compiler

DEBUG_LEVEL     = -g
EXTRA_CCFLAGS   = -Wall
STD_FLAG	= -std=c++11
PTHREAD_FLAG	= -pthread
OTHER_FLAGS	= -Wl,--no-as-needed
CXXFLAGS        = $(DEBUG_LEVEL) $(OTHER_FLAGS) $(EXTRA_CCFLAGS) $(STD_FLAG) $(PTHREAD_FLAG)
CCFLAGS         = $(CXXFLAGS)

# What flags should be passed to the C pre-processor
#   In other words, where should we look for files to include - note,
#   you should never need to include compiler specific directories here
#   because each compiler already knows where to look for its system
#   files (unless you want to override the defaults)

CPPFLAGS        = -I. \
                  -I$(COURSE_DIR)

# What flags should be passed to the linker
#   In other words, where should we look for libraries to link with - note,
#   you should never need to include compiler specific directories here
#   because each compiler already knows where to look for its system files.

LDFLAGS         = -L. \
                  -L$(COURSE_DIR) \
                  -R $(LIB_DIR):$(COURSE_DIR)

# What libraries should be linked with.
# For example, -lm links with libm.so, the math library.
# If you make a library of your own, say, libscandir.a, you have to link it
# in by adding -lscandir here.
LDLIBS          = -lscandir

# All source files have associated object files.
# This line sets `OFILES' to have the same value as `SRC_FILES' but
# with all the .cc's changed into .o's.
O_FILES         = $(SRC_FILES:%.cpp=%.o)


###########################################################################
# Additional rules make should know about in order to compile our files
###########################################################################
# all is the default rule
all: $(EXEC)
	g++	$(STD_FLAG)	$(OTHER_FLAGS)		$(PTHREAD_FLAG)		-g	$(O_FILES)	-lm	-o	vnfplacement 


# exec depends on the object files
# It is made automagically using the LDFLAGS and LOADLIBES variables.
# The .o files are made automagically using the CXXFLAGS variable.
$(EXEC): $(O_FILES)

# to use `makedepend', the target is `depend'
depend:
	->	makedepend	--	$(CXXFLAGS)	--	-Y	$(SRC_FILES)



# clean up after you're done
clean:
	->	$(RM)	$(O_FILES)
