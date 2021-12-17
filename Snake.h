#include <SDL.h>
#include <deque>
#include <SDL_mixer.h>
#include <SDL_image.h>
#include <string>
class Snake
{
public:
	Snake();
	~Snake();
	const static auto Width = 1280;
	const static auto Height = 720;
	static const auto Tail = 0;
	static const auto Turn = 1;
	static const auto Straight = 2;
	static const auto Head = 3;
	static const auto Item = 4;
	int exec();
	bool tick();
	void draw();
	int GetScore();
	void UpdateWindowTitle();

private:
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* sprites;
	SDL_Rect background_RECT;
	std::deque<std::pair<int, int> > segmentsList;
	unsigned ticks = 0;
	int dx = 1;
	int dy = 0;
	int itemX;
	int itemY;
	void generateItem();
	Mix_Music* music;
	Mix_Chunk* hrum;
	Mix_Music* gg;
	int score = -1;
};

