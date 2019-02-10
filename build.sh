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
  -s WASM=1 \
  -s USE_SDL=2 \
  -s USE_SDL_TTF=2 \
