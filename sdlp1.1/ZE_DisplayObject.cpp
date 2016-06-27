#include <iostream>
#include "ZE_DisplayObject.h"
#include "ZE_Core.h"
#include "ZE_Error.h"

using namespace std;

DisplayObject::DisplayObject(weak_ptr<ZeroEngine> core_engine_weak_ptr)
	:EventDispatcher(core_engine_weak_ptr.lock()),
	index(core_engine.lock()->getNewDisplayObjectIndex())
{
}

DisplayObject::DisplayObject(weak_ptr<ZeroEngine> core_engine_weak_ptr, size_t index)
	: EventDispatcher(core_engine_weak_ptr.lock()),
	index{ index }
{
}

void DisplayObject::addChild(shared_ptr<DisplayObject> object)
{
	object->setParent(std::dynamic_pointer_cast<DisplayObject>(shared_from_this()));
	addedObjects.emplace(object->index, object);
}

void DisplayObject::setParent(weak_ptr<DisplayObject> parent)
{
	this->parent = parent;
}

SDL_Rect DisplayObject::setRenderRect(int childWidth, int childHeight)
{
	//���������ߵ�һ�δ���
	//���ȣ��������ڵ㣬��ȡ��Ӧ�����ŵı��������и��׵�ֵ
	int parentX = 0;
	int parentY = 0;
	double parentScaleX = 1;
	double parentScaleY = 1;
	auto mum = parent.lock();
	if (mum)
	{
		while (true)
		{
			//�Ȼ�ȡ���ڵİְֵ�ֵ
			parentX += mum->x;
			parentY += mum->y;
			parentScaleX *= mum->scaleX;
			parentScaleY *= mum->scaleY;
			//�����ǰ����ʹ����ˮƽ��ת�Ļ�
			if (mum->flip == SDL_FLIP_HORIZONTAL)
			{
				//���Ӽ�Ҳ���ó�ˮƽ��ת
				flip = SDL_FLIP_HORIZONTAL;
				//��ǰ����̨�����ȥһ�������ҵ�X*2
				//Ȼ��Ӧ���ټ�ȥһ����ȣ�����IMAGE�����Ѿ��Ӽ�����
				parentX += 0 - (x * 2);
			}
			//��ֱ��ת��ͬ��
			else if (mum->flip == SDL_FLIP_VERTICAL)
			{
				flip = SDL_FLIP_VERTICAL;
				parentY += 0 - (y * 2);
			}
			//��ת����ø�����ԭ�����겢��ֵ���Լ�
			if (mum->rotationAngle != 0)
			{
				this->rotationPoint.x = 0 - x;
				this->rotationPoint.y = 0 - y;
				this->rotationAngle = mum->rotationAngle;
			}
			//��Ҫ˵�����ǣ�ZENG�����Զ��ֻ����һ����ά������硣
			//���Զ�����ת+��ת+������һ��֧�ֲ��Ǻܺá�
			//������ΪSDLԭ��������̣���װΪ������������Ҫ��������ӵ��㷨��
			//˵ʵ�ڵ���ʵҲû���ĸ��ײ�ͼ��API֧����ô�ḻ
			//��������Ӧ��ûʲô���⣬������Ӧ�ûᰴ������ȷ����
			//��ת��Щȱ�ݣ��������´���
			//spr1.add(spr2); spr2.x = 100; spr1.flip;
			//�����޷�����ʵ�ֵģ����λ��
			//������Ҫ���꾡�ķ����͸����ӵ��㷨
			//���Ծ�����Ҫ����Ҫ��ת��spr������һ��spr
			//���һ��Ҫ�Ļ�����Ҳ��Ҫ��������������
			//��ΪSDL����Ҳ��֧�ָ�ֵscale������û�취������ֻ����ʱ������
			//��תҲ����������⣬�Ӽ�����Ƕ��spr��
			//���⣬ǧ��Ҫ�Ѷ�̬image��ת�������鴤�ɹ�
			//��̬image���������㷨�Ѿ��ܸ����ˣ��ټӵĻ����������������ˣ�out of my hand

			//���������⹹�ɵ�ԭ�����һ�£������parentX������ʽ�Ļ������Ǿͻ�����̨����Ϊԭ����з�ת����ת
			//��˵��ǰѭ���м����parentX��Ӧ���ǽ��в�������һ�㸸�������ݣ����ǲ��ǣ�����һֱ��˷��stage
			//�Է�ת��������������Ӧ��������
			//���ȣ�ѯ�ʸ����Ƿ�ת�����Ϊ����ִ�У�������һ��ѭ����˷����
			//����ĳһ��������Ϊ��ת�ˣ�������Ӧ�ðѸø����������Ӽ�������Ϊ��ת(��ǰ���㷨��û��ִ����һ����
			//������sprite��render�����м���ִ�б������Ĵ��룬�����˸����صĴ�λ��Ӧ�����㷨û�п�����ȫ)
			//Ȼ�󣬻�ȡ������ڸø��������꣬����ˮƽ��ת����Ӧ�û�ȡ���ڸø�������ϵ���X��Ȼ��
			//����������ĸ�ֵ����0 - ����Ե�X*2��������image�Լ���ȥһ���Լ��Ŀ�ȾͿ�����ȷ��ת��
			//���������ֵ��Ӧ����parentX����Ϊ�������Ǵ��Ӽ������ʣ����Ե�ִ�е������ʱ��parentXӦ�þ���
			//����������������X����������˷֮�����ջ����������̨��X������������˷��ʱ�򣬸��߲�ĸ���
			//û�б���ת������Ӧ�þͲ����ټ���ִ�з�ת�㷨�ˡ����ǲ�֪��Ϊʲô���ն�����˷����̨
			//��������������ˣ��������һ��Ҫ����������Ļ����Ͷϵ����һ���ķ�����

			//���⣬������ò�Ҫ������ת+��ת+���ţ�������̫����û�취һһ����̫����ʱ����
			//������תҲ�����ԭ��
			//���ܻ���ָ�����������������Ի��ǲ�Ҫ������

			//������ڵİְ�û�аְ��ˣ����˳�
			auto mump = mum->parent.lock();
			if (!mump)
			{
				break;
			}
			//����Ͱ����ڵİְֻ��ɰְֵİְ�
			else
			{
				mum = mump;
			}
		}
	}
	SDL_Rect renderRect =
	{
		(int)((x + parentX)*parentScaleX),
		(int)((y + parentY)*parentScaleY),
		(int)(childWidth*scaleX*parentScaleX),
		(int)(childHeight*scaleY*parentScaleY)
	};

	return renderRect;
}

void DisplayObject::removeFromParent(bool disposeMe)
{
	auto p = parent.lock();
	if (!p)
	{
		core_engine.lock()->ZE_error->PopDebugConsole_Warning("DisplayObject:" + this->name + " do not have a parent!");
	}
	else
	{
		p->removeChild(std::dynamic_pointer_cast<DisplayObject>(shared_from_this()), disposeMe);
	}
}

void DisplayObject::removeChild(shared_ptr<DisplayObject> targetChild, bool disposeIt)
{
	auto it = addedObjects.find(targetChild->index);
	if (it != addedObjects.end())
	{
		addedObjects.erase(it);
	}
	else
	{
		core_engine.lock()->ZE_error->PopDebugConsole_Error("Can't find Child:" + targetChild->name);
	}
}

void DisplayObject::dispose()
{
	addedObjects.clear();
}

DisplayObject::~DisplayObject()
{
	DisplayObject::dispose();
}
