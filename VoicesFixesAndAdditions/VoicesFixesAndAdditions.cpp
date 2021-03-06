// AutoDemo_RedMountain.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <cmath>



//Objects
#include "ObjModels\Objects.h"


//Misc
#include "IniFile.hpp"

//Replacement Functions


//Config Variables
static bool FinalEggAnnouncer = true;
static bool PerfectChaos = true;
static bool SkyDeck3Announcer = true;
static bool BetaRematch = true;
static bool SkyDeck2Announcer = true;
static bool SkyDeck1Announcer = true;
static bool KnucklesNoRing = true;
static bool AmyNoRing = true;
static bool TailsNoRing = true;
static bool BigNoRing = true;
static bool BarrierLoss = true;
static bool WaterStuff = true;
static bool LightSpeed = true;
static bool TailsFlight = true;
static bool KnucklesHurt = true;
static bool AmyHurt = true;
static bool FishingFailed = true;
static bool FinalEggColorDoor = true;
static bool ItemVoices = true;
static bool BoatNoise = true;

//Structs
struct ObjectThing
{
	ObjectFuncPtr func;
	int16_t list;
	int16_t field_A;
	Rotation3 Rotation;
	NJS_VECTOR Position;
	NJS_OBJECT* object;
};

//Additional SADX Variables
DataArray(CollisionData, stru_C67750, 0xC67750, 1);
DataArray(CollisionData, stru_C673B8, 0xC673B8, 7);



FunctionPointer(void, SetStatus, (ObjectMaster *a1), 0x0049CD60);
FunctionPointer(void, DynCol_LoadObject, (ObjectMaster *a1), 0x0049E170);
FunctionPointer(void, sub_446AF0, (ObjectMaster *a1, int a2), 0x446AF0);
DataPointer(float, SkyDeckHeight, 0x03C80610);
//DataPointer(const char *, CurrentSong, 0x00912698);
//DataArray(EntityData1 *, EntityData1Ptrs, 0x3B42E10, 8);
FunctionPointer(void, sub_4145D0, (unsigned __int8 a1, unsigned __int8 a2), 0x04145D0);
FunctionPointer(void, PlaySoundEffectMaybe, (int ID, int a2, int a3, int a4, int duration, EntityData1 *source), 0x0424880);
FunctionPointer(void, sub_441DF0, (unsigned __int8 a1), 0x0441DF0);

bool HasBarrier = false;
bool Submerged = false;
bool PlayBubbly = false;
int TailsflyingFrame = 0;
bool knuckleshurt = false;
bool amyhurt = false;

bool playedBigFailureStuff = false;
int BigFailure2ndLineTimer = 0;

int FinalEggVoicePlayed = 80;
int FinalEggVoiceTimes = 0;
bool FinalEggVoiceDone = false;

bool FinalEgg1stPathTaken = false;
bool FinalEgg2ndPathTaken = false;
bool FinalEgg3rdPathTaken = false;
bool FinalEgg4thPathTaken = false;
bool FinalEgg5thPathTaken = false;
bool Taunt1Played = false;
bool Taunt2Played = false;
bool Taunt3Played = false;

Angle GravityX;
bool SkyDeckVoicePlayed = false;

bool SkyDeckAct2VoicePlayed = false;
bool SkyDeckAct2VoicePlayedOther = false;
bool startVoicing = false;

int SkyDeckAct1Played = 280;
int SkyDeckAct1Times = 0;
bool SkyDeckAct1VoiceDone = false;

int E101VoiceTimer = 0;
bool E101BetaVoicePlayed = false;

int PerfectChaosVoicePlayed = 0;
bool PChaos1stVoice = false;
bool PChaos2ndVoice = false;
bool PChaos3rdVoice = false;
bool PChaos4thVoice = false;
bool startcount = false;
bool playedKnucklesvoice = false;
bool playedAmyvoice = false;
bool playedTailsvoice = false;
int TailsVoiceDelay = 18;
bool playedBigvoice = false;

DataPointer(int, DroppedFrames, 0x03B1117C);

//Additional SADX Functions
FunctionPointer(NJS_OBJECT *, DynamicCollision, (NJS_OBJECT *a1, ObjectMaster *a2, ColFlags surfaceFlags), 0x49D6C0);
FunctionPointer(int, rand1, (), 0x6443BF);
DataPointer(int, FramerateSetting, 0x0389D7DC);



//Null Code (Used for debugging purposes)
void __cdecl NullFunction(ObjectMaster *a1)
{
	return;
}

