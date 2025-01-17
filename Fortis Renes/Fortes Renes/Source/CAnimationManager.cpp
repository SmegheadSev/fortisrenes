#include "CAnimationManager.h"
#include <fstream>
using namespace std;
#include "tSheet.h"
#include "tAnimation.h"
#include "tFrame.h"
#include "tAnimationInstance.h"


CAnimationManager* CAnimationManager::m_pAnimationInstance = NULL;

CAnimationManager* CAnimationManager::GetInstance(void)
{
	static CAnimationManager instance;
	return &instance;
}

void CAnimationManager::DeleteInstance(void)
{
	if(m_pAnimationInstance != NULL)
	{
		delete m_pAnimationInstance;
		m_pAnimationInstance = NULL;
	}
}

CAnimationManager::CAnimationManager(void)
{
	m_pTM = CSGD_TextureManager::GetInstance();
}

CAnimationManager::~CAnimationManager(void)
{
}

tAnimationInstance* CAnimationManager::LoadAnimation(const char* szFilePath) 
{
	tAnimationInstance* instance = NULL;
	
	fstream fs(szFilePath, ios_base::in | ios_base::binary);
	if (fs.is_open())
	{
		//fs.seekg(0, ios_base::end);
		char cStringLength;
		char* szStringBuffer;

		tSheet* sheet = new tSheet();
		instance = new tAnimationInstance();

		fs.read(&cStringLength, 1);
		szStringBuffer = new char[cStringLength+1];
		fs.read(szStringBuffer, cStringLength);
		szStringBuffer[cStringLength] = '\0';

		string sFilePath;
		sFilePath = "resource\\graphics\\";
		 
		sFilePath += szStringBuffer;

		sheet->szFilePath = sFilePath.c_str();

		delete[] szStringBuffer;

		int nNumAnimations = 0;
		int nNumFrames = 0;
		fs.read((char*)&nNumAnimations, sizeof(int));

		tAnimation* animation;
		tFrame* frame;
		for (int i = 0; i < nNumAnimations; i++)
		{
			animation = new tAnimation();

			fs.read(&cStringLength, 1);
			animation->szName = new char[cStringLength+1];
			fs.read(animation->szName, cStringLength);
			animation->szName[cStringLength] = '\0';

			//animation->szName = szStringBuffer;

			//delete[] szStringBuffer;

			float fFrameDuration;
			fs.read((char*)&fFrameDuration, sizeof(float));

			if(fFrameDuration == 0)
				fFrameDuration = 0.05f;

			fs.read((char*)&animation->bIsLooping, sizeof(bool));

			fs.read((char*)&nNumFrames, sizeof(int));
			for(int j = 0; j < nNumFrames; j++)
			{
				frame = new tFrame();

				fs.read((char*)&frame->rDraw.left, sizeof(int));
				fs.read((char*)&frame->rDraw.top, sizeof(int));
				fs.read((char*)&frame->rDraw.right, sizeof(int));
				fs.read((char*)&frame->rDraw.bottom, sizeof(int));

				fs.read((char*)&frame->rCollision.left, sizeof(int));
				fs.read((char*)&frame->rCollision.top, sizeof(int));
				fs.read((char*)&frame->rCollision.right, sizeof(int));
				fs.read((char*)&frame->rCollision.bottom, sizeof(int));

				fs.read((char*)&frame->bTrigger, sizeof(bool));
				
				fs.read(&cStringLength, 1);
				frame->szTriggerName = new char[cStringLength+1];
				fs.read(frame->szTriggerName, cStringLength);
				frame->szTriggerName[cStringLength] = '\0';

				//frame->szTriggerName = szStringBuffer;

				frame->fDuration = fFrameDuration;

				//delete[] szStringBuffer;

				fs.read((char*)&frame->nAnchorX, sizeof(int));
				fs.read((char*)&frame->nAnchorY, sizeof(int));

				animation->m_vFrames.push_back(frame);
			}

			sheet->m_vAnimations.push_back(animation);
		}

		sheet->ImageID = m_pTM->LoadTexture(sheet->szFilePath);

		instance->currSheet = sheet;
		instance->currAnimation = instance->currSheet->m_vAnimations[0];
		instance->currFrame = instance->currAnimation->m_vFrames[0];

		//m_vSheets.push_back(sheet);

		fs.close();
	}

	return instance;
}

void CAnimationManager::UnloadAnimation(tAnimationInstance* instance) 
{
	if( instance != NULL && instance->currAnimation != NULL && instance->currFrame != NULL && instance->currSheet != NULL )
	{
		for(unsigned int i = 0; i < instance->currSheet->m_vAnimations.size(); i++)
		{	
			instance->currAnimation = instance->currSheet->m_vAnimations[i];
			for(unsigned int j = 0; j < instance->currAnimation->m_vFrames.size(); j++)
			{
				if(instance->currAnimation->m_vFrames[j]->szTriggerName)
				{
					delete instance->currAnimation->m_vFrames[j]->szTriggerName;
					instance->currAnimation->m_vFrames[j]->szTriggerName = NULL;
				}
				delete instance->currAnimation->m_vFrames[j];
				instance->currAnimation->m_vFrames[j] = NULL;
				instance->currFrame = NULL;
			}
			instance->currAnimation->m_vFrames.clear();
		}
		instance->currAnimation = NULL;

		for(unsigned int i = 0; i < instance->currSheet->m_vAnimations.size(); i++)
		{
			tAnimation* temp = instance->currSheet->m_vAnimations[i];
			delete temp->szName;
			delete temp;
			instance->currSheet->m_vAnimations[i] = NULL;
		}
		instance->currSheet->m_vAnimations.clear();

		if(instance->currSheet != NULL)
		{
			delete instance->currSheet;
			instance->currSheet = NULL;
		}
		if(instance != NULL)
		{
			delete instance;
			instance = NULL;
		}
	}
}		