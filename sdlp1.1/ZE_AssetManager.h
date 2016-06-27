#pragma once
#include <memory>
#include "ZE_Texture.h"
#include "ZE_Image.h"
#include "ZE_Font.h"
#include "ZE_Sound.h"

using namespace std;

//����starling����Դ������
//���û���ʹ����Դʱ��Ӧ��������һ������Ķ���
class AssetManager
	:public std::enable_shared_from_this<AssetManager>
{
public:
	explicit AssetManager(weak_ptr<ZeroEngine> core_engine);
	~AssetManager();
	//�÷�������һ��xml�ļ���ַ���ļ��б�ע�˸ö�����Ҫ�����ȫ��ͼƬ�ļ��ĵ�ַ
	//xml�ļ���ʽ����
	//<?xml version="1.0"?>
	//    <resourses name="testxml">
	//    <texture name="background" path="data/images/background.png" xml="data/images/tinylight.xml">
	//    <texture name="pngtest" path="data/images/pngtest.png" xml="data/images/tinylight.xml">
	//    <texture name="foo" path="data/images/foo.png" xml="data/images/tinylight.xml">
	//</resourses>
	void Init(string xmlPath);
	//��ȡһ����ע���texture������subtexture
	textureStruct getTexture(string name);
	//��ȡ��ע���������ɵ�һ����ͼ
	auto getTTFTexture(
		string text, string name, int size, SDL_Color color,
		int* width, int* height,
		unsigned int effectLevel = 0
	)->std::unique_ptr<SDL_Texture, decltype(SDL_DestroyTexture)*>;
	//��ȡһ����������
	shared_ptr<Sound> getSound(string name);
	//��ȡ������ע���texture
	deque<textureStruct> getTextures(string partOfName);
	//�÷�����һ��surfaceָ��ת��ΪSDLtextureָ�벢����
	static auto Surface2SDLTexture(
		weak_ptr<ZeroEngine> core_engine,
		std::unique_ptr<SDL_Surface, decltype(SDL_FreeSurface)*>& surface,
		int* getW, int* getH
	)->std::unique_ptr<SDL_Texture, decltype(SDL_DestroyTexture)*>;
	//���
	void dispose();

private:
	//���е���ͼ������AM����ͳһ���д������������٣�������Ϊ����������ע����ͼ������
	// �Ѿ�ʹ������ָ���Զ�������Դ������
	deque<shared_ptr<Texture>> TEXTURES;
	//�����������
	deque<shared_ptr<Font>> FONT;
	//������������
	deque<shared_ptr<Sound>> SOUNDS;

	//��ȡ����������ͼ���ݵķ���������XML�ļ���ַ��Ϊ����
	deque<SubTexture> SubXmlReader(string xml);
	//����ͼƬ�ķ���������ͼƬӲ�̵�ַ��Ϊ����
	void LoadTexture(string name, string path, string xml);
	//��������
	void LoadTTF(string name, string path);
	//��������
	void LoadSound(bool isMusic, string name, string path);
	//�ֱ�ͼƬ��ʽ�ķ�����������һ��Surfaceָ��
	//�û����õ��ô˷���
	auto ImageReader(string)->std::unique_ptr<SDL_Surface, decltype(SDL_FreeSurface)*>;
	//��ȡ������һ��FONT����
	shared_ptr<Font> getFont(string);

	//ɾ������ע���Texture
	void DeleteAllTextures();
	//ɾ������ע���font
	void DeleteAllFonts();
	//ɾ������ע���sound
	void DeleteAllSounds();

private:
	weak_ptr<ZeroEngine> core_engine;
};