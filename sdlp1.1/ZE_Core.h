#pragma once
#include <iostream>
#include <memory>
#include <deque>
#include <atomic>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

using namespace std;

class Fraps;
class Sprite;
class Error;
class EventManager;
class Controller;
class Font;
class ZeroEngine;

//��ɫ�ṹ�壬������̨��ɫ��ʱ��ʹ��
struct Color
{
	uint8_t red;
	uint8_t green;
	uint8_t blue;
};

//��Game�࣬�û�Ӧ�̳д��ಢ��д����
class Game
	:public std::enable_shared_from_this<Game>
{
public:
	// ��ʼ��ʱ��Ҫ���ʺ����������ݵĲ���
	virtual void Init(weak_ptr<ZeroEngine> core_engine) = 0;
	// ����Ⱦѭ��
	virtual void MainLoop() = 0;
	// ��Ҫ������ر�ǰ�رյĲ���
	virtual void Close() = 0;
	virtual ~Game() {}
};

// ������ĳ�ʼ������
struct ZeroEngineInitArgs
{
	string title{ "ZeroEngine" };
	int windowWidth{ 800 };
	int windowHeight{ 600 };
	bool useVSync{ false };
	std::string defaultFontFile{ "data/ttf/SourceHanSansSC-Normal.otf" };
	Color stageColor{ 0,0,0 };
};



// �������
class ZeroEngine
	: public std::enable_shared_from_this<ZeroEngine>
{
public:
	explicit ZeroEngine();
	~ZeroEngine();
	// ��ֹ�ƶ������뿽������
	ZeroEngine(const ZeroEngine&) = delete;
	ZeroEngine(ZeroEngine&&) = delete;
	ZeroEngine& operator=(const ZeroEngine&) = delete;
	ZeroEngine& operator=(ZeroEngine&&) = delete;

	//��ʼ��������ʹ��ZEǰ������ô˷���
	//�÷��������˱����е����������������ֱ��ʼ�����ⲿ��SDL��SDL_IMAGE
	//�÷����᷵��һ��boolֵ��������ģ���ʼ���ɹ��򷵻�true�����򷵻�false
	//�ⲿ��ı���д�ڶ�Ӧ������
	bool Init(ZeroEngineInitArgs& init_args, std::shared_ptr<Game>& userGame);
	//�÷�����ZE����ѭ��������Ŀǰ�����ð�
	void Run();
	// �Һ�
	size_t getNewDisplayObjectIndex();

private:
	//SDL�ĳ�ʼ���������÷��������������ֱ��Ǵ��ڱ��⡢���ڿ�ȡ����ڸ߶�
	bool Init_SDL(string, int, int);
	//��ʼ����SDL_IMAGE��������
	bool Init_SDL_Image(bool);
	//��ʼ����SDL_TTF��������
	bool Init_SDL_ttf() const;
	//��ʼ����Ƶ��
	bool Init_SDL_Mixer() const;
	//����
	void Close();

private:
	//��̨��ɫ��Ĭ��Ϊ��
	Color stageColor{ 0, 0, 0 };
	//�����Ⱦ����̨�ϣ���ʾһЩ���ӱ���
	std::shared_ptr<Fraps> fraps;
	//�û�����Ϸ����
	std::shared_ptr<Game> userGame;
	// ��ʾ����index������
	std::atomic_size_t display_object_index{ 0 };


public:

	//[Global]����SDL�����ָ��
	std::unique_ptr<SDL_Window, decltype(SDL_DestroyWindow)*> g_ZE_Window{ nullptr,SDL_DestroyWindow };
	//[Global]����SDL��Surface��ָ��
	std::unique_ptr<SDL_Surface, decltype(SDL_FreeSurface)*> g_ZE_MainSurface{ nullptr,SDL_FreeSurface };
	//[Global]����Ⱦ�����󶨵���window
	std::unique_ptr<SDL_Renderer, decltype(SDL_DestroyRenderer)*> g_ZE_MainRenderer{ nullptr,SDL_DestroyRenderer };
	//[Global]stage����Ψһ
	std::shared_ptr<Sprite> ZE_stage;
	//[Global]error����(Ӧ����Ψһ�������������Ҳ��private)
	std::unique_ptr<Error> ZE_error;
	//[Global]�¼�����������Ψһ
	std::unique_ptr<EventManager> ZE_eventHandler;
	//[Global]�˳���ѭ�����ж�����
	std::atomic_bool ZE_QUIT_MAIN_LOOP{ false };
	//[Global]�������е��ֱ�ָ��
	std::deque<std::unique_ptr<Controller>> ZE_Controllers;
	//[Global]ϵͳĬ������
	std::shared_ptr<Font> defaultFont;
};