//Standard Display
void __cdecl Basic_Display(ObjectMaster *a2)
{
	EntityData1 *v1; // esi@1
	Angle v2; // eax@2
	Angle v3; // eax@4
	Angle v4; // eax@6

	v1 = a2->Data1;
	if (!MissedFrames)
	{
		SetTextureToLevelObj();
		njPushMatrix(0);
		njTranslateV(0, &v1->Position);
		v2 = v1->Rotation.z;
		if (v2)
		{
			njRotateZ(0, (unsigned __int16)v2);
		}
		v3 = v1->Rotation.x;
		if (v3)
		{
			njRotateX(0, (unsigned __int16)v3);
		}
		v4 = v1->Rotation.y;
		if (v4)
		{
			njRotateY(0, (unsigned __int16)v4);
		}
		ProcessModelNode_AB_Wrapper(v1->Object, 1.0);
		njPopMatrix(1u);
	}
}

//Standard Main
void __cdecl Basic_Main(ObjectMaster *a1)
{
	EntityData1 *v1; // edi@1

	v1 = a1->Data1;
	if (!ClipSetObject_Min(a1))
	{
		if (!ObjectSelectedDebug(a1))
		{
			AddToCollisionList(v1);
		}
		Basic_Display(a1);
	}
}

//Standard Delete Dynamic
void deleteSub_Global(ObjectMaster *a1) {
	if (a1->Data1->Object)
	{
		DynamicCOL_Remove(a1, a1->Data1->Object);
		ObjectArray_Remove(a1->Data1->Object);
	}
	DeleteObject_(a1);
}

void AddToCollision(ObjectMaster *a1, uint8_t col) {
	/*  0 is static
	1 is moving (refresh the colision every frame)
	2 is static, scalable
	3 is moving, scalable   */

	EntityData1 * original = a1->Data1;
	NJS_OBJECT *colobject;

	colobject = ObjectArray_GetFreeObject(); //The collision is a separate object, we add it to the list of object

	//if scalable
	if (col == 2 || col == 3) {
		colobject->evalflags = NJD_EVAL_BREAK | NJD_EVAL_SKIP | NJD_EVAL_HIDE;
		colobject->scl[0] = original->Scale.x;
		colobject->scl[1] = original->Scale.y;
		colobject->scl[2] = original->Scale.z;
	}
	else if (col == 4) {
		colobject->evalflags = NJD_EVAL_BREAK | NJD_EVAL_SKIP | NJD_EVAL_HIDE;
		colobject->scl[0] = 1.0f + original->Scale.x;
		colobject->scl[1] = 1.0f + original->Scale.x;
		colobject->scl[2] = 1.0f + original->Scale.x;
	}
	else if (col == 5) {
		colobject->evalflags = NJD_EVAL_BREAK | NJD_EVAL_SKIP | NJD_EVAL_HIDE;
		colobject->scl[0] = 1.0f + original->Scale.z;
		colobject->scl[1] = 1.0f + original->Scale.z;
		colobject->scl[2] = 1.0f + original->Scale.z;
	}
	else {
		colobject->evalflags = NJD_EVAL_UNIT_SCL | NJD_EVAL_BREAK | NJD_EVAL_SKIP | NJD_EVAL_HIDE; //ignore scale
		colobject->scl[0] = 1.0;
		colobject->scl[1] = 1.0;
		colobject->scl[2] = 1.0;
	}

	//add the rest
	if (col == 4 || col == 1 || col == 5)
	{
		colobject->ang[0] = 0;
		colobject->ang[1] = original->Rotation.y;
		colobject->ang[2] = 0;
	}
	else {
		colobject->ang[0] = original->Rotation.x;
		colobject->ang[1] = original->Rotation.y;
		colobject->ang[2] = original->Rotation.z;
	}
	colobject->pos[0] = original->Position.x;
	colobject->pos[1] = original->Position.y;
	colobject->pos[2] = original->Position.z;

	colobject->basicdxmodel = a1->Data1->Object->basicdxmodel; //object it will use as a collision
	a1->Data1->Object = colobject; //pointer to the collision object into our original object

	if (col == 0 || col == 2) DynamicCOL_Add((ColFlags)1, a1, colobject); //Solid
	else if (col == 1 || col == 3 || col == 4 || col == 5) DynamicCOL_Add((ColFlags)0x8000000, a1, colobject); //Dynamic, solid
}

//Basic drawing call
void DrawObjModel(ObjectMaster *a1, NJS_MODEL_SADX *m, bool scalable) {
	if (!MissedFrames) {
		njSetTexture((NJS_TEXLIST*)&BEACH01_TEXLIST); //Current heroes level texlist is always onto Emerald Coast
		njPushMatrix(0);
		njTranslateV(0, &a1->Data1->Position);
		njRotateXYZ(nullptr, a1->Data1->Rotation.x, a1->Data1->Rotation.y, a1->Data1->Rotation.z);
		if (scalable) njScale(nullptr, a1->Data1->Scale.x, a1->Data1->Scale.y, a1->Data1->Scale.z);
		else njScale(nullptr, 1, 1, 1);
		DrawQueueDepthBias = -6000.0f;
		DrawModel(m);
		DrawQueueDepthBias = 0;
		njPopMatrix(1u);
	}
}


