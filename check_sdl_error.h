#pragma once
#include <stdexcept>
#include <string>
#include <SDL_image.h>
#include <SDL_mixer.h>

#define SDL_CHECK(x, msg) if (!(x))                                      \
    throw std::runtime_error(std::string(msg": ") + SDL_GetError())