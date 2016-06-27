#include "ZE_EventManager.h"
#include "ZE_Core.h"

using namespace std;


EventManager::EventManager(weak_ptr<ZeroEngine> core_engine)
	:core_engine(core_engine)
{
}

EventManager::~EventManager()
{
}

void EventManager::handleEvent()
{
	SDL_Event e;
	while (SDL_PollEvent(&e) != 0)
	{
		if (e.type == SDL_QUIT)
		{
			core_engine.lock()->ZE_QUIT_MAIN_LOOP = true;
			return;
		}
		else
		{
			// �������Ϣģʽ
			if (!RegistedEvent.empty())
			{
				// �ȼ���Ƿ��б�ע��
				if (RegistedEvent.find(e.type) != RegistedEvent.end())
				{
					// ��ȡ�����͵�ԭ��������
					auto itp = AllEvents.get<EventContainerTag::EventTypeAndEventModeType>().equal_range(
						std::make_tuple(e.type, EventMode::RawEventMode)
					);
					// ����Ƿ񲢲�����
					if (itp.first == itp.second)
					{
						// ���������ʾRegistedEvent��ʱ�ˣ����������־λ
						RegistedEvent.erase(e.type);
					}
					else
					{
						// �������������Ҫ���������Ϣ�ļ�������ִ��
						while (itp.first != itp.second)
						{
							itp.first->event_data.func(e);
							++itp.first;
						}
					}
				}
			}
		}
	}
	// �����¼���ģʽ
	{
		const Uint8* currentKeyStates = SDL_GetKeyboardState(nullptr);
		auto it = RegistedKeyboardState.begin();
		while (it != RegistedKeyboardState.end())
		{
			if (currentKeyStates[*it])
			{
				auto itp = AllEvents.get<EventContainerTag::EventTypeAndEventModeType>().equal_range(
					std::make_tuple(*it, EventMode::KeyboardStateMode)
				);
				if (itp.first == itp.second)
				{
					// ���������ʾRegistedEvent��ʱ�ˣ����������־λ
					it = RegistedKeyboardState.erase(it);
					continue;
				}
				else
				{
					// �������������Ҫ���������Ϣ�ļ�������ִ��
					while (itp.first != itp.second)
					{
						itp.first->event_data.func(e);
						++itp.first;
					}
				}
			}
			++it;
		}
	}
	// ���
	{
		// TODO �������ģʽ
		if (
			e.type == SDL_MOUSEMOTION ||
			e.type == SDL_MOUSEWHEEL ||
			e.type == SDL_MOUSEBUTTONDOWN ||
			e.type == SDL_MOUSEBUTTONUP
			)
		{

		}
		//Get mouse position 
		int x, y;
		SDL_GetMouseState(&x, &y);
	}
	// EveryLoopÿһ���¼�ѭ�������и����¼�ִ�еĴ��룬������Լ��㡢���ƶ�����ˢ�� ֮���
	{
		auto itp = AllEvents.get<EventContainerTag::EventModeType>().equal_range(EventMode::EveryLoop);
		while (itp.first != itp.second)
		{
			itp.first->event_data.func(e);
			++itp.first;
		}
	}
}

size_t EventManager::addEventFunction(size_t dispatch_index, EventMode event_mode, Uint32 type, function<void(SDL_Event)> func)
{
	size_t index = ++event_index;
	EventData temp;
	temp.type = type;
	temp.func = func;
	temp.eventIndex = index;
	AllEvents.emplace_back(index, dispatch_index, event_mode, type, temp);
	switch (event_mode)
	{
	case EventMode::RawEventMode:
		RegistedEvent.insert(type);
		break;
	case EventMode::KeyboardStateMode:
		RegistedKeyboardState.insert(type);
		break;
	default:
		// any other
		break;
	}
	return index;
}

void EventManager::removeEventOfIndex(size_t event_index)
{
	AllEvents.get<EventContainerTag::Index>().erase(event_index);
}

void EventManager::removeAllEventOfDispatch(size_t dispatch_index)
{
	AllEvents.get<EventContainerTag::DispatchIndex>().erase(dispatch_index);
}

void EventManager::removeAllEventOfDispatchAndTypeAndMode(size_t dispatch_index, Uint32 type, EventMode event_mode)
{
	auto itp = AllEvents.get<EventContainerTag::DispatchIndexAndEventTypeAndEventModeType>().equal_range(
		std::make_tuple(dispatch_index, type, event_mode)
	);
	AllEvents.get<EventContainerTag::DispatchIndexAndEventTypeAndEventModeType>().erase(itp.first, itp.second);
}

void EventManager::removeAllEventOfDispatchAndType(size_t dispatch_index, Uint32 type)
{
	auto itp = AllEvents.get<EventContainerTag::DispatchIndexAndEventTypeAndEventModeType>().equal_range(
		std::make_tuple(dispatch_index, type)
	);
	AllEvents.get<EventContainerTag::DispatchIndexAndEventTypeAndEventModeType>().erase(itp.first, itp.second);
}

void EventManager::removeAllEventOfDispatchAndModeType(size_t dispatch_index, EventMode type)
{
	auto itp = AllEvents.get<EventContainerTag::EventModeType>().equal_range(type);
	AllEvents.get<EventContainerTag::EventModeType>().erase(itp.first, itp.second);
}

void EventManager::removeAllEvent()
{
	AllEvents.clear();
}

size_t EventManager::dispatchIndexDistributor()
{
	return ++dispatch_index;
}