void FinalEggAct1Announcer()
{
	if (FinalEggVoiceDone != true && (GameState == 3 || GameState == 4 || GameState == 7 || GameState == 21))
	{
		if (Camera_Data1 != nullptr && Camera_Data1->Position.z > -900)
		{
			FinalEggVoicePlayed = 280;
			FinalEggVoiceTimes = 0;
		}
		else
		{
			FinalEggVoiceTimes = 3;
		}
	}

	if (!MissedFrames)
	{
		if (CurrentLevel != 10)
		{
			FinalEggVoicePlayed = 280;
			FinalEggVoiceTimes = 0;
			FinalEggVoiceDone = false;
		}

		if (CurrentLevel == 10 && CurrentAct == 0 && CurrentCharacter != 5 && FinalEggVoiceTimes < 3)
		{
			if (FinalEggVoicePlayed >= 332)
			{
				if (FramerateSetting >= 2)
				{
					PlayVoice(1707);
				}
				else
				{
					PlayVoice(1706);
				}
				FinalEggVoiceTimes++;
			}

			if (!IsGamePaused())
			{
				if (FramerateSetting >= 2)
				{
					FinalEggVoicePlayed++;
				}
				FinalEggVoicePlayed++;
			}

			if (FinalEggVoicePlayed > 332)
			{
				FinalEggVoicePlayed = 0;
			}

			if (FinalEggVoiceTimes >= 3)
			{
				FinalEggVoiceDone = true;
			}
		}
	}
}

void PerfectChaosCheer()
{
	if (GameState == 3 || GameState == 4 || GameState == 7 || GameState == 21)
	{
		PerfectChaosVoicePlayed = 0;
		PChaos1stVoice = false;
		PChaos2ndVoice = false;
		PChaos3rdVoice = false;
		PChaos4thVoice = false;
		startcount = false;
	}

	if (!MissedFrames)
	{
		if (CurrentLevel != 19)
		{
			PerfectChaosVoicePlayed = 0;
			PChaos1stVoice = false;
			PChaos2ndVoice = false;
			PChaos3rdVoice = false;
			PChaos4thVoice = false;
			startcount = false;
		}

		if (GetRings() == 50)
		{
			startcount = true;
		}

		if (CurrentLevel == 19 && startcount == true)
		{
			if (PChaos1stVoice != true && PerfectChaosVoicePlayed == 559)
			{
				PlayVoice(1713); //Up to you
				PChaos1stVoice = true;
			}

			if (PChaos2ndVoice != true && PerfectChaosVoicePlayed == 719)
			{
				PlayVoice(1714); //Be Brave
				PChaos2ndVoice = true;
			}

			if (PChaos3rdVoice != true && PerfectChaosVoicePlayed == 919)
			{
				PlayVoice(388); //Show you
				PChaos2ndVoice = true;
			}

			if (PChaos4thVoice != true && PerfectChaosVoicePlayed == 1119)
			{
				PlayVoice(1716); //Stay as Super Sonic
				PChaos2ndVoice = true;
			}

			if (!IsGamePaused())
			{
				if (FramerateSetting >= 2)
				{
					PerfectChaosVoicePlayed += 2;
				}
				else
				{
					PerfectChaosVoicePlayed += 1;
				}
			}
		}
	}
}

void SkyDeck3Announcement()
{
	if ((GameState == 3 || GameState == 4 || GameState == 7 || GameState == 21))
	{
		if (EntityData1Ptrs[0] && EntityData1Ptrs[0]->Position.x < 0)
		{
			SkyDeckVoicePlayed = false;
		}
	}

	if (!MissedFrames)
	{
		if (CurrentLevel != 6)
		{
			SkyDeckVoicePlayed = false;
		}

		if (CurrentLevel == 6 && CurrentAct == 2 && CurrentCharacter != 3)
		{
			GravityX = Gravity.x;
			if (EntityData1Ptrs[0] && EntityData1Ptrs[0]->Position.x > 0 && !IsGamePaused() && SkyDeckVoicePlayed == false)
			{
				if (Gravity.x != GravityX)
				{
					PlayVoice(1726);
					SkyDeckVoicePlayed = true;
				}
			}
			else if (EntityData1Ptrs[0] && EntityData1Ptrs[0]->Position.x < -240 && !IsGamePaused())
			{
				if (Gravity.x != GravityX)
				{
					SkyDeckVoicePlayed = false;
				}
			}
		}
	}
}

