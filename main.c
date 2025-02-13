#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include <SDL2/SDL.h>

#include <zip.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

void * get_file_content(zip_t * file, const char * name, size_t * size) {
  void * result = NULL;
  struct zip_stat finfo;
  zip_stat_init(&finfo);

  size_t index = 0;
  while ((zip_stat_index(file, index, 0, &finfo)) == 0) {
    if(strncmp(finfo.name, name, strlen(name)) == 0) {
      *size =  finfo.size;
      result = calloc(*size, sizeof(uint8_t));
      zip_file_t * fd = zip_fopen_index(file, index, 0);
      zip_fread(fd, result, *size);
      break;
    }
    index++;
  }
  return result;
}

int main(int argc, char *argv[]) {
  SDL_Window * window = NULL;
  SDL_Renderer * renderer = NULL;

  if (argc < 2) {
    printf("No file provided!\n");
    return 1;
  }

  zip_t * file = NULL;
  file = zip_open(argv[1], 0, NULL);
  if (!file) {
    printf("Could not open file %s!\n", argv[1]);
    return 2;
  }

  size_t size;
  void * menu_background = get_file_content(file, "ui/startup/mainbck.tga", &size);
  zip_close(file);



  if (!menu_background) {
    printf("Could not get file from zip!\n");
    return 3;
  }

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0) {
    printf("Could not initialize SDL2: %s\n", SDL_GetError());
    return 4;
  }

  window = SDL_CreateWindow(
    "image",
    SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED,
    800,
    600,
    SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
  );
  if (window == NULL) {
    printf("Could not create window: %s\n", SDL_GetError());
    return 5;
  }

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (renderer == NULL) {
    printf("Failed to initialize renderer: %s\n", SDL_GetError());
    return 6;
  }

  // Load image
  int width, height, pixel_size;
  uint8_t * image_data = stbi_load_from_memory((uint8_t *)menu_background, size, &width, &height, &pixel_size, STBI_rgb);
  if (!image_data) {
    printf("Failed to load image into texture\n");
    return 6;
  }
  free(menu_background);

  SDL_Surface * buffer = SDL_CreateRGBSurfaceWithFormat(0, width, height, 0, SDL_PIXELFORMAT_RGB24);
  memcpy(buffer->pixels, (void *) image_data, width * height * pixel_size);
  stbi_image_free(image_data);
  image_data = NULL;

  SDL_Texture * image = SDL_CreateTextureFromSurface(renderer, buffer);
  SDL_SetTextureScaleMode(image, SDL_ScaleModeBest);
  SDL_FreeSurface(buffer);
  buffer = NULL;

  SDL_Event event;
  int running = 1;
  while (running > 0) {
    SDL_RenderClear(renderer);
    if (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT:
          running = 0;
          break;
      }
    }
    SDL_RenderCopy(renderer, image, NULL, NULL);
    SDL_RenderPresent(renderer);
  }

  SDL_DestroyTexture(image);
  SDL_DestroyWindow(window);

  return 0;
}