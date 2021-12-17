#include "Snake.h"
#include "check_sdl_error.h"
#include <SDL_mixer.h>
#include "windows.h"
#include <SDL_image.h>
#include <string>
Snake::Snake()
{
    bool success = true;
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags))
    {
        printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
        success = false;
    }

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        success = false;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        success = false;
    }

    void SDL_MixAudioFormat(Uint8 * dst,
    const Uint8 * src,
    SDL_AudioFormat format,
    Uint32 len, int volume);
    auto res = SDL_Init(SDL_INIT_EVERYTHING);
    SDL_CHECK(res == 0, "SDL_Init");
    SDL_DisplayMode displayMode;
    int request = SDL_GetDesktopDisplayMode(0, &displayMode);
    SDL_CreateWindowAndRenderer(Width, Height, SDL_WINDOW_SHOWN, &window, &renderer);
    SDL_CHECK(window, "SDL_CreateWindowAndRenderer");
    SDL_CHECK(renderer, "SDL_CreateWindowAndRenderer");
    SDL_SetWindowPosition(window, 300, 150);
    SDL_SetWindowTitle(window, "Score: 0");
    auto surface = SDL_LoadBMP("smake.bmp");
    SDL_CHECK(surface, "SDL_LoadBMP(\"smake.bmp\")");
    sprites = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_CHECK(sprites, "SDL_CreateTextureFromSurface");
    SDL_FreeSurface(surface);

    background_RECT.x = 0;
    background_RECT.y = 0;
    background_RECT.w = displayMode.w;
    background_RECT.h = displayMode.h;

    segmentsList.push_back(std::make_pair(5, 5));
    segmentsList.push_back(std::make_pair(5, 6));
    segmentsList.push_back(std::make_pair(4, 6));
    generateItem();
}

Snake::~Snake()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    Mix_Quit();
    IMG_Quit();
}

void Snake::generateItem()
{
    auto done = false;
    do
    {
        score++;
        std::string tmp = std::to_string(score);
        char const* num_char = tmp.c_str();
        SDL_SetWindowTitle(window, num_char);
        hrum = Mix_LoadWAV("2.wav");
        Mix_PlayChannel(1, hrum, 0) == 1;
        itemX = rand() % (Width / 32);
        itemY = rand() % (Height / 32);
        done = true;
        for (const auto& segment : segmentsList)
        {
            if (itemX == segment.first && itemY == segment.second)
            {
                done = false;
                break;
            }
        }
    } while (!done);
}
int Snake::exec()
{
    music = Mix_LoadMUS("1.mp3");
    if (Mix_PlayMusic(music, -1) == -1) {
        printf("Mix_PlayMusic: %s\n", Mix_GetError());
    }
    auto oldTick = SDL_GetTicks();
    SDL_Surface* bg = SDL_LoadBMP("bg.bmp");
    SDL_Texture* bg_texture = SDL_CreateTextureFromSurface(renderer, bg);
    for (auto done = false; !done;)
    {
        SDL_Event e;

        if (SDL_PollEvent(&e))
        {
            switch (e.type)
            {
            case SDL_KEYDOWN:
            {
                switch (e.key.keysym.sym)
                {
                case SDLK_UP:
                    dx = 0;
                    dy = -1;
                    break;
                case SDLK_RIGHT:
                    dx = 1;
                    dy = 0;
                    break;
                case SDLK_DOWN:
                    dx = 0;
                    dy = 1;
                    break;
                case SDLK_LEFT:
                    dx = -1;
                    dy = 0;
                    break;
                }
            }
            break;
            case SDL_QUIT:
                done = true;
                break;
            }
        }

        SDL_RenderClear(renderer);
        auto currentTick = SDL_GetTicks();
        for (auto t = oldTick; t < currentTick; ++t)
            if (!tick())
                return 1;
        oldTick = currentTick;
        SDL_RenderCopy(renderer, bg_texture, NULL, NULL);
        draw();
        SDL_RenderPresent(renderer);
    }
    return 0;
}

int Snake::GetScore()
{
    return score;
}