void SkyDeck2OtherAnnouncements()
{
	if ((GameState == 3 || GameState == 4 || GameState == 7 || GameState == 21))
	{
		if (EntityData1Ptrs[0] && EntityData1Ptrs[0]->Position.z > -2700)
		{
			SkyDeckAct2VoicePlayed = false;
			SkyDeckAct2VoicePlayedOther = false;
			startVoicing = false;
		}
	}

	if (!MissedFrames)
	{
		if (CurrentLevel != 6)
		{
			SkyDeckAct2VoicePlayed = false;
			SkyDeckAct2VoicePlayedOther = false;
			startVoicing = false;
		}

		if (CurrentLevel == 6 && CurrentAct == 1)
		{
			//Rings = SkyDeckHeight; //Use this to check the altitude in real-time.
			if (EntityData1Ptrs[0] && EntityData1Ptrs[0]->Position.z < -2700 && !IsGamePaused())
			{
				if (SkyDeckHeight == 700.0f || SkyDeckHeight == 0.0f)
				{
					startVoicing = true;
				}

				if (startVoicing == true)
				{
					if (((SkyDeckHeight < 2.0f && SkyDeckHeight > 0.0f) || (SkyDeckHeight > 698.0f && SkyDeckHeight < 700.0f)) && SkyDeckAct2VoicePlayed == false && SkyDeckAct2VoicePlayedOther == true)
					{
						PlayVoice(1728); //Alert Cancelled
						SkyDeckAct2VoicePlayed = true;
						SkyDeckAct2VoicePlayedOther = false;
					}
					else if (((SkyDeckHeight < 697.0f && SkyDeckHeight > 690.0f) || (SkyDeckHeight < 10.0f && SkyDeckHeight > 3.0f)) && SkyDeckAct2VoicePlayedOther == false)
					{
						PlayVoice(1727); //Winds approaching
						SkyDeckAct2VoicePlayedOther = true;
						SkyDeckAct2VoicePlayed = false;
					}
				}
			}
		}
	}
}

void SkyDeckAct1Announcer()
{
	if (SkyDeckAct1VoiceDone != true && (GameState == 3 || GameState == 4 || GameState == 7 || GameState == 21))
	{
		if (Camera_Data1 != nullptr && Camera_Data1->Position.z < 260)
		{
			SkyDeckAct1Played = 80;
			SkyDeckAct1Times = 0;
		}
		else
		{
			SkyDeckAct1Times = 3;
		}
	}

	if (!MissedFrames)
	{
		if (CurrentLevel != 6)
		{
			SkyDeckAct1Played = 80;
			SkyDeckAct1Times = 0;
			SkyDeckAct1VoiceDone = false;
		}

		if (CurrentLevel == 6 && CurrentAct == 0 && CurrentCharacter != 2 && SkyDeckAct1Times < 3)
		{
			if (SkyDeckAct1Played >= 332)
			{
				if (FramerateSetting >= 2)
				{
					PlayVoice(1704);
				}
				else
				{
					PlayVoice(1705);
				}
				SkyDeckAct1Times++;
			}

			if (!IsGamePaused())
			{
				if (FramerateSetting >= 2)
				{
					SkyDeckAct1Played++;
				}
				SkyDeckAct1Played++;
			}

			if (SkyDeckAct1Played > 332)
			{
				SkyDeckAct1Played = 0;
			}

			if (SkyDeckAct1Times >= 3)
			{
				SkyDeckAct1VoiceDone = true;
			}
		}
	}
}

void BetaRematchVoice()
{
	if (GameState == 3 || GameState == 4 || GameState == 7 || GameState == 21)
	{
		E101VoiceTimer = 0;
		E101BetaVoicePlayed = false;
	}

	if (!MissedFrames)
	{
		if (CurrentLevel != 25)
		{
			E101VoiceTimer = 0;
			E101BetaVoicePlayed = false;
		}

		if (CurrentLevel == 25 && CurrentCharacter == 6 && E101BetaVoicePlayed == false)
		{
			if (E101BetaVoicePlayed != true && E101VoiceTimer == 459)
			{
				PlayVoice(1681);
				E101BetaVoicePlayed = true;
			}

			if (!IsGamePaused())
			{
				if (FramerateSetting >= 2)
				{
					E101VoiceTimer += 2;
				}
				else
				{
					E101VoiceTimer += 1;
				}
			}
		}

	}
}

void KnucklesNoRingsVoice()
{
	if (EntityData1Ptrs[0])
	{
		if (CurrentCharacter == 3 && EntityData1Ptrs[0]->Action == 51 && playedKnucklesvoice == false)
		{
			PlayVoice(1796);
			playedKnucklesvoice = true;
		}

		if (playedKnucklesvoice == true && (GameState == 3 || GameState == 4 || GameState == 7 || GameState == 21))
		{
			playedKnucklesvoice = false;
		}
	}
}

void AmyNoRingsVoice()
{
	if (EntityData1Ptrs[0])
	{
		if (CurrentCharacter == 5 && EntityData1Ptrs[0]->Action == 47 && playedAmyvoice == false)
		{
			PlayVoice(1740);
			playedAmyvoice = true;
		}

		if (playedAmyvoice == true && (GameState == 3 || GameState == 4 || GameState == 7 || GameState == 21))
		{
			playedAmyvoice = false;
		}
	}
}

