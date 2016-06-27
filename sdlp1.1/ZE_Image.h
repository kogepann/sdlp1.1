#pragma once
#include "ZE_TextureContainer.h"
#include "ZE_Timer.h"

using namespace std;

//Image�Ĳ����ṹ��
struct textureStruct
{
	//���յ���ͼ
	shared_ptr<Texture> texture;
	//��ͼ�Ķ�ȡ��Ϣ
	SubTexture subData;
};

//ZE��ͼƬ�࣬������STARING��ͼƬ��һ��
class Image : public TextureContainer
{
public:
	//�������е���ͼ��ȡ����
	deque<textureStruct> subData;
	//��ǰ�����Ƿ��Ƕ�ͼ
	bool isMoveble = false;
	// TODO texture�費��Ҫʹ������ģʽ
	Image(weak_ptr<ZeroEngine> core_engine_weak_ptr, textureStruct texture);
	Image(weak_ptr<ZeroEngine> core_engine_weak_ptr, deque<textureStruct> textureData, unsigned int frameSpeed = 12);
	virtual int getWidth();
	virtual int getHeight();
	//SDL��blendmode�ƺ������࣬��źö඼���Լ�ʵ��
	void setBlendMode(SDL_BlendMode);
	virtual void Render();
	bool playState();
	void changeCurrentFrame(unsigned int);
	void play();
	void stop();

private:
	int currentFrame = 0;
	bool playing = false;
	unsigned int frame;
	Timer frameTimer;
};