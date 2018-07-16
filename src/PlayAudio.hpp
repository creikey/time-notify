#ifndef HPP_PLAY_AUDIO
#define HPP_PLAY_AUDIO

#include <ao/ao.h>
#include <sndfile.h>
#include <signal.h>

#include <vector>
#include <memory>

class PlayAudio
{
  private:
    static const int bufferSize = 8192;
    bool cancelPlayback;
    std::string soundPath;
    bool readyToPlay;
    void cleanDevice(ao_device *device, SNDFILE *file);

  public:
    static std::vector<std::reference_wrapper<PlayAudio>> toShutdown;
    static void initAudio();
    static void onSignal(int inSignal);
    PlayAudio() : readyToPlay(false){};
    PlayAudio(std::string inSoundPath) : soundPath(inSoundPath), readyToPlay(true){};
    void onCancelPlayback();
    void setSoundPath(std::string inSoundPath);
    void playSound();
};

#endif // HPP_PLAY_AUDIO