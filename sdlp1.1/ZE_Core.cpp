
#include <SDL_image.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include "ZE_Core.h"
#include "ZE_EventManager.h"
#include "ZE_Error.h"
#include "ZE_Fraps.h"
#include "ZE_Sprite.h"
#include "ZE_Font.h"
#include "ZE_Controller.h"

//[Global]ZE�汾��
std::string ZE_version = "2.0.0";

using namespace std;

ZeroEngine::ZeroEngine()
{
	// ���캯���в���ʹ��shared_from_this()
}

ZeroEngine::~ZeroEngine()
{
	//���Ĭ������
	defaultFont.reset();
	fraps.reset();
	ZE_stage.reset();
	ZE_eventHandler.reset();
	ZE_error.reset();
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
	//�˳�
	cout << "ZeroEngine::~ZeroEngine()" << endl;
}

bool ZeroEngine::Init(ZeroEngineInitArgs& init_args, std::shared_ptr<Game>& userGame)
{
	this->userGame = userGame;

	// �����캯��·û���������
	ZE_error.reset(new Error);
	ZE_eventHandler.reset(new EventManager(shared_from_this()));
	ZE_stage = make_shared<Sprite>(shared_from_this());
	this->stageColor = init_args.stageColor;


	if (!Init_SDL(init_args.title, init_args.windowWidth, init_args.windowHeight))
	{
		throw std::runtime_error("SDL Init Error.");
	}

	if (!Init_SDL_Image(init_args.useVSync))
	{
		throw std::runtime_error("SDL Image Init Error.");
	}

	if (!Init_SDL_ttf())
	{
		throw std::runtime_error("SDL Ttf Init Error.");
	}

	if (!Init_SDL_Mixer())
	{
		throw std::runtime_error("SDL Mixer Init Error.");
	}


	fraps = make_shared<Fraps>(shared_from_this(), this->getNewDisplayObjectIndex());

	// ��������
	defaultFont = make_shared<Font>(shared_from_this(), "default", init_args.defaultFontFile);


	// �ڲ���ʼ������  ��ʼ��ʼ���û���

	userGame->Init(shared_from_this());


	//function<void(SDL_Event)> addJoyStick = [&](SDL_Event evt)->void
	//{
	//	ZE_Controllers.push_back(make_unique<Controller>(SDL_NumJoysticks() - 1));
	//	//(new Controller(SDL_NumJoysticks() - 1));
	//};

	//function<void(SDL_Event)> removeJoyStick = [&](SDL_Event evt)->void
	//{
	//	//delete(ZE_Controllers[evt.jdevice.which]);
	//	//ZE_Controllers[evt.jdevice.which].reset();
	//	// FIXME ����ʹ���±���з��ʲ�����ĳ�������ըô
	//	ZE_Controllers.erase(ZE_Controllers.begin() + evt.jdevice.which);
	//};
	//ZE_stage->addEventListener(EventMode::RawEventMode, SDL_JOYDEVICEADDED, addJoyStick);
	//ZE_stage->addEventListener(EventMode::RawEventMode, SDL_JOYDEVICEREMOVED, removeJoyStick);

	return true;
}

bool ZeroEngine::Init_SDL(string Title, int windowWidth, int windowHeight)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC | SDL_INIT_AUDIO) < 0)
	{
		ZE_error->PopDebugConsole_SDLError("SDL could not initialize!");
		return false;
	}
	//�������壺���ڱ��⣬����Ĭ�����꣬���ڿ�ߣ����ڳ�ʼ״̬
	g_ZE_Window.reset(SDL_CreateWindow(Title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowWidth, windowHeight, SDL_WINDOW_SHOWN));
	if (!g_ZE_Window)
	{
		ZE_error->PopDebugConsole_SDLError("Window could not be created!");
		return false;
	}
	//��ȡ���ڵ���Surface�����еĻ��ƶ����������surface��
	g_ZE_MainSurface.reset(SDL_GetWindowSurface(g_ZE_Window.get()));

	return true;
}

