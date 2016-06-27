#include <algorithm>
#include <SDL_image.h>
#include "ZE_Core.h"
#include "tinyxml2.h"
#include "ZE_AssetManager.h"
#include "ZE_Error.h"
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace tinyxml2;

void AssetManager::Init(string xmlPath)
{
	XMLDocument doc;
	if (XML_NO_ERROR != doc.LoadFile(xmlPath.c_str()))
	{
		throw std::runtime_error("XMLDocument LoadFile ERROR");
	}
	XMLElement* docroot = doc.RootElement();
	XMLElement* sub = docroot->FirstChildElement("element");
	while (sub)
	{
		string type;
		string name;
		string path;
		string xml;

		const XMLAttribute* attr = sub->FirstAttribute();
		type = attr->Value();
		attr = attr->Next();
		name = attr->Value();
		attr = attr->Next();
		path = attr->Value();
		attr = attr->Next();
		xml = attr->Value();

		if (type == "image")
			LoadTexture(name, path, xml);
		else if (type == "ttf")
			LoadTTF(name, path);
		else if (type == "music" || type == "sound")
		{
			bool isMusic = false;
			if (type == "music")
				isMusic = true;
			LoadSound(isMusic, name, path);
		}

		sub = sub->NextSiblingElement();
	}
}

textureStruct AssetManager::getTexture(string name)
{
	textureStruct temp;
	for (unsigned int i = 0; i < TEXTURES.size(); i++)
	{
		if (name == TEXTURES[i]->name)
		{
			temp.subData = TEXTURES[i]->mySubInfo;
			temp.texture = TEXTURES[i];
			return temp;
			break;
		}
		for (unsigned int j = 0; j < TEXTURES[i]->subTextures.size(); j++)
		{
			if (name == TEXTURES[i]->subTextures[j].name)
			{
				temp.subData = TEXTURES[i]->subTextures[j];
				temp.texture = TEXTURES[i];
				return temp;
				break;
			}
		}
	}
	core_engine.lock()->ZE_error->PopDebugConsole_Error("Can't find Texture: " + name);
	return temp;
}

auto AssetManager::getTTFTexture(string text, string name, int size, SDL_Color color, int* width, int* height, unsigned int effectLevel)
->std::unique_ptr<SDL_Texture, decltype(SDL_DestroyTexture)*>
{
	auto usingfont = getFont(name);
	TTF_Font* tempfont = usingfont->getFont(size);
	std::unique_ptr<SDL_Surface, decltype(SDL_FreeSurface)*> tempsur{ nullptr,SDL_FreeSurface };
	if (effectLevel == 0)
		tempsur.reset(TTF_RenderUTF8_Solid(tempfont, text.c_str(), color));
	else if (effectLevel == 1)
		tempsur.reset(TTF_RenderUTF8_Shaded(tempfont, text.c_str(), color, { 0, 0, 0 }));
	else if (effectLevel == 2)
		tempsur.reset(TTF_RenderUTF8_Blended(tempfont, text.c_str(), color));
	if (!tempsur)
	{
		core_engine.lock()->ZE_error->PopDebugConsole_SDL_ttfError("Unable to render text surface!");
		return std::unique_ptr<SDL_Texture, decltype(SDL_DestroyTexture)*>{nullptr, SDL_DestroyTexture};
	}
	return Surface2SDLTexture(core_engine, tempsur, width, height);
}

shared_ptr<Font> AssetManager::getFont(string name)
{
	for (auto i : FONT)
	{
		if (name == i->name)
		{
			return i;
		}
	}
	core_engine.lock()->ZE_error->PopDebugConsole_Error("Can't find font name:" + name);
	return nullptr;
}

shared_ptr<Sound> AssetManager::getSound(string name)
{
	for (auto i : SOUNDS)
	{
		if (name == i->name)
		{
			return i;
		}
	}
	core_engine.lock()->ZE_error->PopDebugConsole_Error("Can't find sound name:" + name);
	return nullptr;
}

//���ڶԱ������ṹ��Ĵ�С����getTextures����ʹ�ã���AС��B������true
//��Ϊsort�ƺ����ܷ���������ĺ���������д������
bool comparTextureStruce(textureStruct a, textureStruct b)
{
	return a.subData.name < b.subData.name;
}

