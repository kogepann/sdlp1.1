////////////////////////////////////////////////////////////
//���е������ʾ��������ע�Ͷ�д����/����/������������һ��//
//˵���׳���֮��Ҳ��һ�������������������...             //
////////////////////////////////////////////////////////////

#include "ZE_Core.h"

#ifdef _DEBUG
#include <vld.h>
#endif // _DEBUG

#include "mygame.h"

using namespace std;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;


int main(int argc, char* args[])
{
	// ����������ʼ��
	ZeroEngineInitArgs engine_arg;
	engine_arg.stageColor = { 0,0,0 };
	engine_arg.title = u8"��TM������һ��SDL���ڻ���һ��TM��Ӣ��ȫ��ǻ�ϱ���Ĵ����Բۣ�!��";
	engine_arg.windowWidth = SCREEN_WIDTH;
	engine_arg.windowHeight = SCREEN_HEIGHT;
	engine_arg.useVSync = true;

	// �������Ķ���
	auto engine = make_shared<ZeroEngine>();

	{
		// �����û�����
		auto game = make_shared<mygame>();

		auto temp = std::dynamic_pointer_cast<Game>(game);
		engine->Init(engine_arg, temp);
		engine->Run();
	}

	return 0;
}