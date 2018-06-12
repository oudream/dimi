#include "rtdb_logfile.h"

#include <hdfs.h>

using namespace std;

static const string f_sMonsbChangedIndexFileName = "monsb_changed_index.txt";
static const string f_sYcaddChangedIndexFileName = "ycadd_changed_index.txt";
static const string f_sStrawChangedIndexFileName = "straw_changed_index.txt";

// iOldIndex, iNewIndex, 1, CxTime::currentMsepoch()
static const string f_sMeasureChangedIndexText = "%d, %d, 1, %lld\n";

// oChangedData->changedTime,
// oChangedData->value,
// oChangedData->quality,
// oChangedData->changedSourceId,
// oChangedData->changedReasonId
static const string f_sMonsbChangedIndexText = ",[%lld,%d,%d,%d,%d]";
static const string f_sYcaddChangedIndexText = ",[%lld,%f,%d,%d,%d]";
static const string f_sStrawChangedIndexText = ",[%lld,%s,%d,%d,%d]";

static string f_sLogPath = string();

static FILE* f_oMonsbChangedIndexFile = NULL;
static FILE* f_oYcaddChangedIndexFile = NULL;
static FILE* f_oStrawChangedIndexFile = NULL;


size_t
RtdbLogFile::loadMeasureChangedIndex(const std::string& sFileName)
{
//    sValue.empty() ? GM_UINT_MAX : CxString::toInt32(sValue);
    string sFilePath = CxFileSystem::mergeFilePath(f_sLogPath, sFileName);
    vector<string> sLines;
    CxFile::loadLast(sFilePath, 100, sLines, "\n");
    for (int i = sLines.size(); i > 0; --i)
    {
        const string& sLine = sLines.at(i);
        vector<string> sObject = CxString::split(sLine, ',');
        if (sObject.size() > 3)
        {
            return CxString::toInt32(sObject[1]);
        }
    }
    return GM_UINT_MAX;
}

size_t
RtdbLogFile::loadMonsbChangedIndex()
{
    return loadMeasureChangedIndex(f_sMonsbChangedIndexFileName);
}

size_t
RtdbLogFile::loadYcaddChangedIndex()
{
    return loadMeasureChangedIndex(f_sYcaddChangedIndexFileName);
}

size_t
RtdbLogFile::loadStrawChangedIndex()
{
    return loadMeasureChangedIndex(f_sStrawChangedIndexFileName);
}

int
RtdbLogFile::saveMeasureChangedIndex(size_t iOldIndex, size_t iNewIndex, FILE* oFile)
{
    if (oFile == NULL)
        return FALSE;
    string sText = CxString::format(f_sMeasureChangedIndexText.c_str(), iOldIndex, iNewIndex, CxTime::currentMsepoch());
    size_t iWrote = fwrite(const_cast<char*>(sText.data()), 1, sText.size(), oFile);
    return iWrote;
}

int
RtdbLogFile::saveMonsbChangedIndex(size_t iOldIndex, size_t iNewIndex)
{
    return saveMeasureChangedIndex(iOldIndex, iNewIndex, f_oMonsbChangedIndexFile);
}

int
RtdbLogFile::saveYcaddChangedIndex(size_t iOldIndex, size_t iNewIndex)
{
    return saveMeasureChangedIndex(iOldIndex, iNewIndex, f_oYcaddChangedIndexFile);
}

int
RtdbLogFile::saveStrawChangedIndex(size_t iOldIndex, size_t iNewIndex)
{
    return saveMeasureChangedIndex(iOldIndex, iNewIndex, f_oStrawChangedIndexFile);
}

int
RtdbLogFile::start()
{
    init();

    string sFilePath;
    sFilePath = CxFileSystem::mergeFilePath(f_sLogPath, f_sMonsbChangedIndexFileName);
    f_oMonsbChangedIndexFile = fopen(sFilePath.data(), "ab+");

    sFilePath = CxFileSystem::mergeFilePath(f_sLogPath, f_sYcaddChangedIndexFileName);
    f_oYcaddChangedIndexFile = fopen(sFilePath.data(), "ab+");

    sFilePath = CxFileSystem::mergeFilePath(f_sLogPath, f_sStrawChangedIndexFileName);
    f_oStrawChangedIndexFile = fopen(sFilePath.data(), "ab+");

    return TRUE;
}

int
RtdbLogFile::stop()
{
    return TRUE;
}

std::string
RtdbLogFile::getMeasureValue(const int& value)
{
    return CxString::toString(value);
}

std::string
RtdbLogFile::getMeasureValue(const double& value)
{
    return CxString::toString(value);
}

std::string
RtdbLogFile::getMeasureValue(const StrawValue& value)
{
    int datalen = strlen(value.value) > ci_straw_length ? ci_straw_length : strlen(value.value);
    return std::string(value.value, datalen);
}

std::string
RtdbLogFile::getMeasureLogPath()
{
    return f_sLogPath;
}

