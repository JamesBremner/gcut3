#source file search paths
VPATH = ./src 
# compiler include search paths
INCS = -I./include \
	-I../ravencode/pub/src \
	-I../windex/include \
	-I../raven-set \
	-I../boost/boost1_72 \
	-I../pack2/src
LIBS = \
	-lgdiplus -lgdi32 -lcomdlg32 -lstdc++fs \
	#-mwindows
static_link_flags = -static-libstdc++ -static-libgcc -static

# folders
ODIR = ./.vscode/obj
RDIR = ../ravencode/pub/src

OPT = -g
CCflags = -std=c++17 $(OPT) -D_mingw_

# sources

OBJ = \
	$(ODIR)/main.o $(ODIR)/instance.o $(ODIR)/gcut3.o \
	$(ODIR)/space.o $(ODIR)/cut.o $(ODIR)/level.o \
	$(ODIR)/pack2.o $(ODIR)/cCut.o

TESTOBJ = \
	$(ODIR)/unitTest.o $(ODIR)/instance.o $(ODIR)/gcut3.o \
	$(ODIR)/space.o $(ODIR)/cut.o $(ODIR)/level.o \
	$(ODIR)/pack2.o $(ODIR)/cCut.o \
	$(ODIR)/cutest.o

# COMPILE 

$(ODIR)/pack2.o: ../pack2/src/pack2.cpp
	g++ $(CCflags) \
	-c -o  $@ $< $(INCS)
$(ODIR)/cCut.o: ../pack2/src/cCut.cpp
	g++ $(CCflags) \
	-c -o  $@ $< $(INCS)
$(ODIR)/cutest.o: ../raven-set/cutest.cpp
	g++ $(CCflags) \
	-c -o  $@ $< $(INCS)
$(ODIR)/%.o: %.cpp  
	g++  $(CCflags) \
	-c -o $@ $< $(INCS)

# LINK

gcut3: $(OBJ)
	g++ $(OPT) \
	-o bin/gcut3.exe $^  \
	$(static_link_flags) $(LIBS)

test: $(TESTOBJ)
	g++ $(OPT) \
	-o bin/test.exe $^  \
	$(static_link_flags) $(LIBS)
