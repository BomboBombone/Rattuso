#include "Log.h"

std::string GetCWD() {
    WCHAR buffer[MAX_PATH] = { 0 };
    GetModuleFileNameW(NULL, buffer, MAX_PATH);
    std::wstring ws(buffer);
    std::string file_path(ws.begin(), ws.end());
    std::wstring::size_type pos = file_path.find_last_of("\\/");
    return file_path.substr(0, pos + 1);
}

void Log::Write(LogLevel& level)
{
}

Log::Log()
{
    logFolder = GetCWD() + "log";
    fs::create_directory(logFolder);


}
