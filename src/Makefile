.PHONY: all clean install uninstall

EXNAME=time-notify
RINGNAME=old-telephone-ring.wav
CXX=g++
CXXFLAGS=-Wall
CXXLIBS=-lao -lsndfile

CXXFLAGS += -D RINGPATH=\"/var/lib/$(EXNAME)/$(RINGNAME)\"

all: main.cpp PlayAudio.cpp PlayAudio.hpp
	$(CXX) $(CXXFLAGS) main.cpp PlayAudio.cpp $(CXXLIBS)

clean:
	-rm -f a.out

install: all
	sudo install -s a.out /usr/local/bin/$(EXNAME)
	sudo install -d /var/lib/$(EXNAME)
	sudo install -m 0644 $(RINGNAME) /var/lib/$(EXNAME)

uninstall:
	if [ "$(EXNAME)" != "" ]; then sudo rm /usr/local/bin/$(EXNAME); else exit 1; fi
	if [ "$(EXNAME)" != "" ]; then sudo rm -r /var/lib/$(EXNAME); else exit 1; fi
