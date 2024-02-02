#include "SDL.h"
#include <iostream>

bool IsPointInRect(int x, int y, const SDL_Rect& rect);
float Function(float x, float a, int n);
void rainbowColor(float value, Uint8& red, Uint8& green, Uint8& blue);

int main(int argc, char* argv[])
{
	SDL_InitSubSystem(SDL_INIT_EVERYTHING);
	int mouseX = 0, mouseY = 0; // stores the position of the mouse pointer
	SDL_Rect screenRect; // stores the resolution of the screen
	SDL_Rect sliderA;
	SDL_Rect sliderN;
	SDL_Rect sliderAInner;
	SDL_Rect sliderNInner;
	SDL_Rect sliderAHeader;
	SDL_Rect sliderNHeader;

	int screenMid;

	int sliderHeight;
	int sliderWidth;

	float maxA = 20.0f;
	float maxN = 10;
	float a = maxA;
	int n = maxA;

	SDL_DisplayMode dm;
	if (SDL_GetCurrentDisplayMode(0, &dm) != 0) {
		std::cout << "error while determining the display resolution\n" << SDL_GetError() << std::endl;
		return 1;
	}
	screenRect.w = dm.w;
	screenRect.h = dm.h;
	sliderHeight = screenRect.h / 50;
	sliderWidth = (screenRect.w * 2) / 3;

	screenMid = screenRect.w / 2;

	sliderA = { (screenRect.w - sliderWidth) / 2, sliderHeight, sliderWidth, sliderHeight };
	sliderN = { (screenRect.w - sliderWidth) / 2, 3 * sliderHeight, sliderWidth, sliderHeight };
	sliderAInner = { (screenRect.w - sliderWidth) / 2, sliderHeight, sliderWidth, sliderHeight };
	sliderNInner = { (screenRect.w - sliderWidth) / 2, 3 * sliderHeight, sliderWidth, sliderHeight };

	sliderAHeader = {(int)(screenMid - (screenRect.w * 0.07f)), 0, (int)(screenRect.w * 0.14f), sliderHeight};
	sliderNHeader = { (int)(screenMid - (screenRect.w * 0.07f)), 2 * sliderHeight, (int)(screenRect.w * 0.14f), sliderHeight };

	int threeHbyFive = 3.0f * screenRect.h / 5.0f;

	float functionYScalingFactor = screenRect.h * 2.0f;
	float functionXScalingFactor = screenRect.w * 0.01f;

	const SDL_Rect functionImageRect = { 0.4f * screenRect.w, 5 * sliderHeight, 0.2f * screenRect.w, 5 * sliderHeight };

	SDL_Window* window = SDL_CreateWindow("physics project", 0, 0, NULL, NULL, SDL_WINDOW_FULLSCREEN_DESKTOP);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	SDL_Event ev;

	bool running = true;
	bool leftButtonPressed = false;

	// making it so that when the renderer draws it blends the color according to the alpha value
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	SDL_Texture* functionImage = SDL_CreateTextureFromSurface(renderer, SDL_LoadBMP("Image.bmp"));
	SDL_Texture* sliderAHeadingImage = SDL_CreateTextureFromSurface(renderer, SDL_LoadBMP("SliderAHeading.bmp"));
	SDL_Texture* sliderNHeadingImage = SDL_CreateTextureFromSurface(renderer, SDL_LoadBMP("SliderNHeading.bmp"));

	while (running) {
		// TAKING INPUT FROM USER
		while (SDL_PollEvent(&ev)) {
			if (ev.type == SDL_QUIT) {
				running = false;
			}
			else if (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_ESCAPE) {
				running = false;
			}
			else if (ev.type == SDL_MOUSEMOTION) {
				SDL_GetMouseState(&mouseX, &mouseY);
			}
			else if (ev.type == SDL_MOUSEBUTTONDOWN && ev.button.button == SDL_BUTTON_LEFT) {
				leftButtonPressed = true;
			}
			else if (ev.type == SDL_MOUSEBUTTONUP && ev.button.button == SDL_BUTTON_LEFT) {
				leftButtonPressed = false;
			}
		}

		// UPDATING THE VARIABLES
		if (leftButtonPressed) {
			if (IsPointInRect(mouseX, mouseY, sliderA)) {
				sliderAInner.w = mouseX - sliderAInner.x;
				a = maxA * sliderAInner.w / sliderA.w;
			}
			else if (IsPointInRect(mouseX, mouseY, sliderN)) {
				sliderNInner.w = mouseX - sliderNInner.x;
				n = round(maxN * sliderNInner.w / sliderN.w);
				sliderNInner.w = (float)sliderA.w * (n / maxN);
			}
		}

		// DRAWING THE SCENE
		SDL_SetRenderDrawColor(renderer, 255, 230, 125, 255);
		SDL_SetRenderTarget(renderer, nullptr);
		SDL_RenderClear(renderer); // setting the target to nullptr sets the target to the window


		// drawing the inner portion of the sliders
		SDL_SetRenderDrawColor(renderer, 255, 122, 34, 255);
		SDL_RenderFillRect(renderer, &sliderAInner);
		SDL_RenderFillRect(renderer, &sliderNInner);

		// drawing the outer border of the sliders
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderDrawRect(renderer, &sliderA);
		SDL_RenderDrawRect(renderer, &sliderN);

		// drawing the slider headings
		SDL_RenderCopy(renderer, sliderAHeadingImage, NULL, &sliderAHeader);
		SDL_RenderCopy(renderer, sliderNHeadingImage, NULL, &sliderNHeader);
		
		// drawing the function
		for (int i = 0.1f * screenRect.w; i <= 0.9f * screenRect.w; i++) {
			float y = functionYScalingFactor * Function(functionXScalingFactor * ((float)(i - 0.1f * screenRect.w) / (0.8f * screenRect.w)), a, n);
			Uint8 r, g, b;
			rainbowColor((float)i / (0.8f * screenRect.w), r, g, b);
			SDL_SetRenderDrawColor(renderer, r, g, b, 255);
			SDL_RenderDrawLine(renderer, i, (int)y + threeHbyFive, i, threeHbyFive);
		}

		// drawing the function image with the function written
		SDL_RenderCopy(renderer, functionImage, NULL, &functionImageRect);

		

		// PRESENTING THE SCREEN
		SDL_RenderPresent(renderer);
	}

	return 0;
}

