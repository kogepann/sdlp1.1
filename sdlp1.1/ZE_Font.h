#pragma once
#include <string>
#include <deque>
#include <memory>
#include <array>
#include <unordered_map>
#include <SDL_Extension/SDL_ttf.h>

using namespace std;

class ZeroEngine;

class Font
	: public enable_shared_from_this<Font>
{
public:
	Font(weak_ptr<ZeroEngine> core_engine, string name, string path);
	~Font();
	// ������������ָ��
	TTF_Font* getFont(int size = 15);
	string name;

private:
	weak_ptr<ZeroEngine> core_engine;
	string mPath;
	//����������������治ͬ���ֺ�
	unordered_map<int, TTF_Font*> mFont;

	void setNewSizeOfFont(int);
};