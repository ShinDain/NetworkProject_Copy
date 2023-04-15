#pragma warning(disable:4819)
#pragma once
#include <Windows.h>
#include <fmod.h>
#include <tchar.h>
#include <windowsx.h>

#include <chrono>
#include <iostream>
// 초 단위, 충돌 시 나는 소리 재생한 뒤 여유를 두고 다시 재생
#define COLLIDE_SOUND_COOL_TIME 1
enum class EffectSoundType { getItem, deadEnemySound, collision, click, falling, flash, hit, ENDID };

using namespace std::chrono;

// 0번 스테이지 시작
// 1번 게임 시작시
// 2번 게임 종료
// 3번 아이템 획득 소리
// 4번 적 죽는 소리
// 5번 충돌 했을 때

class Sound {
  FMOD_SYSTEM *systemSound;
  FMOD_SOUND *soundIntro, *soundSelect, *soundRun, *soundGameOver, *soundsEffect[(unsigned)EffectSoundType::ENDID];
  FMOD_CHANNEL *channelEffect, *channelBGM;
  float volBGM, volEffect;
  high_resolution_clock::time_point prevPlayPoint;

 public:
  Sound();
  ~Sound();

  void resetChannelBGM();

  void setVolumeBGM(float volume);
  void setVolumeEffect(float volume);

  void playEffect(EffectSoundType num);
  void checkplay();

  void playIntro();
  void playSelect();
  void playRun();
  void playGameOver();
};
