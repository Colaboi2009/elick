#pragma once

#include <SDL3_ttf/SDL_ttf.h>
#include <string>

#include "texture.hpp"

class RawText {
  private:
    Texture m_texture;

  public:
    RawText(std::string, SDL_Color, TTF_Font *, int allowedHeight = 0);
    ~RawText();

	Texture tex() { return m_texture; }
    void render(SDL_FRect);

  public:
    static void Render(std::string, SDL_FRect, SDL_Color, TTF_Font *);
};
