gcc -Wfatal-errors src/main.c -o nematode_farm \
  -I /usr/local/Cellar/sdl2/2.0.8/include/SDL2 \
  -I ./include \
  -I ./nanotode/include \
  nanotode/src/*.c \
  src/worm.c \
  src/behaviors.c \
  src/muscle_display.c \
  src/motion_component_display.c \
  -L /usr/local/Cellar/sdl2/2.0.8/lib \
  -l SDL2-2.0.0
