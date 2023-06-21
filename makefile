# Compiler settings
CC = g++
CXXFLAGS = -std=c++14 -I. -I../comunication -I../comunication/comm -I../comunication/utils -DICE_CPP11_MAPPING

# Makefile settings
APPNAME = backend
EXT = .cpp
SRCDIR = ./src
CMMDIR = ../comunication/comm
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
OBJCOMM = $(wildcard $(CMMDIR)/*.o) $(COMM:$(CMMDIR)/%$(EXT)=$(OBJDIR)/%.o)
OBJ = $(SRC:$(SRCDIR)/%$(EXT)=$(OBJDIR)/%.o)

########################################################################
####################### Targets beginning here #########################
########################################################################

all: $(APPNAME)

# Builds the app
$(APPNAME): $(OBJ) $(OBJCOMM)
	$(CC) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Includes all .h files
-include $(DEP)

# Building rule for .o files and its .c/.cpp in combination with all .h
$(OBJDIR)/%.o: $(SRCDIR)/%$(EXT) | gen_obj_dir
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

.PHONY: gen_obj_dir
gen_obj_dir:
	mkdir -p $(OBJDIR)

################### Cleaning rules ###################
# Cleans complete project
.PHONY: clean
clean:
	$(RM) $(APPNAME) $(DEP) $(OBJ)

################### Display variables ###################
echo:
	@echo $(SRC)
	@echo $(COMM)
	@echo $(OBJ)
	@echo $(DEP)
	@echo $(UTIL)
	@echo $(RLIB)
	@echo $(LDFLAGS)
	@echo $(CXXFLAGS)
	@echo $(APPNAME)
	@echo $(EXT)
	@echo $(SRCDIR)
	@echo $(CMMDIR)
	@echo $(OBJDIR)
	@echo $(RLIB)
	@echo $(RCMD)