deque<textureStruct> AssetManager::getTextures(string partOfName)
{
	deque<textureStruct> temp;
	for (unsigned int i = 0; i < TEXTURES.size(); i++)
	{
		textureStruct oneOfTemp;
		if (TEXTURES[i]->name.find(partOfName) == 0)
		{
			oneOfTemp.subData = TEXTURES[i]->mySubInfo;
			oneOfTemp.texture = TEXTURES[i];
			temp.push_back(oneOfTemp);
		}
		else
		{
			for (unsigned int j = 0; j < TEXTURES[i]->subTextures.size(); j++)
			{
				if (TEXTURES[i]->subTextures[j].name.find(partOfName) == 0)
				{
					oneOfTemp.subData = TEXTURES[i]->subTextures[j];
					oneOfTemp.texture = TEXTURES[i];
					temp.push_back(oneOfTemp);
				}
			}
		}
	}
	if (temp.size() == 0)
	{
		core_engine.lock()->ZE_error->PopDebugConsole_Error("Can't find Texture has part of name: " + partOfName);
	}
	sort(temp.begin(), temp.end(), comparTextureStruce);
	return temp;
}

void AssetManager::LoadTexture(string name, string path, string xml)
{
	/*֮ǰ��TEXTURES��һ��һ��������������ָ�����顣
	�����������������һ���ֲ�Texture������Init��push��TEXTURE������
	��������ͼ�ᶪʧ��
	�Ҳ��룬������Ϊ�ֲ�����Texture�ڷ��������󼴱����٣�����push��TEXTURE��Ԫ����һ������Ұָ��Ĵ��ڡ�
	�����Ҳ��Ϊʲô��Ҷ���ָ���ԭ�򣬾ͺ�������A�������һ��¥��������B�������ܲ���˵��¥���ȥ��B����
	���Ծ���һ��ָ�룬����B��ַ�����Լ������ͺ��ˡ�*/

	auto temp_texture = make_shared<Texture>();

	//��֮��߱����SURFACE��ȡ����������������������S2T������ָ�봫����
	//ֱ�ӰѺ�������INIT��Ļ����޷���ȷ����WH���������Ե����ó�����ֵһ�顣
	int tempWidth = 0;
	int tempHeight = 0;

	//����һ������ͼ���飬���ڴ��ݸ���ͼ��
	deque <SubTexture> subtexture = SubXmlReader(xml);

	//��������Ķ�ȡ��ַ�������һ��surfaceָ�봫�ݸ�S2T����ʼ��
	auto tempsur = ImageReader(path);
	auto temptex = Surface2SDLTexture(core_engine, tempsur, &tempWidth, &tempHeight);
	temp_texture->Init(name, temptex, tempWidth, tempHeight, subtexture);
	// ����ָ��
	TEXTURES.push_back(temp_texture);
}

void AssetManager::LoadTTF(string name, string path)
{
	FONT.emplace_back(new Font(core_engine, name, path));
}

void AssetManager::LoadSound(bool isMusic, string name, string path)
{
	if (isMusic)
	{
		Mix_Music* music = Mix_LoadMUS(path.c_str());
		SOUNDS.emplace_back(new Sound(core_engine, name, music));
	}
	else
	{
		Mix_Chunk* sound = Mix_LoadWAV(path.c_str());
		SOUNDS.emplace_back(new Sound(core_engine, name, sound));
	}
}

auto AssetManager::Surface2SDLTexture(
	weak_ptr<ZeroEngine> core_engine,
	std::unique_ptr<SDL_Surface, decltype(SDL_FreeSurface)*>& surface,
	int* getW, int* getH
)->std::unique_ptr<SDL_Texture, decltype(SDL_DestroyTexture)*>
{
	std::unique_ptr<SDL_Texture, decltype(SDL_DestroyTexture)*> newTexture{
		SDL_CreateTextureFromSurface(core_engine.lock()->g_ZE_MainRenderer.get(), surface.get()),
		SDL_DestroyTexture
	};
	//��Surfaceת��ΪTexture
	//ֱ�Ӵ�����ͼҲ�У�����Ҫ�Ĳ������Ӻܶ࣬����ֱ��ת��
	if (!newTexture)
	{
		core_engine.lock()->ZE_error->PopDebugConsole_SDLError("Unable to creat texture!");
	}
	//��ȡ��ߣ���ָ�봫��ȥ
	*getW = surface->w;
	*getH = surface->h;
	return newTexture;
}


