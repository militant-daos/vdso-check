#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <vector>
#include <dirent.h>

void ProcessEntity(struct dirent* entity);
void ProcessDirectory(const std::string& directory);
void ProcessFile(const std::string& file, const std::string& pid);


const std::string PROC_PATH("/proc"); // Please note: MUST END WITH '/' SEPARATOR!
const char SEPARATOR = '/';

const std::string VDSO("[vdso]");
const std::string MAPS("maps");

const std::size_t EXEC_BUFFER_SIZE = 128;


bool IsPidDir(const std::string& path, std::string& pid)
{
    if(path.empty())
    {
        return false;
    }

    std::size_t sep_pos = path.find_last_of(SEPARATOR);
    std::string maybe_pid;
    
    // path == 1234 OR path == /proc/1234

    if(sep_pos == std::string::npos)
    {
        maybe_pid = path;
    }
    else
    {
        maybe_pid = path.substr(sep_pos + 1);
        
        if(maybe_pid.empty())
        {
            return false;
        }
    }
    
    // Check whether the char sequence is a number.    
    for(auto c = maybe_pid.begin(); c != maybe_pid.end(); ++c)
    {
        if(*c < '0' || *c > '9')
        {
            return false;
        }
    }

    pid = maybe_pid;
        
    return true;
}

std::string Exec(const std::string& cmd) 
{
    std::vector<char> buffer;
    buffer.resize(EXEC_BUFFER_SIZE);

    std::string result;
    FILE* pipe = popen(cmd.c_str(), "r");

    if(NULL != pipe) 
    {
        while(!feof(pipe)) 
        {
            if (fgets(&buffer.at(0), buffer.size(), pipe) != NULL)
            {
                result += &buffer.at(0);
            }
        }

        pclose(pipe);       
    }
    
    return result;
}

void ProcessEntity(struct dirent* entity, const std::string& parent_directory)
{
    if(entity == NULL)
    {
        return;
    }

    std::string pid;
    if(entity->d_type == DT_DIR)
    {
        if(entity->d_name[0] == '.')
        {
            return;
        }

        std::string dir_name(entity->d_name);

        if(IsPidDir(dir_name, pid))
        {
            ProcessDirectory(parent_directory + SEPARATOR + dir_name);
        }
    } 
    else if(entity->d_type == DT_REG)
    {
        std::string file_name(entity->d_name);
        if(file_name == MAPS)
        {
            IsPidDir(parent_directory, pid);
            ProcessFile(parent_directory + SEPARATOR + file_name, pid);
        }
    }
}


void ProcessDirectory(const std::string& directory)
{
    auto dir = opendir(directory.c_str());

    if(NULL == dir)
    {
        std::cerr << "Cannot open directory: " << directory << std::endl;
        return;
    }

    auto entity = readdir(dir);

    while(entity != NULL)
    {
        ProcessEntity(entity, directory);
        entity = readdir(dir);
    }

    closedir(dir);
}

void ProcessFile(const std::string& file, const std::string& pid)
{
    std::ifstream input(file.c_str());
    if(!input)
    {
        std::cout << "Failed to open " << file << std::endl;
        return;
    }

    std::string map_line;
    while(std::getline(input, map_line))
    {
        // Search for ...[vdso] entries. 
        if(map_line.find_first_of(VDSO) != std::string::npos)
        {
            // Get process name by PID
            std::string ps_query("ps " + pid);
            std::string ps_stats = Exec(ps_query.c_str());
            std::cout << "! " << ps_stats.substr(ps_stats.find_first_of('\n') + 1);
            break;
        }
    }
}

int main(int argc, char** argv)
{
    ProcessDirectory(PROC_PATH);    
    return 0;
}
