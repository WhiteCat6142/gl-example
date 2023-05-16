#Makefile の書き方 (C 言語) — WTOPIA v1.0 documentation 
#https://ie.u-ryukyu.ac.jp/~e085739/c.makefile.tuts.html

#https://www.gnu.org/software/make/manual/html_node/Implicit-Variables.html
#https://stackoverflow.com/questions/12054858/add-so-and-a-libraries-to-makefile

SHELL=/bin/bash

TARGET=main
CXXFLAGS=-Iglfw/deps
LDLIBS=-lglfw
OBJS = main.o


all:submodule install_glfw build

submodule: glfw
	git submodule update --init

#https://stackoverflow.com/questions/17768008/how-to-build-install-glfw-3-and-use-it-in-a-linux-project
build: main.c
	$(CXX) -c $^ $(CXXFLAGS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDLIBS)

#https://www.glfw.org/docs/latest/compile.html
#https://github.com/glfw/glfw
install_glfw:
	cmake -S glfw -B glfw/build
	cd glfw/build && $(make) && $(make) install

uninstall_glfw:
	cd glfw/build && $(make) uninstall
 
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all submodule build install_glfw uninstall_glfw clean
