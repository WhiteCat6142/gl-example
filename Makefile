#Makefile の書き方 (C 言語) — WTOPIA v1.0 documentation 
#https://ie.u-ryukyu.ac.jp/~e085739/c.makefile.tuts.html

#https://www.gnu.org/software/make/manual/html_node/Implicit-Variables.html
#https://stackoverflow.com/questions/12054858/add-so-and-a-libraries-to-makefile

TARGET=main
CXXFLAGS=-Iglfw/deps
LDLIBS=-lglfw
OBJS = main.o

.PHONY: all
all:submodule build

.PHONY: submodule
submodule: glfw
	git submodule update --init

#https://stackoverflow.com/questions/17768008/how-to-build-install-glfw-3-and-use-it-in-a-linux-project
.PHONY: build
build: main.c
	$(CXX) -c $^ $(CXXFLAGS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDLIBS)

#https://www.glfw.org/docs/latest/compile.html
#https://github.com/glfw/glfw
.PHONY: install_glfw
install_glfw:
	cmake -S glfw -B glfw/build
	cd glfw/build && $(make) && $(make) install

.PHONY: uninstall_glfw
uninstall_glfw:
	cd glfw/build && $(make) uninstall
 
.PHONY: clean
clean:
	rm -f $(OBJS) $(TARGET)
