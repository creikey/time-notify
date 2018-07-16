#include <ao/ao.h>
#include <sndfile.h>
#include <signal.h>
#include <string>
#include <memory>

#include "PlayAudio.hpp"

void PlayAudio::cleanDevice(ao_device *device, SNDFILE *file)
{
    ao_close(device);
    sf_close(file);
    ao_shutdown();
}

void PlayAudio::onCancelPlayback()
{
    cancelPlayback = true;
}

void PlayAudio::setSoundPath(std::string inSoundPath)
{
    soundPath = inSoundPath;
    readyToPlay = true;
}

void PlayAudio::playSound()
{
    if (!readyToPlay)
    {
        std::string toThrow = std::string("PlayAudio instance not ready to play: ");
        if (soundPath == "")
        {
            toThrow += "soundPath not given";
        }
        throw toThrow;
    }
    PlayAudio::toShutdown.push_back(std::ref(*this));
    ao_device *device;
    ao_sample_format format;
    SF_INFO sfinfo;

    int default_driver;

    short *buffer;

    SNDFILE *file = sf_open(soundPath.c_str(), SFM_READ, &sfinfo);

    ao_initialize();

    default_driver = ao_default_driver_id();

    switch (sfinfo.format & SF_FORMAT_SUBMASK)
    {
    case SF_FORMAT_PCM_16:
        format.bits = 16;
        break;
    case SF_FORMAT_PCM_24:
        format.bits = 24;
        break;
    case SF_FORMAT_PCM_32:
        format.bits = 32;
        break;
    case SF_FORMAT_PCM_S8:
        format.bits = 8;
        break;
    case SF_FORMAT_PCM_U8:
        format.bits = 8;
        break;
    default:
        format.bits = 16;
        break;
    }

    format.channels = sfinfo.channels;
    format.rate = sfinfo.samplerate;
    format.byte_format = AO_FMT_NATIVE;
    format.matrix = 0;

    device = ao_open_live(default_driver, &format, NULL);

    if (device == NULL)
    {
        throw std::string("Failed to open device");
    }

    buffer = (short *)calloc(PlayAudio::bufferSize, sizeof(short));

    while (1)
    {
        int read = sf_read_short(file, buffer, PlayAudio::bufferSize);

        if (ao_play(device, (char *)buffer, (uint_32)(read * sizeof(short))) == 0)
        {
            //printf("ao_play: failed.\n");
            cleanDevice(device, file);
            throw std::string("ao_play failed");
            break;
        }

        if (cancelPlayback)
        {
            cleanDevice(device, file);
            break;
        }
    }

    cleanDevice(device, file);
}

std::vector<std::reference_wrapper<PlayAudio>> PlayAudio::toShutdown = std::vector<std::reference_wrapper<PlayAudio>>();

void PlayAudio::initAudio()
{
    signal(SIGINT, PlayAudio::onSignal);
}

void PlayAudio::onSignal(int inSignal)
{
    if (inSignal != SIGINT)
    {
        return;
    }

    for (auto i = PlayAudio::toShutdown.begin(); i != PlayAudio::toShutdown.end(); i++)
    {
        (*i).get().cancelPlayback = true;
    }
}