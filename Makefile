#Makefile の書き方 (C 言語) — WTOPIA v1.0 documentation 
#https://ie.u-ryukyu.ac.jp/~e085739/c.makefile.tuts.html

#https://www.gnu.org/software/make/manual/html_node/Implicit-Variables.html
#https://stackoverflow.com/questions/12054858/add-so-and-a-libraries-to-makefile

#https://stackoverflow.com/questions/71894605/make-cant-find-files-from-github-action

SHELL=/bin/bash

TARGET=main
CXXFLAGS=-Iglfw/deps
LDLIBS=-lglfw3
OBJS = main.o


#https://stackoverflow.com/questions/19146525/meaning-of-library-dl-in-gcc
#https://stackoverflow.com/questions/714100/os-detecting-makefile
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
        LDLIBS += -ldl
endif
ifeq ($(UNAME_S),Darwin)
        LDLIBS += -framework OpenGL -framework Cocoa -framework IOKit -framework Corevideo 
endif

all:submodule install_glfw build

submodule: glfw
	git submodule update --init

#https://stackoverflow.com/questions/17768008/how-to-build-install-glfw-3-and-use-it-in-a-linux-project
build: main.c
	$(CXX) $^  -o $(TARGET) $(LDLIBS) $(CXXFLAGS)

#https://www.glfw.org/docs/latest/compile.html
#https://github.com/glfw/glfw
install_glfw:
	cmake -S glfw -B glfw/build
	cd glfw/build && $(MAKE) && sudo $(MAKE) install

uninstall_glfw:
	cd glfw/build && $(MAKE) uninstall
 
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all submodule build install_glfw uninstall_glfw clean
