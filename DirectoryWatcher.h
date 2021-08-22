
#ifndef __DIR_WATCH__
#define __DIR_WATCH__

#include <map>
#include <list>
#include <thread>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <queue>
#include <dirent.h>
#include <sys/types.h>
#include <string.h>

enum Action
{
    FILE_CREATED = 0,
    FILE_DELETED,
    FILE_MODIFIED
};


struct FileData
{
    std::string strFileName;
    std::string strFileData;
    Action enAction;
};

class CDirWatcher
{
    bool m_bInitialized;
    std::map<std::string,struct stat> m_strLocalList;
    std::queue<FileData> *m_ExternQueue;
    bool m_bStop;
    std::thread m_pThread;
    std::string m_strDirPath;
    void WorkerThread();
    bool GetListofFIles(std::string strDirFQPN, std::list<std::string> &lstFiles);

public:
    CDirWatcher();
    ~CDirWatcher();

    bool Initialize(std::string strDirPath,std::queue<FileData> *queue);
    bool StartWatch();
    bool StopWatch();
};


#endif //PLUGINBASEDFILEFORMAT_JSON_PLUGIN_H
