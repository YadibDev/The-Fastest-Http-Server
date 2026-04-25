#include "../PartRespond/mainprocess/clsMainProcess.hpp"

#ifndef monitor_cgi_0000
#define monitor_cgi_0000

#define CGI_TIMEOUT 4


class clsMonitorCGI
{
private:
    int pid;
    int pipe;
    time_t startTime;
    stEventProcess::eEventProcess stateProcess;
    stEventData::eEventData stateData;
public:
    clsMonitorCGI(){
        pid = -1;
        pipe = -1;
    }
    void initialzie(int pid, int pipe, time_t start)
    {
        this->pid = pid;
        this->pipe = pipe;
        startTime = start;
        stateProcess = stEventProcess::RUNINNG;
        stateData = stEventData::STILL_EXIST;
    }

    bool TimeoutCgi()
    {
        if (HelperFunctions::isTimeout(startTime, CGI_TIMEOUT))
        {
            if (stateProcess < stEventProcess::THE_END)
            {
                kill(pid, SIGKILL);
                HelperFunctions::checkProcessStatus(pid);
                stateProcess = stEventProcess::END_WITH_TIMOUT;
            }
            if (stateData == stEventData::STILL_EXIST)
                close(pipe);
            stateData = stEventData::END_PIPE;
            return true;
        }
        return false;
    }

    short getDataFromCgi(char *buffer, short bufferSize) // -1 end of pipe
    {
        if (stateProcess == stEventProcess::RUNINNG)
            stateProcess = HelperFunctions::checkProcessStatus(pid);

        if (stateProcess > stEventProcess::THE_END)
        {
            if (stateData == stEventData::STILL_EXIST)
                close(pipe);
            stateData = stEventData::END_PIPE;
            return -1;
        }
        short reads = read(pipe, buffer, bufferSize);
        if (reads == 0 || (stateProcess == stEventProcess::THE_END && reads < bufferSize))
        {
            close(pipe);
            stateData = stEventData::END_PIPE;
        }
        return reads;
    }

    stEventProcess::eEventProcess getStateProcess()
    {
        return this->stateProcess;
    }
    stEventData::eEventData getStateData()
    {
        return this->stateData;
    }
    void setStateProcess(stEventProcess::eEventProcess state)
    {
        this->stateProcess = state;
    }
    void setStateData(stEventData::eEventData state)
    {
        this->stateData = state;
    }

};

#endif