void TailsNoRingsVoice()
{
	if (EntityData1Ptrs[0])
	{
		if (CurrentCharacter == 2 && EntityData1Ptrs[0]->Action == 60 && playedTailsvoice == false)
		{
			if (TailsVoiceDelay == 0)
			{
				PlayVoice(1809);
				playedTailsvoice = true;
			}
			else
			{
				TailsVoiceDelay--;
			}
		}

		if (playedTailsvoice == true && (GameState == 3 || GameState == 4 || GameState == 7 || GameState == 21))
		{
			playedTailsvoice = false;
			TailsVoiceDelay = 18;
		}
	}
}

void BigNoRingsVoice()
{
	if (EntityData1Ptrs[0])
	{
		//37 = caught fish
		if (CurrentCharacter == 7 && EntityData1Ptrs[0]->Action == 58 && playedBigvoice == false)
		{
			PlayVoice(1755);
			playedBigvoice = true;
		}

		if (playedBigvoice == true && (GameState == 3 || GameState == 4 || GameState == 7 || GameState == 21))
		{
			playedBigvoice = false;
		}
	}
}

void __cdecl BoatStartSound(unsigned __int8 a1, unsigned __int8 a2)
{
	sub_4145D0(a1, a2);

	if (EntityData1Ptrs[0])
	{
		if (CurrentLevel == 29 && CurrentAct < 2) //Egg Carrier
		{

			if (EntityData1Ptrs[0]->Position.x > 0 && EntityData1Ptrs[0]->Position.z < -530)
			{
				PlayVoice(163);
			}
			else if (EntityData1Ptrs[0]->Position.x < 0 && EntityData1Ptrs[0]->Position.z < -530)
			{
				PlayVoice(1610);
			}
		}
		else if (CurrentLevel == 33 && CurrentAct == 0 && EntityData1Ptrs[0]->Position.y < -370) //Mystic Ruins
		{
			PlayVoice(1610); //1609 is stop sound
		}
		else if (CurrentLevel == 26 && CurrentAct == 3 && EntityData1Ptrs[0]->Position.z > 1870) //Station Square
		{
			PlayVoice(163); //162 is stop sound

		}
	}
}

void BarrierLossSound()
{
	if (CurrentLevel != 0 && CurrentLevel != 19 && CurrentLevel != 26 && CurrentLevel != 29 && CurrentLevel != 32 && CurrentLevel != 33 && CurrentLevel != 34 && CurrentLevel != 37 && CurrentLevel != 21 && CurrentLevel != 22 && CurrentLevel != 24)
	{
		if (GetCharObj2(0) && !IsGamePaused())
		{
			if ((GetCharObj2(0)->Powerups & Powerups_Barrier) || (GetCharObj2(0)->Powerups & Powerups_MagneticBarrier) && HasBarrier == false)
			{
				HasBarrier = true;
			}
			else if ((GameState == 3 || GameState == 4 || GameState == 7 || GameState == 21))
			{
				HasBarrier = false;
			}
			else if (!GetCharObj2(0)->Powerups && HasBarrier == true)
			{
				if (!(GameState == 3 || GameState == 4 || GameState == 7 || GameState == 21))
				{
					PlaySound(23, 0, 0, 0); //Playing it twice to ensure it doesn't get overwritten.
					PlaySound(23, 0, 0, 0);
					HasBarrier = false;
				}
			}
		}
	}
	else
	{
		if (HasBarrier == true)
		{
			HasBarrier = false;
		}
	}
}

void WaterEffects()
{
	auto PlayEntity = EntityData1Ptrs[0];

	if (CurrentLevel != 0 && CurrentLevel != 19)
	{
		if (GetCharObj2(0) && !IsGamePaused())
		{
			if (GetCharObj2(0)->UnderwaterTime != 0 && Submerged == false)
			{
				Submerged = true;
			}
			else if (Submerged == true && PlayBubbly == false)
			{
				QueueSound_DualEntity(759, (EntityData1 *)0x41, 1, 0, 680);
				//PlaySound2(759, 0, 0, 0);
				PlayBubbly = true;
			}
			else if (GetCharObj2(0)->UnderwaterTime == 0 && (Submerged == true || PlayBubbly == true))
			{
				if (PlayEntity->Status & Status_Ground)
				{
					PlaySound(757, 0, 0, 0);
				}
				else
				{
					PlaySound(758, 0, 0, 0);
				}
				Submerged = false;
				PlayBubbly = false;
			}
		}
	}
	else
	{
		if (Submerged == true || PlayBubbly == true)
		{
			Submerged = false;
			PlayBubbly = false;
		}
	}
}

