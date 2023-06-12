# Compiler settings
CC = g++
CXXFLAGS = -std=c++14 -I. -I../comunication -DICE_CPP11_MAPPING

# Makefile settings
APPNAME = backend
EXT = .cpp
SRCDIR = ./src
CMMDIR = ../comunication
OBJDIR = ./src/obj

# Linking lib
LDFLAGS = -lwt -lwtdbo -lwtdbosqlite3 -lIce++11
# -lwtdbosqlite3
# -lwtdbopostgres

# Runtime lib
RLIB = 

############## Creating variables #############
SRC = $(wildcard $(SRCDIR)/*$(EXT))
COMM = $(wildcard $(CMMDIR)/*$(EXT))
OBJ = $(SRC:$(SRCDIR)/%$(EXT)=$(OBJDIR)/%.o) $(COMM:$(CMMDIR)/%$(EXT)=$(OBJDIR)/%.o)
DEP = $(OBJ:$(OBJDIR)/%.o=%.d)

########################################################################
####################### Targets beginning here #########################
########################################################################

all: $(APPNAME)

# Builds the app
$(APPNAME): $(OBJ)
	$(CC) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Creates the dependecy rules
%.d: $(SRCDIR)/%$(EXT)
	@$(CPP) $(CFLAGS) $< -MM -MT $(@:%.d=$(OBJDIR)/%.o) >$@

# Includes all .h files
-include $(DEP)

# Building rule for .o files and its .c/.cpp in combination with all .h
$(OBJDIR)/%.o: $(SRCDIR)/%$(EXT) 
	$(CC) $(CXXFLAGS) -o $@ -c $<

$(OBJDIR)/%.o: $(CMMDIR)/%$(EXT)
	$(CC) $(CXXFLAGS) -o $@ -c $<

################## Run #################
run:
	./$(APPNAME) $(RLIB)

runTrace:
	./$(APPNAME) --Ice.Trace.Network=2 $(RCMD) 

dbg:
	gdb ./$(APPNAME)

################### Cleaning rules ###################
# Cleans complete project
.PHONY: clean
clean:
	$(RM) $(APPNAME) $(DEP) $(OBJ)

################### Display variables ###################
displayVariables:
	@echo $(SRC)
	@echo $(COMM)
	@echo $(OBJ)
	@echo $(DEP)
