SYSCONF_LINK = g++
CPPFLAGS     = -MD -Wall -Wextra -Weffc++ -pedantic -std=c++98
LDFLAGS      = -O3
LIBS         = -lm 
LIBS += `pkg-config --libs opencv`
INC += `pkg-config --cflags opencv`

DESTDIR = ./
TARGET  = main

OBJECTS := $(patsubst %.cpp,%.o,$(wildcard *.cpp))
DEPS=$(OBJECTS:.o=.d)

all: $(DESTDIR)$(TARGET)

$(DESTDIR)$(TARGET): $(OBJECTS)
	$(SYSCONF_LINK) -Wall $(LDFLAGS) -o $(DESTDIR)$(TARGET) $(OBJECTS) $(LIBS)

$(OBJECTS): %.o: %.cpp
	$(SYSCONF_LINK) -Wall $(CPPFLAGS) -c $(CFLAGS) $< -o $@

clean:
	-rm -f $(OBJECTS)
	-rm -f $(TARGET)
	-rm -f *.tga
	-rm -f $(DEPS)

-include $(DEPS)