void __cdecl InvincibilityPowerupMORE(EntityData1 *entity)
{
	GiveInvincibility(0);

	switch (GetCurrentCharacterID())
	{
	case 0: //Sonic
		if (!MetalSonicFlag)
		{
			PlayVoice(1849); //"Yes!"
		}
		break;
	case 1: //Eggman
		break;
	case 2: //Tails
		PlayVoice(1811); //"Alright!"
		break;
	case 3: //Knuckles
		break;
	case 4: //Tikal
		break;
	case 5: //Amy
		PlaySound(1309, 0, 0, 0); //"Cool!" Only available in her sound bank. Not a voice clip.
		break;
	case 6: //Gamma
		PlaySound(1306, 0, 0, 0);
		break;
	case 7: //Big
		PlayVoice(1760); //"Super!"
		break;
	default:
		break;
	}
	PlaySound(11, 0, 0, 0);
}

void __cdecl SpeedShoesPowerupMORE(EntityData1 *entity)
{
	sub_441DF0(entity->CollisionInfo->CollidingObject->Object->Data1->CharIndex); //This used to set the hibyte, like this: CollidingObject->Object->Data1.Entity->CharIndex.UByte[0]); Problems?
	switch (GetCurrentCharacterID())
	{
	case 0: //Sonic
		if (!MetalSonicFlag)
		{
			PlayVoice(1850); //"Here we go!"
		}
		break;
	case 1: //Eggman
		break;
	case 2: //Tails
		PlayVoice(1812); //"Cool!"
		break;
	case 3: //Knuckles
		break;
	case 4: //Tikal
		break;
	case 5:
		PlaySound(1323, 0, 0, 0); //"Here I come!" Only available in her sound bank. Not a voice clip.
		break;
	case 6: //Gamma
		PlaySound(1306, 0, 0, 0);
		break;
	case 7: //Big
		//PlayVoice(1760);
		break;
	default:
		break;
	}
	PlaySound(11, 0, 0, 0);
}

void __cdecl ExtraLifePowerupMORE(EntityData1 *entity)
{
	GiveLives(1);
	switch (GetCurrentCharacterID())
	{
	case 0: //Sonic
		if (!MetalSonicFlag)
		{
			PlayVoice(1849); //"Yes!"
		}
		break;
	case 1: //Eggman
		break;
	case 2: //Tails
		PlayVoice(1811); //"Alright!"
		break;
	case 3: //Knuckles
		break;
	case 4: //Tikal
		break;
	case 5:
		PlaySound(1309, 0, 0, 0); //"Cool!" Only available in her sound bank. Not a voice clip.
		break;
	case 6: //Gamma
		PlaySound(1306, 0, 0, 0);
		break;
	case 7: //Big
		PlayVoice(1760); //"Super!"
		break;
	default:
		break;
	}
	PlaySound(11, 0, 0, 0);
}

void __cdecl LightSpeedChargeSound()
{
	if (CurrentCharacter == 0)
	{
		auto entity = EntityData1Ptrs[0];

		if (entity)
		{
			if (entity->Status & Status_LightDash)
			{
				QueueSound_DualEntity(766, entity, 1, 0, 150);
			}
		}
	}
}

void __cdecl TailsFlightSound()
{
	if (CurrentCharacter == 2)
	{
		auto entity = EntityData1Ptrs[0];

		if (GetCharObj2(0) && !IsGamePaused())
		{
			if (GetCharObj2(0)->TailsFlightTime != 0 && TailsflyingFrame == 0 && !(entity->Status & Status_Ground) && entity->Action == 15 && entity->Action != 19) //15 = flying, 19 = exhausted
			{
				QueueSound_DualEntity(770, entity, 1, 0, 30);
				TailsflyingFrame++;
			}
			else if (GetCharObj2(0)->TailsFlightTime != 0 && TailsflyingFrame < 30 && !(entity->Status & Status_Ground))
			{
				TailsflyingFrame++;
			}
			else
			{
				TailsflyingFrame = 0;
				/*if (GetCharObj2(0)->TailsFlightTime < 1)
				{
					GetCharObj2(0)->TailsFlightTime = 0;
				}*/
			}
		}
	}
}

void __cdecl KnucklesHurtSound()
{
	if (CurrentCharacter == 3)
	{
		auto entity = EntityData1Ptrs[0];

		if (entity)
		{
			if ((entity->Status & Status_Hurt) && knuckleshurt == false)
			{
				PlaySound(1309, 0, 0, 0); //"Hey!" Only available in his sound bank. Not a voice clip.
				knuckleshurt = true;
			}
			else if (!(entity->Status & Status_Hurt) && knuckleshurt == true && EntityData1Ptrs[0]->InvulnerableTime == 0)
			{
				knuckleshurt = false;
			}
		}
		else
		{
			knuckleshurt = false;
		}
	}
}

