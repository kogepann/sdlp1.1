#pragma once
#include <memory>
#include <array>
#include "ZE_Image.h"


class tuanzi
	:public Image
{
public:

	explicit tuanzi(weak_ptr<ZeroEngine> core_engine, const textureStruct& texture_struct);

	tuanzi(weak_ptr<ZeroEngine> core_engine, const deque<textureStruct>& texture_structs, unsigned frameSpeed);

	void registerEventListener();

	void do_inertia(shared_ptr<tuanzi>& my, SDL_Event evt);

	void check_inertia();

	// ��ʹû��ʹ��Ҳ�����ﴫ������ָ���Ա�֤���󲻱�����
	void left(shared_ptr<tuanzi>& my, SDL_Event evt);

	void right(shared_ptr<tuanzi>& my, SDL_Event evt);

	void up(shared_ptr<tuanzi>& my, SDL_Event evt);

	void down(shared_ptr<tuanzi>& my, SDL_Event evt);
private:
	std::array<int, 4> inertia{ 0,0,0,0 };
	int base_inertia{ 5 };
};