bool Snake::tick()
{
    if (ticks++ % 250 == 0)
    {
        auto p = segmentsList.front();
        p.first += dx;
        p.second += dy;
        if (p.first < 0 || p.first >= Width / 32 ||
            p.second < 0 || p.second >= Height / 32) {
            gg = Mix_LoadMUS("gg.mp3");
            if (Mix_PlayMusic(gg, 1) == 1) {
                printf("Mix_PlayMusic: %s\n", Mix_GetError());
            }
            Sleep(2000);
            return false;
        }
        for (const auto& segment : segmentsList)
            if (p == segment) {
                gg = Mix_LoadMUS("gg.mp3");
                if (Mix_PlayMusic(gg, 1) == 1) {
                    printf("Mix_PlayMusic: %s\n", Mix_GetError());
                }
                Sleep(2000);
                return false;
            }
        segmentsList.push_front(p);
        if (p.first != itemX || p.second != itemY) {

            segmentsList.pop_back();
        }
        else
            generateItem();
    }
    return true;
}
void Snake::draw()
{
    SDL_Rect src;
    src.x = 0;
    src.y = 0;
    src.w = 32;
    src.h = 32;
    SDL_Rect dest;
    dest.w = 32;
    dest.h = 32;
    int ds[][3] = {
      { -1, 0, 0 },
      { 0, -1, 90 },
      { 1, 0, 180 },
      { 0, 1, -90 },
    };
    for (auto iter = std::begin(segmentsList); iter != std::end(segmentsList); ++iter)
    {
        float direction = 0;
        const auto& segment = *iter;
        if (iter == std::begin(segmentsList))
        {
            src.x = Head * 32;
            if (iter + 1 != std::end(segmentsList))
            {
                const auto& nextSegment = *(iter + 1);
                for (const auto& d : ds)
                {
                    if (segment.first + d[0] == nextSegment.first && segment.second + d[1] == nextSegment.second)
                    {
                        direction = d[2];
                        break;
                    }
                }
            }
        }
        else if (iter + 1 == std::end(segmentsList))
        {
            src.x = Tail * 32;
            const auto& prevSegment = *(iter - 1);
            for (const auto& d : ds)
            {
                if (segment.first == prevSegment.first + d[0] && segment.second == prevSegment.second + d[1])
                {
                    direction = d[2];
                    break;
                }
            }
        }
        else
        {
            const auto& nextSegment = *(iter + 1);
            const auto& prevSegment = *(iter - 1);
            if (nextSegment.first == prevSegment.first)
            {
                src.x = Straight * 32;
                direction = 90;
            }
            else if (nextSegment.second == prevSegment.second)
            {
                src.x = Straight * 32;
                direction = 0;
            }
            else
            {
                src.x = Turn * 32;
                bool up = false;
                if (segment.first == nextSegment.first && segment.second - 1 == nextSegment.second)
                    up = true;
                if (segment.first == prevSegment.first && segment.second - 1 == prevSegment.second)
                    up = true;
                bool right = false;
                if (segment.first + 1 == nextSegment.first && segment.second == nextSegment.second)
                    right = true;
                if (segment.first + 1 == prevSegment.first && segment.second == prevSegment.second)
                    right = true;
                bool down = false;
                if (segment.first == nextSegment.first && segment.second + 1 == nextSegment.second)
                    down = true;
                if (segment.first == prevSegment.first && segment.second + 1 == prevSegment.second)
                    down = true;
                bool left = false;
                if (segment.first - 1 == nextSegment.first && segment.second == nextSegment.second)
                    left = true;
                if (segment.first - 1 == prevSegment.first && segment.second == prevSegment.second)
                    left = true;
                if (up && right)
                    direction = 0;
                else if (right && down)
                    direction = 90;
                else if (down && left)
                    direction = 180;
                else if (left && up)
                    direction = 270;
            }
        }

        dest.x = 32 * segment.first;
        dest.y = 32 * segment.second;
        SDL_RenderCopyEx(renderer, sprites, &src, &dest, direction, nullptr, SDL_FLIP_NONE);
    }
    src.x = Item * 32;
    dest.x = itemX * 32;
    dest.y = itemY * 32;
    SDL_RenderCopyEx(renderer, sprites, &src, &dest, 0, nullptr, SDL_FLIP_NONE);

}