void __cdecl AmyHurtSound()
{
	if (CurrentCharacter == 5)
	{
		auto entity = EntityData1Ptrs[0];

		if (entity)
		{
			if ((entity->Status & Status_Hurt) && amyhurt == false)
			{
				PlaySound(1295, 0, 0, 0); //"Ow!" Only available in her sound bank. Not a voice clip.
				amyhurt = true;
			}

			else if (!(entity->Status & Status_Hurt) && amyhurt == true && EntityData1Ptrs[0]->InvulnerableTime == 0)
			{
				amyhurt = false;
			}
		}

		else
		{
			amyhurt = false;
		}
	}
}

void __cdecl BigFailedFishing()
{
	if (CurrentCharacter == 7)
	{
		auto entity = EntityData1Ptrs[0];

		if (entity)
		{
			if (entity->Action == 39 && playedBigFailureStuff == false) //39 = "Big's line snapped while fishing" Action for him.
			{
				PlayVoice(1752); //GASP!
				PlayMusic(MusicIDs_fishmiss);
				playedBigFailureStuff = true;
				BigFailure2ndLineTimer++;
				if (FramerateSetting <= 2)
				{
					BigFailure2ndLineTimer++;
				}
			}
			else if (BigFailure2ndLineTimer != 0 && BigFailure2ndLineTimer < 120)
			{
				BigFailure2ndLineTimer++;
				if (FramerateSetting <= 2)
				{
					BigFailure2ndLineTimer++;
				}
			}
			else if (BigFailure2ndLineTimer >= 120)
			{
				PlayVoice(1753); //Oh no!
				BigFailure2ndLineTimer = 0;
			}
			//What's 1756 for?
			else if ((GameState == 3 || GameState == 4 || GameState == 7 || GameState == 21))
			{
				playedBigFailureStuff = false;
				BigFailure2ndLineTimer = 0;
			}
		}
	}
	else if (playedBigFailureStuff == true && (GameState == 3 || GameState == 4 || GameState == 7 || GameState == 21))
	{
		playedBigFailureStuff = false;
		BigFailure2ndLineTimer = 0;
	}
}

void __cdecl FinalEggTaunt()
{
	auto entity = EntityData1Ptrs[0];

	if (entity)
	{
		NJS_VECTOR AmyPos = entity->Position;

		if (CurrentCharacter == 5 && CurrentLevel == 10 && CurrentAct == 0)
		{
			if (AmyPos.z < -1800 && AmyPos.y < 400 && AmyPos.y > 200)
			{
				if (AmyPos.x > 2160 && AmyPos.z > -2000 && FinalEgg1stPathTaken == false)
				{
					if (!Taunt1Played)
					{
						PlayVoice(1984);
						Taunt1Played = true;
					}
					else if (!Taunt2Played)
					{
						PlayVoice(1985);
						Taunt2Played = true;
					}
					else if (!Taunt3Played)
					{
						PlayVoice(1986);
						Taunt3Played = true;
					}
					FinalEgg1stPathTaken = true;
				}
				else if (AmyPos.x < 1520 && AmyPos.z > -2000 && FinalEgg2ndPathTaken == false)
				{
					if (!Taunt1Played)
					{
						PlayVoice(1984);
						Taunt1Played = true;
					}
					else if (!Taunt2Played)
					{
						PlayVoice(1985);
						Taunt2Played = true;
					}
					else if (!Taunt3Played)
					{
						PlayVoice(1986);
						Taunt3Played = true;
					}
					FinalEgg2ndPathTaken = true;
				}
				else if (AmyPos.x > 1779 && AmyPos.x < 1841 && AmyPos.z < -2220 && FinalEgg3rdPathTaken == false)
				{
					if (!Taunt1Played)
					{
						PlayVoice(1984);
						Taunt1Played = true;
					}
					else if (!Taunt2Played)
					{
						PlayVoice(1985);
						Taunt2Played = true;
					}
					else if (!Taunt3Played)
					{
						PlayVoice(1986);
						Taunt3Played = true;
					}
					FinalEgg3rdPathTaken = true;
				}
				else if (AmyPos.z < -2150 && AmyPos.x < 1779 && AmyPos.x > 1590 && FinalEgg4thPathTaken == false)
				{
					if (!Taunt1Played)
					{
						PlayVoice(1984);
						Taunt1Played = true;
					}
					else if (!Taunt2Played)
					{
						PlayVoice(1985);
						Taunt2Played = true;
					}
					else if (!Taunt3Played)
					{
						PlayVoice(1986);
						Taunt3Played = true;
					}
					FinalEgg4thPathTaken = true;
				}
				else if (AmyPos.z < -2150 && AmyPos.x > 1841 && AmyPos.x < 2120 && FinalEgg5thPathTaken == false)
				{
					if (!Taunt1Played)
					{
						PlayVoice(1984);
						Taunt1Played = true;
					}
					else if (!Taunt2Played)
					{
						PlayVoice(1985);
						Taunt2Played = true;
					}
					else if (!Taunt3Played)
					{
						PlayVoice(1986);
						Taunt3Played = true;
					}
					FinalEgg5thPathTaken = true;
				}
			}

			if ((GameState == 3 || GameState == 4 || GameState == 7 || GameState == 21))
			{
				FinalEgg1stPathTaken = false; //Right-most path.
				FinalEgg2ndPathTaken = false; //Left-most path.
				FinalEgg3rdPathTaken = false; //Middle path.
				FinalEgg4thPathTaken = false; //Left-middle path.
				FinalEgg5thPathTaken = false; //Right-middle path.
				Taunt1Played = false;	//1984
				Taunt2Played = false;	//1985
				Taunt3Played = false;	//1986
			}
		}
	}
}


