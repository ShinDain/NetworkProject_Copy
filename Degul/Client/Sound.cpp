#include "Sound.h"

Sound::Sound() {
  channelEffect = NULL;
  channelBGM = NULL;
  // 시스템 오브젝트 생성과 초기화
  FMOD_System_Create(&systemSound);
  FMOD_System_Init(systemSound, 64, FMOD_INIT_NORMAL, NULL);

  // 반복 사운드 배경음

  FMOD_System_CreateSound(systemSound, "Resource\\Sound\\intro.wav", FMOD_DEFAULT | FMOD_LOOP_NORMAL, 0, &soundIntro);
  FMOD_System_CreateSound(systemSound, "Resource\\Sound\\run1.wav", FMOD_DEFAULT | FMOD_LOOP_NORMAL, 0, &soundSelect);
  FMOD_System_CreateSound(systemSound, "Resource\\Sound\\Bossrun.wav", FMOD_DEFAULT | FMOD_LOOP_NORMAL, 0, &soundRun);
  FMOD_System_CreateSound(systemSound, "Resource\\Sound\\Defeat.mp3", FMOD_DEFAULT | FMOD_LOOP_NORMAL, 0,
                          &soundGameOver);

  // 효과 사운드
  // 아이템 획득 소리
  FMOD_System_CreateSound(systemSound, "Resource\\Sound\\play_fire.wav", FMOD_DEFAULT, 0,
                          &soundsEffect[(unsigned)EffectSoundType::getItem]);
  // 적 죽는 소리
  FMOD_System_CreateSound(systemSound, "Resource\\Sound\\enemy_fire.wav", FMOD_DEFAULT, 0,
                          &soundsEffect[(unsigned)EffectSoundType::deadEnemySound]);
  // 충돌 했을 때
  FMOD_System_CreateSound(systemSound, "Resource\\Sound\\collision.wav", FMOD_DEFAULT, 0,
                          &soundsEffect[(unsigned)EffectSoundType::collision]);
  // UI 클릭음
  FMOD_System_CreateSound(systemSound, "Resource\\Sound\\click3.wav", FMOD_DEFAULT, 0,
                          &soundsEffect[(unsigned)EffectSoundType::click]);
  // 떨어질때
  FMOD_System_CreateSound(systemSound, "Resource\\Sound\\falling.wav", FMOD_DEFAULT, 0,
                          &soundsEffect[(unsigned)EffectSoundType::falling]);
  // 순간이동
  FMOD_System_CreateSound(systemSound, "Resource\\Sound\\flash.ogg", FMOD_DEFAULT, 0,
                          &soundsEffect[(unsigned)EffectSoundType::flash]);
  // 플레이어 히트
  FMOD_System_CreateSound(systemSound, "Resource\\Sound\\hit.wav", FMOD_DEFAULT, 0,
                          &soundsEffect[(unsigned)EffectSoundType::hit]);
  // 충돌 사운드 재생 간격
  prevPlayPoint = high_resolution_clock::now();
  //볼륨 초기화
  volBGM = 0.5;
  volEffect = 4.0;
}

Sound::~Sound() {
  FMOD_Sound_Release(soundIntro);
  FMOD_Sound_Release(soundSelect);
  FMOD_Sound_Release(soundRun);
  FMOD_Sound_Release(soundGameOver);

  for (int i = 0; i < 6; i++) FMOD_Sound_Release(soundsEffect[i]);

  FMOD_System_Close(systemSound);
  FMOD_System_Release(systemSound);
}

void Sound::resetChannelBGM() {
  FMOD_BOOL checkPlay = false;

  FMOD_Channel_IsPlaying(channelBGM, &checkPlay);
  if (checkPlay) {
    FMOD_Channel_Stop(channelBGM);
    return;
  }
}

void Sound::setVolumeBGM(float volume) {
  if (volume < 0.00f || volume > 1.00f) {
    return;
  }
  volBGM = volume;
  FMOD_BOOL checkPlay = false;

  FMOD_Channel_IsPlaying(channelBGM, &checkPlay);
  if (checkPlay) {
    FMOD_Channel_SetVolume(channelBGM, volBGM);
    return;
  }
}

void Sound::setVolumeEffect(float volume) {
  if (volume < 0 || volume > 8) {
    return;
  }
  volEffect = volume;
  FMOD_BOOL checkPlay = false;

  FMOD_Channel_IsPlaying(channelEffect, &checkPlay);
  if (checkPlay) {
    FMOD_Channel_SetVolume(channelEffect, volEffect);
    return;
  }
}

void Sound::playEffect(EffectSoundType num) {
  //충돌할 때 빠르게 함수가 호출되서 소리가 이상하게 들리는 것을 방지
  if (num == EffectSoundType::collision) {
    auto nowPlayPoint = high_resolution_clock::now();

    duration<float, std::milli> timeDura = nowPlayPoint - prevPlayPoint;
    if (timeDura.count() < 200.0) {
      return;
    } else {
      prevPlayPoint = nowPlayPoint;
    }
  }
  // FMOD_Channel_Stop(channelEffect);
  FMOD_System_PlaySound(systemSound, soundsEffect[(unsigned)num], NULL, false, &channelEffect);
  FMOD_Channel_SetVolume(channelEffect, volEffect);
}

void Sound::checkplay() { FMOD_System_Update(systemSound); }

void Sound::playIntro() {
  resetChannelBGM();
  FMOD_System_PlaySound(systemSound, soundIntro, NULL, false, &channelBGM);
  FMOD_Channel_SetVolume(channelBGM, volBGM);
}

void Sound::playSelect() {
  resetChannelBGM();
  FMOD_System_PlaySound(systemSound, soundSelect, NULL, false, &channelBGM);
  FMOD_Channel_SetVolume(channelBGM, volBGM);
}

void Sound::playRun() {
  resetChannelBGM();
  FMOD_System_PlaySound(systemSound, soundRun, NULL, false, &channelBGM);
  FMOD_Channel_SetVolume(channelBGM, volBGM);
}
void Sound::playGameOver() {
  resetChannelBGM();
  FMOD_System_PlaySound(systemSound, soundGameOver, NULL, false, &channelBGM);
  FMOD_Channel_SetVolume(channelBGM, volBGM);
}