// FIXME ���ﷵ�صĿ�ָ����ܵ��¹�����ɢ  �Ƿ���Ҫ����
auto AssetManager::ImageReader(string path)->std::unique_ptr<SDL_Surface, decltype(SDL_FreeSurface)*>
{
	// ȡ���һ��С���㿪ʼΪ��չ�� ��ת��ΪСд�Ա�Ա�
	size_t last_point = path.find_last_of('.');
	if (string::npos == last_point)
	{
		core_engine.lock()->ZE_error->PopDebugConsole_Error("file ext canot find : " + path);
		throw std::runtime_error("file ext canot find : " + path);
		//return  nullptr;
	}
	++last_point;
	// ע��to_lower_copyֻ�����õ�ASCII��  UTF16/32�������
	string temp = boost::algorithm::to_lower_copy(path.substr(last_point, path.length() - last_point));


	std::unique_ptr<SDL_Surface, decltype(SDL_FreeSurface)*> tempSurface{ nullptr,SDL_FreeSurface };

	if (temp == "bmp")
	{
		//����BMP�ķ���ʹ��SDLԭ�����뺯����SDLֻ������BMP
		tempSurface.reset(SDL_LoadBMP(path.c_str()));
		//��Ϊ�Ǵ�C��������string����������׸��
		if (!tempSurface)
		{
			core_engine.lock()->ZE_error->PopDebugConsole_SDLError("Unable to load bmp file:" + path + " |Load error");
		}
	}
	else if (temp == "png")
	{
		//SDLֻ������BMP��PNG����SDL_IMAGE
		tempSurface.reset(IMG_Load(path.c_str()));
		if (!tempSurface)
		{
			core_engine.lock()->ZE_error->PopDebugConsole_SDL_ImageError("Unable to load png file:" + path + " |Load error");
		}
	}
	else
	{
		core_engine.lock()->ZE_error->PopDebugConsole_Error("Unable to read " + temp + " file.");
		//if�Ƕ�û��ס�Ǿ���û��ĸ�ʽ��...
	}

	return tempSurface;
}

deque<SubTexture> AssetManager::SubXmlReader(string xml)
{
	deque<SubTexture> temp;
	if (xml == "")
	{
		return temp;
	}
	else
	{
		XMLDocument doc;
		doc.LoadFile(xml.c_str());
		XMLElement* resourses = doc.RootElement();
		XMLElement* texture = resourses->FirstChildElement("SubTexture");
		while (texture)
		{
			SubTexture tempSub;
			const XMLAttribute* attr = texture->FirstAttribute();
			tempSub.name = attr->Value();
			attr = attr->Next();
			tempSub.x = attr->IntValue();
			attr = attr->Next();
			tempSub.y = attr->IntValue();
			attr = attr->Next();
			tempSub.width = attr->IntValue();
			attr = attr->Next();
			tempSub.height = attr->IntValue();
			attr = attr->Next();
			if (!attr)
			{
				tempSub.frameX = 0;
				tempSub.frameY = 0;
				tempSub.frameWidth = tempSub.width;
				tempSub.frameHeight = tempSub.height;
			}
			else
			{
				tempSub.frameX = attr->IntValue();
				attr = attr->Next();
				tempSub.frameY = attr->IntValue();
				attr = attr->Next();
				tempSub.frameWidth = attr->IntValue();
				attr = attr->Next();
				tempSub.frameHeight = attr->IntValue();
			}
			temp.push_back(tempSub);
			texture = texture->NextSiblingElement();
		}
		return temp;
	}
}

AssetManager::AssetManager(weak_ptr<ZeroEngine> core_engine)
	:core_engine(core_engine)
{
}

AssetManager::~AssetManager()
{
	dispose();
}

void AssetManager::dispose()
{
	DeleteAllTextures();
	DeleteAllFonts();
	DeleteAllSounds();
}

void AssetManager::DeleteAllSounds()
{
	SOUNDS.clear();
}

void AssetManager::DeleteAllFonts()
{
	FONT.clear();
}

void AssetManager::DeleteAllTextures()
{
	// ����ָ���ʱ�� �Ը�����ָ�����鶼��ֱ��clear��
	TEXTURES.clear();
}