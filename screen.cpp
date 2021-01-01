#include "screen.h"

#include <iostream>
#include <vector>
#include <algorithm>

namespace simulation {

  Screen::Screen() : _window(NULL),
                     _renderer(NULL),
                     _texture(NULL),
                     _buffer(NULL) {}


  bool Screen::init() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
      std::cout << "SDL_Init failed. Error: " << SDL_GetError() << std::endl;
      // return 1;  // returning a positive int means an error occurred
      return false;
    }
    // Main window
    _window = SDL_CreateWindow("Fire Particle Simulation", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    // Check that the window was successfully created
    if (_window == NULL || _window == nullptr) {
      // In the case that the window could not be made...
      printf("Could not create window: %s\n", SDL_GetError());
      SDL_Quit();
      return false;
    }
    // Renderer object
    _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_PRESENTVSYNC);  // index: the index of the rendering driver to initialize, or -1 to initialize the first driver supporting the requested flags; flags: SDL_RENDERER_PRESENTVSYNC – present is synchronized with the refresh rate (it helps prevent rendering at the time when screen is rendering)
    _texture = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC, SCREEN_WIDTH, SCREEN_HEIGHT);  // format (2nd arg) is a number of possible constants (enum), depending on how many bites you want to represent one pixel; SDL_PIXELFORMAT_RGBA8888 uses one byte for each pixel. Access (3rd arg) is the way we are writing to the texture
    // Check if renderer is absent
    if (_renderer == NULL || _renderer == nullptr) {
      printf("Could not create renderer: %s\n", SDL_GetError());
      SDL_DestroyWindow(_window);
      SDL_Quit();
      return false;
    }
    // Check if texture is absent
    if (_texture == NULL || _texture == nullptr) {
      printf("Could not create texture: %s\n", SDL_GetError());
      SDL_DestroyRenderer(_renderer);
      SDL_DestroyWindow(_window);
      SDL_Quit();
      return false;
    }
    // Allocating memory buffer for pixels (RGBA = 4 values x 8 bits = 32 bits)
    _buffer = new Uint32[SCREEN_WIDTH * SCREEN_HEIGHT];  // TODO 1: Ideally, one needs to check for an exception in case the memory allocation fails. Not done here, as any computer will succeed, unless it's tight of memory.

    // Write pixel information into the buffer
    memset(_buffer, 0, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(Uint32));  // set a block of memory with a particular value; make pixel white (2nd arg) in all pixels (3rd arg). IMPORTANT: one can substiture 0xFF (hex) for 255 (2nd arg). Useful, as one ends up with only 2 digits (FF) per byte with hex, rather than with decimal values. It means it's easier to see which byte represents what in the final integer that represents the color of the pixel of the screen. Advantage: easier to see which byte corresponds to which color.

    // // Set the color of an individual pixel
    // _buffer[30000] = 0xFFFFFFFF;  // setting all 4 bytes to their maximum value (FF). I.e. 0xFF(1)FF(2)FF(3)FF(4) = R(1)G(2)B(3)A(4) set to white (255)
    // // Experimenting with changing colors
    // for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {  // for loop works slower than memset()
    //   _buffer[i] = 0xFFFF00FF;
    // }
    // For reference (see more in "_playground/_bit-shifting.cpp")
    // 0x123456FF
    // unsigned char red = 0x12;
    // unsigned char green = 0x34;
    // unsigned char blue = 0x56;
    // unsigned char alpha = 0xFF;
    // 
    // unsigned int color = 0;

    return true;
  }


  // Listen to events incurred by user
  bool Screen::processEvents() {
    SDL_Event event;
    // Check for messages/events (loop through all waiting events and process them)
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {  // Quit event is raised whenever we click "close" on a window
        // quit = true;  // before refactor
        return false;
      }
    }
    return true;
  }


  // Change pixel
  void Screen::setPixel(int x, int y, Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha) {  // TODO: Experiment with references and check top
    // If the pixel is off the screen
    if (x < 0 || x > SCREEN_WIDTH || y < 0 || y > SCREEN_HEIGHT) {
      return;
    }
    std::vector<Uint8> rgba;
    Uint32 color{0};
    color += red;
    color <<= 8;
    color += green;
    color <<= 8;
    color += blue;
    color <<= 8;
    color += alpha;
    _buffer[(y * SCREEN_WIDTH) + x] = color;  // y * SCREEN_WIDTH -> going to the needed row; + x -> move to the right column by x columns. Then we get an index and pass it to the buffer
  }


  // Update screen
  void Screen::update() {
    // Update texture (take pixel data and put it on the screen)
    SDL_UpdateTexture(_texture, NULL, _buffer, SCREEN_WIDTH * sizeof(Uint32));  // rect (2nd arg) represents area of screen which requires update; NULL stands for entire screen. Pixels (3rd arg) = buffer. Pitch (4th arg) is the number of bytes in a row of pixel data, including padding between lines.
    // Clear the renderer
    SDL_RenderClear(_renderer);
    // Copy texture to the renderer
    SDL_RenderCopy(_renderer, _texture, NULL, NULL);  // This function copies a portion of the texture to the current rendering target. srcrect (3rd arg – the source SDL_Rect structure or NULL for the entire texture. dstrect (4th arg) – the destination SDL_Rect structure or NULL for the entire rendering target; the texture will be stretched to fill the given rectangle
    // Update the screen
    SDL_RenderPresent(_renderer);  // Use this function to update the screen with any rendering performed since the previous call.
  }


  // Freeing memory and quitting SDL before the program ends
  void Screen::close() {
    delete [] _buffer;
    SDL_DestroyRenderer(_renderer); // Destroy renderer
    SDL_DestroyTexture(_texture); // Destroy texture
    SDL_DestroyWindow(_window);  // Destroy window
    SDL_Quit();
  }


}  /* namespace simulation */