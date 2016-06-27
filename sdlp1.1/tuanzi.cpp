#include "tuanzi.h"
#include "mygame.h"
#include "ZE_Sprite.h"
#include "ZE_DisplayObject.h"
#include "ZE_EventContainer.h"

void tuanzi::registerEventListener()
{

	// �����ﴫ������ָ�� ������ж��������ָ���Ա�֤���󲻱���ǰ�������ұ�֤�����ܹ�������
	// ��Ҫ����������ָ��ת��Ϊ��������ָ���Ա���������ҵ���Ӧ�ĺ���
	core_engine.lock()->ZE_stage->addEventListener(EventMode::KeyboardStateMode, SDL_SCANCODE_UP, std::bind(&tuanzi::up, this, std::dynamic_pointer_cast<tuanzi>(shared_from_this()), std::placeholders::_1));
	core_engine.lock()->ZE_stage->addEventListener(EventMode::KeyboardStateMode, SDL_SCANCODE_DOWN, std::bind(&tuanzi::down, this, std::dynamic_pointer_cast<tuanzi>(shared_from_this()), std::placeholders::_1));
	core_engine.lock()->ZE_stage->addEventListener(EventMode::KeyboardStateMode, SDL_SCANCODE_LEFT, std::bind(&tuanzi::left, this, std::dynamic_pointer_cast<tuanzi>(shared_from_this()), std::placeholders::_1));
	core_engine.lock()->ZE_stage->addEventListener(EventMode::KeyboardStateMode, SDL_SCANCODE_RIGHT, std::bind(&tuanzi::right, this, std::dynamic_pointer_cast<tuanzi>(shared_from_this()), std::placeholders::_1));
	core_engine.lock()->ZE_stage->addEventListener(EventMode::EveryLoop, 0, std::bind(&tuanzi::do_inertia, this, std::dynamic_pointer_cast<tuanzi>(shared_from_this()), std::placeholders::_1));

}

tuanzi::tuanzi(weak_ptr<ZeroEngine> core_engine, const textureStruct& texture_struct)
	:Image(core_engine, texture_struct)
{
}

tuanzi::tuanzi(weak_ptr<ZeroEngine> core_engine, const deque<textureStruct>& texture_structs, unsigned frameSpeed)
	: Image{ core_engine, texture_structs, frameSpeed }
{
}

void tuanzi::do_inertia(shared_ptr<tuanzi>& my, SDL_Event evt)
{
	if (inertia[0] > 1)
	{
		x -= inertia[0]--;
	}
	if (inertia[1] > 1)
	{
		x += inertia[1]--;
	}
	if (inertia[2] > 1)
	{
		y -= inertia[2]--;
	}
	if (inertia[3] > 1)
	{
		y += inertia[3]--;
	}
	check_inertia();
}

void tuanzi::check_inertia()
{
	for (auto& a : inertia)
	{
		if (a > 10)
		{
			a = 10;
		}
	}
}

void tuanzi::left(shared_ptr<tuanzi>& my, SDL_Event evt)
{
	inertia[0] += base_inertia;
	flip = SDL_FLIP_NONE;
}

void tuanzi::right(shared_ptr<tuanzi>& my, SDL_Event evt)
{
	inertia[1] += base_inertia;
	flip = SDL_FLIP_HORIZONTAL;
}

void tuanzi::up(shared_ptr<tuanzi>& my, SDL_Event evt)
{
	inertia[2] += base_inertia;
}

void tuanzi::down(shared_ptr<tuanzi>& my, SDL_Event evt)
{
	inertia[3] += base_inertia;
}
