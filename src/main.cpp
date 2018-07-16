#include <iostream>
#include <exception>
#include <memory>
#include <string>
#include <unistd.h>

#include "PlayAudio.hpp"

class AmountTime
{
  public:
    int minutes;
    int seconds;
    bool done;
    void decrementTime(int amountToDecrement)
    {
        seconds -= amountToDecrement;
        if (seconds < 0)
        {
            minutes -= 1;
            seconds += 60;
        }
        updateDoneFlag();
    }
    std::string getTimeString()
    {
        std::string toReturn;
        if (minutes < 10)
        {
            toReturn += "0";
        }
        toReturn += std::to_string(minutes);
        toReturn += ":";
        if (seconds < 10)
        {
            toReturn += "0";
        }
        toReturn += std::to_string(seconds);
        return toReturn;
    }
    AmountTime(std::string toParse)
    {
        int itemsRead = sscanf(toParse.c_str(), "%d:%d", &minutes, &seconds);
        if (itemsRead < 2)
        {
            throw std::string("Expected 2 items to be read, read " + std::to_string(itemsRead));
        }
        updateDoneFlag();
    }
    AmountTime(int inMinutes, int inSeconds) : minutes(inMinutes), seconds(inSeconds) { updateDoneFlag(); };
    AmountTime(){};

  private:
    void updateDoneFlag()
    {
        if (minutes > 0 || seconds > 0)
        {
            done = false;
        }
        else
        {
            done = true;
        }
    }
};

#ifndef RINGPATH
#define RINGPATH "nopathsupplied"
#endif

int main(int argc, char **argv)
{
    std::string ringPath = RINGPATH;
    std::unique_ptr<PlayAudio> ringPlayer = std::make_unique<PlayAudio>(ringPath);
    if (argc <= 1)
    {
        std::cerr << "[first arg] - amount of minutes:seconds to wait" << std::endl;
        return 1;
    }
    std::unique_ptr<AmountTime> currentAmountTimeLeft;
    try
    {
        currentAmountTimeLeft = std::make_unique<AmountTime>(argv[1]);
    }
    catch (std::string e)
    {
        std::cerr << "Failed to parse " << argv[1] << ", " << e << std::endl;
        return 1;
    }
    while (currentAmountTimeLeft->done != true)
    {
        currentAmountTimeLeft->decrementTime(1);
        std::cout << currentAmountTimeLeft->getTimeString() << std::endl;
        sleep(1);
    }
    std::cout << "Done!" << std::endl;
    try
    {
        ringPlayer->playSound();
    }
    catch (std::string e)
    {
        std::cerr << "Failed to play ringing noise: " << e << std::endl;
        return 1;
    }
    return 0;
}