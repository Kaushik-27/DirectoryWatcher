#include "DirectoryWatcher.h"
#include <sys/stat.h>

CDirWatcher::CDirWatcher()
{
    m_bInitialized = false;
    m_strDirPath = "";
}

CDirWatcher::~CDirWatcher()
{
}

bool CDirWatcher::Initialize(std::string strDirPath, std::queue<FileData> *queue)
{
    if (queue == NULL)
    {
        return false;
    }

    m_ExternQueue = queue;

    struct stat fileInfo;
    if (stat(strDirPath.c_str(), &fileInfo) != 0)
    {
        std::cerr << "Error: " << strDirPath << "-Directory is not present...\n";
        return false;
    }

    if ((fileInfo.st_mode & S_IFMT) == S_IFDIR)
    {
        std::cout << strDirPath << "- is a Directory...\n";
        m_bInitialized = true;
        m_strDirPath = strDirPath;
    }
    else
    {
        std::cout << strDirPath << "- is not a Directory...\n";
        return false;
    }
    return true;
}

bool CDirWatcher::StartWatch()
{
    m_pThread = std::thread(WorkerThread, this);
    return m_pThread.joinable();
}

bool CDirWatcher::StopWatch()
{
    m_bStop = true;
    m_pThread.join();
    return m_bStop;
}

void CDirWatcher::WorkerThread()
{
    std::list<std::string> lstFiles;
    m_strLocalList.clear();

    //Pull initial status
    if (GetListofFIles(m_strDirPath, lstFiles) == true)
    {
        for (std::list<std::string>::iterator it = lstFiles.begin(); it != lstFiles.end(); it++)
        {
            struct stat fileInfo;

            if (stat((*it).c_str(), &fileInfo) != 0)
            {
                std::cerr << "Error: " << (*it) << "- Read failure...\n";
                continue;
            }

            if ((fileInfo.st_mode & S_IFMT) == S_IFDIR)
            {
                std::cout << (*it) << "- is a Directory...Not Supported Currently\n";
                continue;
            }

            m_strLocalList.insert(std::make_pair((*it), fileInfo));
        }
    }

    //now poll and check the new status
    while (m_bStop == false)
    {
        lstFiles.clear();
        if (GetListofFIles(m_strDirPath, lstFiles) == true)
        {
            for (std::list<std::string>::iterator it = lstFiles.begin(); it != lstFiles.end(); it++)
            {
                std::map<std::string,struct stat>::iterator mit = m_strLocalList.find((*it));
                if(mit == m_strLocalList.end())
                {
                    //New file added. Add it to map.
                    
                }
                else
                {
                    //check if modified
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}

bool CDirWatcher::GetListofFIles(std::string strDirFQPN, std::list<std::string> &lstFiles)
{

    struct dirent *entry;
    DIR *dir = opendir(strDirFQPN.c_str());

    if (dir == NULL)
    {
        return false;
    }

    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        std::string strFile = strDirFQPN + "/" + entry->d_name;
        lstFiles.push_back(strFile);
    }
    closedir(dir);
    return true;
}