bool IsPointInRect(int x, int y, const SDL_Rect& rect) {
	return (x <= rect.x + rect.w && x >= rect.x && y >= rect.y && y <= rect.y + rect.h);
}

float Function(float x, float a, int n) {
	float y = sqrt(2.0f / a) * sin(n * M_PI * x / a);
	return -y * y;
}

void rainbowColor(float value, Uint8 & r, Uint8& g, Uint8& b) {
	float red = 0;
	float green = 0;
	float blue = 0;
	float hue = 360.0f * value;

	float c = 1.0f;
	float x = c * (1 - std::abs(std::fmod(hue / 60.0f, 2) - 1));
	float m = 0.0f;

	if (hue >= 0 && hue < 60) {
		red = c;
		green = x;
		blue = 0;
	}
	else if (hue >= 60 && hue < 120) {
		red = x;
		green = c;
		blue = 0;
	}
	else if (hue >= 120 && hue < 180) {
		red = 0;
		green = c;
		blue = x;
	}
	else if (hue >= 180 && hue < 240) {
		red = 0;
		green = x;
		blue = c;
	}
	else if (hue >= 240 && hue < 300) {
		red = x;
		green = 0;
		blue = c;
	}
	else {
		red = c;
		green = 0;
		blue = x;
	}

	red = std::max(0.0f, std::min(1.0f, red));
	green = std::max(0.0f, std::min(1.0f, green));
	blue = std::max(0.0f, std::min(1.0f, blue));

	red += m;
	green += m;
	blue += m;

	red *= 255;
	green *= 255;
	blue *= 255;

	r = red;
	g = green;
	b = blue;
}