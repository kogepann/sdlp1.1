#pragma once
#include <functional>
#include <SDL.h>
#include <atomic>
#include <set>
#include "ZE_EventDispatcher.h"
#include "ZE_EventContainer.h"
#include "ZE_Core.h"

using namespace std;




//�¼�ͳ����
//����eventDispatcher����ע�����ʱ�������ø����add����
//����������ָ�뽫�ᱻ������ͳ������������¼���ͶԱ�Ȼ�󰤸�����
class EventManager
{
public:
	EventManager(weak_ptr<ZeroEngine> core_engine);
	~EventManager();
	EventManager(const EventManager&) = delete;
	EventManager(EventManager&&) = delete;
	EventManager& operator=(const EventManager&) = delete;
	EventManager& operator=(EventManager&&) = delete;

	// �¼�������ú���
	void handleEvent();
	// ע��һ���¼������������ؼ��������
	// ������
	// dispatch_index  ��dispatchIndexDistributor��õı�ţ��˱����Ψһ�ģ�Ӧ��ֻ��dispatchIndexDistributor��ȡһ��
	// type  �¼�������
	// signedObject  ����
	// func  �¼�������
	size_t addEventFunction(size_t dispatch_index, EventMode event_mode, Uint32 type, function<void(SDL_Event)> func);
	// �Ƴ�ָ��������
	void removeEventOfIndex(size_t event_index);
	// �Ƴ�һ��Dispatch�����м�����
	void removeAllEventOfDispatch(size_t dispatch_index);
	// �Ƴ�һ��Dispatch������ָ�����͵ļ�����
	void removeAllEventOfDispatchAndType(size_t dispatch_index, Uint32 type);
	void removeAllEventOfDispatchAndModeType(size_t dispatch_index, EventMode type);
	void removeAllEventOfDispatchAndTypeAndMode(size_t dispatch_index, Uint32 type, EventMode event_mode);
	// �Ƴ����м�����
	void removeAllEvent();
	// dispatch�Һ��� �������ȡdispatch��index���
	size_t dispatchIndexDistributor();

	/*�ھ����ˣ��ܴ�ģ�һ�����ۺ����գ����ҵģ�ȷ����event���޸ķ���
	����starling�ķ�������ע��һ���¼���ʱ��Ӧ�ô���һ��������
	ʹ��C++�Ļ������Ӧ����Ҫ���뺯��ָ�롣
	Ȼ����������ľ����Ժܴ�����һ�������Ծ��ǣ���Ŀ�꺯��Ϊ��Ա����ʱ�޷����ã�
	������ΪCpp�е���һ�㺯�����Ա�����ķ�����ͬ��
	��Ϊ����취��ȫ��������������Կ�ʼѰ�ҽ��������

	֮��֪��������������βε�ʱ�����һ��className::������ȷʹ�ã�Ȼ������취���ʺ��ҡ�
	��Ϊ��ʵ���Ҵ���ķ�����eventdispatcher����ķ�������������������ED��
	�ڵ㲦֮��֪����C11��������function��

	����ʹ��function���ݷ�����Ȼ��function��װ��Ա�����ܸ��ӣ����Ը�Ϊʹ��lambda���ʽ��������������
	���һ��ʵ������ȷ�ĵ��úʹ��ݣ�������Ϊfunction������̬���޷���������жϣ�
	�����û����޷�ʹ��ע��ʱ�ĺ����������Ƴ���ʹ��ָ��Ҳ���У�function�޷�ͨ��ָ����ȷ���á�

	Ȩ�������Ժ󣬾����Ƴ�removeEventListener�������û�����ע�᲻ͬ�ķ�������ͬһ���¼���
	���Ƴ���ʱ��ֻ�ܰ��¼������Ƴ����м����¼���

	��һ���ռ������������EventDispatcher��������(���ű���)��һ�������¼����麯����
	Ȼ��add����ֻ���ݸ�eventHandlerһ��thisָ�롣
	������ע���¼���Ԫ�ض���д���麯����Ȼ��eventHandler���ᰴ�¼����͵���������ע��ĺ�����
	����������ȶ��ȫ�����������Ϊÿһ���¼���Ҫ��Ӧһ���麯����
	���ҵķ�������ֱ��ʹ��ԭ��SDL�¼���Ҳ����ֻҪSDL֧���ҵķ�����֧�֡�
	cocos�õľ�������취������cocos
	д�����ﵱ����ѡ����

	BTW�����������ˣ������۵ò���*/

	/*Ҳ��֪��д���ģ���д�������
	SDL�ܻ���������ZEĿǰ��������¼���֧�ֲ��Ǻܺ�
	��Ϊ���е��¼������ɸ���ͳһ���ȴ���ģ�Ҳ����˵
	��һ������¼�������ʱ����ֻ����������ע��ķ����ɷ����¼�
	Ҳ����˵������ͼ��ı�����Σ���ʹAԪ�ر�BԪ�ظ��ţ�AԪ��Ҳ����յ�����¼�
	���仰˵�����ǲ����ڡ���ס��ꡱ�ĸ���
	���������ʵ�ָо���ܸ��ӣ���Ϊ��Ŀǰ��״����������ֻ���ڴ���ʱ������������һ��������̨Ԫ�أ�
	����֪��˭��ס��˭�����߷������events���飬Ȼ��ֻ�ɷ�һ��֮���
	��֮Ŀǰ�Ȳ�����*/

	// Jeremie:
	// ������¼��϶�Ҫ�����ڱε����⣬�����ķ����Ƕ����ж������ڵ���ʽ�����ѵ�����Ȼ�������ϱ���������Ӧ�¼��Ķ���
	// ������������ʺ�����Ҿ��ÿ����ֶ�ͼ��ֲ㣬Ȼ�󰴲�ɸѡ�������Ӧ�¼��Ķ���
	// ���߸�������Ҫ��Ӧ����¼��Ķ�����ͬ������ײ��һ������һ���¼���Ӧ�䣬Ȼ��ֱ�Ӳ鿴����㴦����Щ��Ӧ�䣬Ȼ����ݲ����ϵ�������¼�


private:
	// ����ָ��
	std::weak_ptr<ZeroEngine> core_engine;
	// ��ż�����
	std::atomic_size_t event_index{ 0 };
	// dispatch��ŷַ���
	std::atomic_size_t dispatch_index{ 0 };
	//�������е��¼�
	EventContainer AllEvents;
	// �¼�����   ������������Ԫ�ؼ������м��������ڼ������¼�
	std::set<Uint32> RegistedEvent;
	// ����״̬����
	std::set<SDL_Keycode> RegistedKeyboardState;
};