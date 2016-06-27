#pragma once
#include <SDL.h>
#include "ZE_AssetManager.h"
#include "ZE_TextureContainer.h"

using namespace std;

class TextField : public TextureContainer
{
public:
	string Text;
	unsigned int EffectLevel;
	int FontSize;
	SDL_Color TextColor;
	std::weak_ptr<AssetManager> Ass;
	string FontName;

	explicit TextField(
		weak_ptr<ZeroEngine> core_engine,
		string text_f,
		unsigned int effectLevel_f = 0,
		int fontSize_f = 20,
		SDL_Color textColor_f = { 0, 0, 0 },
		std::weak_ptr<AssetManager> ass_f = std::weak_ptr<AssetManager>(),
		string fontName_f = ""
	);
	~TextField();
	void changeText(string);
	int getWidth() override;
	int getHeight() override;
	weak_ptr<ZeroEngine> core_engine;
};