emcc --embed-file fonts \
  --embed-file img \
  source/main.c -o nematodefarm.js \
  -I./source \
  -I./nanotode/source \
  nanotode/source/*.c \
  source/*.c \
  -s WASM=1 \
  -s USE_SDL=2 \
  -s USE_SDL_TTF=2