bool ZeroEngine::Init_SDL_Image(bool useVSync)
{
	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) == imgFlags))
	{
		//����ֻ��ʼ����PNG��IMG�ĳ�ʼ���������᷵�ض�Ӧ��FLAG����ʾ�ɹ�
		//�в�֪������������������������ã���������ô��������������ʲô
		//����һ��SDL_IMAGE֧�ֶ��ָ�ʽ�治�Ǹǵģ����˳���JPG PNG֮���Ȼ��֧��GIF������ټ�������һЩ�����еĸ�ʽ
		//BTW��IMG_INIT_PNG == 2
		ZE_error->PopDebugConsole_SDL_ImageError("SDL_image could not initialize!");
		return false;
	}

	int renderFlag;
	if (useVSync)
		renderFlag = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
	else
		renderFlag = SDL_RENDERER_ACCELERATED;
	//��Ⱦ�������ѱ����������Ի�ȡ
	g_ZE_MainRenderer.reset(SDL_GetRenderer(g_ZE_Window.get()));
	if (g_ZE_MainRenderer == nullptr)
	{
		//��Ⱦ��֮ǰ��δ�����������ڴ���
		g_ZE_MainRenderer.reset(SDL_CreateRenderer(g_ZE_Window.get(), -1, renderFlag));
	}
	//��ʼ��������Ⱦ��
	if (g_ZE_MainRenderer == nullptr)
	{
		ZE_error->PopDebugConsole_SDLError("Renderer could not be created!");
		return false;
	}
	//������Ⱦ���Ļ�����ɫ
	//Ҫ����Ⱦ����Ĭ��ͼ�εĻ�����SDL_RENDERDRAWRECT�����Ҫ���ϵ��л�������ɫ��
	//����Ҫ��̨�ǰ�ɫ�ģ������Ǻ�ɫ�ģ���Ҫ�Ȼ�����̨���ٻ��췽�飬��һ��ѭ�����л��ذ�ɫ
	SDL_SetRenderDrawColor(g_ZE_MainRenderer.get(), stageColor.red, stageColor.green, stageColor.blue, 1);

	return true;
}

bool ZeroEngine::Init_SDL_ttf() const
{
	if (TTF_Init() == -1)
	{
		ZE_error->PopDebugConsole_SDL_ttfError("SDL_ttf could not initialize!");
		return false;
	}
	return true;
}

bool ZeroEngine::Init_SDL_Mixer() const
{
	if (Mix_Init(MIX_INIT_OGG | MIX_INIT_MP3) < 0)
	{
		ZE_error->PopDebugConsole_SDL_MixerError("SDL_mixer could not initialize!");
		return false;
	}
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		ZE_error->PopDebugConsole_SDL_MixerError("cuold not open audio on SDL_mixer!");
		return false;
	}
	return true;
}

void ZeroEngine::Run()
{
	fraps->setUp();
	while (!ZE_QUIT_MAIN_LOOP)
	{

		//�����¼�
		ZE_eventHandler->handleEvent();
		//���
		SDL_RenderClear(g_ZE_MainRenderer.get());
		//���߼�
		userGame->MainLoop();
		//�ػ�
		ZE_stage->Render();
		//��frapsִ���߼�
		fraps->setText();
		fraps->Render();
		//�ı��Ĭ����ɫ
		SDL_SetRenderDrawColor(g_ZE_MainRenderer.get(), stageColor.red, stageColor.green, stageColor.blue, 1);
		//��ˢ��
		SDL_RenderPresent(g_ZE_MainRenderer.get());
	}
	Close();
}

size_t ZeroEngine::getNewDisplayObjectIndex()
{
	return ++display_object_index;
}

void ZeroEngine::Close()
{
	//�����û��Ĺرշ���
	userGame->Close();
	// �ͷ��û�ָ�� �û���������ڴ�����
	userGame.reset();

	ZE_Controllers.clear();

	//������̨
	ZE_stage->dispose();

	// ���������ѭ��Ӧ�����ʼ��˳���෴
	g_ZE_MainSurface.reset();
	//ɾ����Ⱦ��
	g_ZE_MainRenderer.reset();
	//ɾ��SDL����
	g_ZE_Window.reset();
	Mix_CloseAudio();
	while (Mix_Init(0))
	{
		Mix_Quit();
	}


}