PointerInfo pointers[] = {

0
};



void Init(const char *path, const HelperFunctions &helperFunctions)
{
	const IniFile *config = new IniFile(std::string(path) + "\\config.ini");
	FinalEggAnnouncer = config->getBool("Options", "FinalEggAnnouncer", true);
	PerfectChaos = config->getBool("Options", "PerfectChaos", true);
	BetaRematch = config->getBool("Options", "BetaRematch", true);
	SkyDeck2Announcer = config->getBool("Options", "SkyDeck2Announcer", true);
	SkyDeck1Announcer= config->getBool("Options", "SkyDeck1Announcer", true);
	KnucklesNoRing = config->getBool("Options", "KnucklesNoRing", true);
	AmyNoRing = config->getBool("Options", "AmyNoRing", true);
	TailsNoRing = config->getBool("Options", "TailsNoRing", true);
	BigNoRing = config->getBool("Options", "BigNoRing", true);
	BarrierLoss = config->getBool("Options", "BarrierLoss", true);
	WaterStuff = config->getBool("Options", "WaterStuff", true);
	LightSpeed = config->getBool("Options", "LightSpeed", true);
	TailsFlight = config->getBool("Options", "TailsFlight", true);
	KnucklesHurt = config->getBool("Options", "KnucklesHurt", true);
	AmyHurt = config->getBool("Options", "AmyHurt", true);
	FishingFailed = config->getBool("Options", "FishingFailed", true);
	FinalEggColorDoor = config->getBool("Options", "FinalEggColorDoor", true);
	ItemVoices = config->getBool("Options", "ItemVoices", true);
	BoatNoise = config->getBool("Options", "BoatNoise", true);

	delete config;
}

extern "C"
{
	__declspec(dllexport) ModInfo SADXModInfo = { ModLoaderVer, &Init, NULL, 0, NULL, 0, NULL, 0, arrayptrandlength(pointers) };

	__declspec(dllexport) void cdecl Init()
	{
		WriteData((short*)0x915C68, (short)371); //Idle line for Chaos 4 for Sonic fix
		if (BoatNoise)
		{
			WriteJump((void *)0x412D80, BoatStartSound);
		}

		if (ItemVoices)
		{
			WriteJump((void *)0x4D6D80, InvincibilityPowerupMORE);
			WriteJump((void *)0x4D6BF0, SpeedShoesPowerupMORE);
			WriteJump((void *)0x4D6D40, ExtraLifePowerupMORE);
		}

		//HYPER-IMPORTANT NOTE: sub_431930 = plays voice in event. Like, cutscenes. Use this to maybe find a lead on how to add in the unused sky chase clips.
	}

	__declspec(dllexport) void __cdecl OnFrame()
	{
		if (FinalEggAnnouncer)
		{
			FinalEggAct1Announcer();
		}
		if (PerfectChaos)
		{
			PerfectChaosCheer();
		}
		if (SkyDeck3Announcer)
		{
			SkyDeck3Announcement();
		}
		if (BetaRematch)
		{
			BetaRematchVoice();
		}
		if (SkyDeck2Announcer)
		{
			SkyDeck2OtherAnnouncements();
		}
		if (SkyDeck1Announcer)
		{
			SkyDeckAct1Announcer();
		}
		if (KnucklesNoRing)
		{
			KnucklesNoRingsVoice();
		}
		if (AmyNoRing)
		{
			AmyNoRingsVoice();
		}
		if (TailsNoRing)
		{
			TailsNoRingsVoice();
		}
		if (BigNoRing)
		{
			BigNoRingsVoice();
		}
		if (BarrierLoss)
		{
			BarrierLossSound();
		}
		if (WaterStuff)
		{
			WaterEffects();
		}
		if (LightSpeed)
		{
			LightSpeedChargeSound();
		}
		if (TailsFlight)
		{
			TailsFlightSound();
		}
		if (KnucklesHurt)
		{
			KnucklesHurtSound();
		}
		if (AmyHurt)
		{
			AmyHurtSound();
		}
		if (FishingFailed)
		{
			BigFailedFishing();
		}
		if (FinalEggColorDoor)
		{
			FinalEggTaunt();
		}
	}
}