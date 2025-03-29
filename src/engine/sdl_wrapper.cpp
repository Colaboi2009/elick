#include "sdl_wrapper.hpp"
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <cstdlib>

SDL::SDL() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();

    SDL_CreateWindowAndRenderer("elric_sdl", 1920, 1080, SDL_WINDOW_RESIZABLE, &m_window, &m_renderer);
    SDL_SetRenderVSync(m_renderer, 1);

    present();
    system("hyprctl dispatch togglefloating class:elric");
}

SDL::~SDL() {
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

void SDL::drawCircle(int radius, int x, int y) {
    int offsetx, offsety, d;

    offsetx = 0;
    offsety = radius;
    d = radius - 1;

    while (offsety >= offsetx) {
        drawLine(x - offsety, y + offsetx, x + offsety, y + offsetx);
        drawLine(x - offsetx, y + offsety, x + offsetx, y + offsety);
        drawLine(x - offsetx, y - offsety, x + offsetx, y - offsety);
        drawLine(x - offsety, y - offsetx, x + offsety, y - offsetx);

        if (d >= 2 * offsetx) {
            d -= 2 * offsetx + 1;
            offsetx += 1;
        } else if (d < 2 * (radius - offsety)) {
            d += 2 * offsety - 1;
            offsety -= 1;
        } else {
            d += 2 * (offsety - offsetx - 1);
            offsety -= 1;
            offsetx += 1;
        }
    }
}