int
RtdbLogFile::checkMeaureChangedFiles()
{
    static int iTime = 0;
    // change day
    static msepoch_t dtDayEnd = CxTime::currentDayEnd();
    if (iTime % 30 == 0)
    {
        if (CxTime::currentMsepoch() > dtDayEnd)
        {
            string sDirName = CxTime::currentDayString(0);
            string sFilePath;
            f_sLogPath = CxFileSystem::mergeFilePath(CxAppEnv::dataPath(), sDirName);
            CxFileSystem::createDirMultiLevel(f_sLogPath);
            dtDayEnd = CxTime::currentDayEnd();
        }
    }
    return 0;
}

std::string
RtdbLogFile::getMeasureChanngedText(const int& value,
                                    int iQuality,
                                    msepoch_t dtChangedTime,
                                    int iChangedSourceId,
                                    int iChangedReasonId)
{
    return CxString::format(f_sMonsbChangedIndexText.c_str(),
                            dtChangedTime,
                            value,
                            iQuality,
                            iChangedSourceId,
                            iChangedReasonId);
}

std::string
RtdbLogFile::getMeasureChanngedText(const double& value,
                                    int iQuality,
                                    msepoch_t dtChangedTime,
                                    int iChangedSourceId,
                                    int iChangedReasonId)
{
    return CxString::format(f_sYcaddChangedIndexText.c_str(),
                            dtChangedTime,
                            value,
                            iQuality,
                            iChangedSourceId,
                            iChangedReasonId);
}

std::string
RtdbLogFile::getMeasureChanngedText(const StrawValue& value,
                                    int iQuality,
                                    msepoch_t dtChangedTime,
                                    int iChangedSourceId,
                                    int iChangedReasonId)
{
    int datalen = strlen(value.value) > ci_straw_length ? ci_straw_length : strlen(value.value);
    std::string sValue = std::string(value.value, datalen);
    return CxString::format(f_sStrawChangedIndexText.c_str(),
                            dtChangedTime,
                            sValue.c_str(),
                            iQuality,
                            iChangedSourceId,
                            iChangedReasonId);
}

int
RtdbLogFile::init()
{
    string sDirName = CxTime::currentDayString(0);
    f_sLogPath = CxFileSystem::mergeFilePath(CxAppEnv::dataPath(), sDirName);
    CxFileSystem::createDirMultiLevel(f_sLogPath);

    return TRUE;
}

std::string
RtdbLogFile::loadMeasureChanged(int iMeasureId, const std::string& sDirName)
{
    string sLogPath = CxFileSystem::mergeFilePath(CxAppEnv::dataPath(), sDirName);
    std::string sFileName = CxString::toHexstring(iMeasureId) + ".txt";
    std::string sFilePath = CxFileSystem::mergeFilePath(sLogPath, sFileName);
    string r = CxFile::load(sFilePath);
    return r;
}

std::string
RtdbLogFile::loadMeasureChanged(int iMeasureId, msepoch_t dtBegin, msepoch_t dtEnd)
{
    string r;
    int iDayCount = (dtBegin - dtEnd + GM_MSEPOCH_ONE_DAY -1) / GM_MSEPOCH_ONE_DAY;
    if (iDayCount > 7) return r;
    for (int i = 0; i < iDayCount; ++i)
    {
        string sDirName = CxTime::toStringDay((dtBegin + i * GM_MSEPOCH_ONE_DAY) % 1000, 0);
        string sLogPath = CxFileSystem::mergeFilePath(CxAppEnv::dataPath(), sDirName);
        std::string sFileName = CxString::toHexstring(iMeasureId) + ".txt";
        std::string sFilePath = CxFileSystem::mergeFilePath(sLogPath, sFileName);
        if (i == 0 || i == iDayCount - 1)
        {
            vector<string> sLines;
            CxFile::load(sFilePath, sLines, '\n');
            size_t iLineBegin = 0;
            for (size_t j = 0; j < sLines.size(); ++j)
            {
                const string& sLine = sLines.at(j);
                string sDt(sLine.data(), sLine.find(','));
                int64 r;
                stringstream ss(sDt);
                ss >> r;
                if (r > dtBegin)
                {
                    iLineBegin = j;
                    break;
                }
            }
            size_t iLineEnd = sLines.size()-1;
            for (size_t j = sLines.size()-1; j >= 0; --j)
            {
                const string& sLine = sLines.at(j);
                string sDt(sLine.data(), sLine.find(','));
                int64 r;
                stringstream ss(sDt);
                ss >> r;
                if (r < dtEnd)
                {
                    iLineEnd = j;
                    break;
                }
            }
            for (size_t j = iLineBegin; j <= iLineEnd; ++j)
            {
                r += ( sLines.at(j) + "\n" );
            }
        }
        else
        {
            r += CxFile::load(sFilePath);
        }
    }

    return r;
}
