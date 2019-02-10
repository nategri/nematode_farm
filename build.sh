emcc --embed-file fonts \
  --embed-file img \
  src/main.c -o index.js \
  -I./include \
  -I./nanotode/include \
  nanotode/src/*.c \
  src/worm.c \
  src/behaviors.c \
  src/muscle_display.c \
  src/motion_component_display.c \
  src/trap.c \
  src/text_box.c \
  -L /usr/local/Cellar/sdl2/2.0.8/lib \
  -L /usr/local/Cellar/sdl2_ttf/2.0.14/lib \
  -s WASM=1 \
  -s USE_SDL=2 \
  -s USE_SDL_TTF=2 \
