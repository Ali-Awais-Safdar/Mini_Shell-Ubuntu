// mini_shell.cpp

#include <iostream>
#include <cstring>
#include <fstream>
#include <unistd.h>
#include <cstdlib>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <ctime>
#include <cstdio>
#include <dirent.h>

#define TOKENSIZE 100

void StrTokenizer(char *line, char **argv);
void myExecvp(char **argv);
int GetEnv();
void ExecuteCommand(char **argv);
void PrintWorkingDirectory();
void ShowHelp();
void PrintCurrentDateTime();
void EchoCommand(char **argv);
void CreateDirectory(char **argv);
void RemoveFile(char **argv);
void ListFiles();
void ChangeDirectory(char **argv);
void CopyFile(char **argv);
void RenameFile(char **argv);
void FilePermissions(char **argv);
void FileSize(char **argv);
void CountLines(char **argv);
void ReadFile(char **argv);
void WriteFile(char **argv);
void AppendToFile(char **argv);
void SearchFile(char **argv);
void SwapFile(char **argv);
void PipeCommandExample();


int main()
{
    char *arr[250];
    char input[250];
    char *argv[TOKENSIZE];

    while (true)
    {
        std::cout << "Ali_Huzi_Shell-> ";
        std::cin.getline(input, 250);
        StrTokenizer(input, argv);
        if (strcmp(input, "exit") == 0)
        {
            break;
        }
        else if (strlen(input) == 0)
        {
            continue;
        }
        else if (strcmp(input, "pwd") == 0)
        {
            PrintWorkingDirectory();
            continue;
        }
        else if (strcmp(input, "help") == 0)
        {
            ShowHelp();
            continue;
        }
        else if (strcmp(input, "date") == 0)
        {
            PrintCurrentDateTime();
            continue;
        }
        else if (strcmp(input, "echo") == 0)
        {
            EchoCommand(argv);
            continue;
        }
        else if (strcmp(input, "mkdir") == 0)
        {
            CreateDirectory(argv);
            continue;
        }
        else if (strcmp(input, "rm") == 0)
        {
            RemoveFile(argv);
            continue;
        }
        else if (strcmp(input, "env") == 0)
        {
            GetEnv();
            continue;
        }
        else if (strcmp(input, "list") == 0)
        {
            ListFiles();
            continue;
        }
        else if (strcmp(input, "change") ==0)
        {
            ChangeDirectory(argv);
            continue;
        }
        else if (strcmp(input, "copy") ==0)
        {
            CopyFile(argv);
            continue;
        }
        else if (strcmp(input, "rename") ==0)
        {
            RenameFile(argv);
            continue;
        }
        else if (strcmp(input, "perm") ==0)
        {
            FilePermissions(argv);
            continue;
        }
        else if (strcmp(input, "size") == 0)
        {
            FileSize(argv);
            continue;
        }
        else if (strcmp(input, "count") ==0)
        {
            CountLines(argv);
            continue;
        }
        else if (strcmp(input, "read") ==0)
        {
            ReadFile(argv);
            continue;
        }
        else if (strcmp(input, "write") ==0)
        {
            WriteFile(argv);
            continue;
        }
        else if (strcmp(input, "append") ==0)
        {
            AppendToFile(argv);
            continue;
        }
        else if (strcmp(input, "search") == 0)
        {
            SearchFile(argv);
            continue;
        }
        else if (strcmp(input, "swap") == 0)
        {
            SwapFile(argv);
            continue;
        }
        else if (strcmp(input, "pipe") == 0)
        {
            PipeCommandExample();
            continue;
        }
        else
        {
            ExecuteCommand(argv);
        }
    }
    return 0;
}


void PipeCommandExample()
{
    int pipefd[2];
    char buffer[256];

    if (pipe(pipefd) == -1)
    {
        std::cerr << "ERROR: Failed to create pipe" << std::endl;
        return;
    }

    pid_t pid = fork();
    if (pid == -1)
    {
        std::cerr << "ERROR: Failed to fork" << std::endl;
        return;
    }

    if (pid == 0)
    {
        // Child process
        close(pipefd[0]);  // Close unused read end of the pipe

        // Redirect stdout to the write end of the pipe
        if (dup2(pipefd[1], STDOUT_FILENO) == -1)
        {
            std::cerr << "ERROR: Failed to redirect stdout" << std::endl;
            exit(EXIT_FAILURE);
        }

        // Close the write end of the pipe
        close(pipefd[1]);

        // Execute the command "ls -l" using execvp
        char* command[] = { "ls", "-l", nullptr };
        execvp(command[0], command);

        // If execvp fails, the following code will be executed
        std::cerr << "ERROR: Failed to execute command" << std::endl;
        exit(EXIT_FAILURE);
    }
    else
    {
        // Parent process
        close(pipefd[1]);  // Close unused write end of the pipe

        // Read the output of the command from the read end of the pipe
        ssize_t bytesRead = read(pipefd[0], buffer, sizeof(buffer));
        if (bytesRead == -1)
        {
            std::cerr << "ERROR: Failed to read from pipe" << std::endl;
            return;
        }

        std::cout << "Command output:\n" << std::string(buffer, bytesRead) << std::endl;

        close(pipefd[0]);  // Close read end of the pipe

        // Wait for the child process to exit
        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status))
        {
            int exitStatus = WEXITSTATUS(status);
            std::cout << "Child process exited with status: " << exitStatus << std::endl;
        }
    }
}


void SwapFile(char **argv)
{
    // Check if both source and destination files are provided
    if (argv[1] == nullptr || argv[2] == nullptr)
    {
        std::cout << "Please provide both source and destination file names." << std::endl;
        return;
    }

    std::ifstream sourceFile(argv[1]);
    std::ifstream destFile(argv[2]);

    // Check if both files exist
    if (!sourceFile || !destFile)
    {
        std::cout << "One or both of the files does not exist." << std::endl;
        return;
    }

    std::string sourceContent((std::istreambuf_iterator<char>(sourceFile)),
                              (std::istreambuf_iterator<char>()));
    std::string destContent((std::istreambuf_iterator<char>(destFile)),
                            (std::istreambuf_iterator<char>()));

    std::ofstream tempFile("temp.txt");

    // Write the content of the source file to the temporary file
    tempFile << sourceContent;
    tempFile.close();

    sourceFile.close();
    destFile.close();

    std::ofstream source(argv[1]);
    std::ofstream dest(argv[2]);

    // Write the content of the destination file to the source file
    source << destContent;

    // Write the content of the source file to the destination file
    dest << sourceContent;

    source.close();
    dest.close();

    std::cout << "Swapped contents of " << argv[1] << " and " << argv[2] << " successfully." << std::endl;
}

void myExecvp(char **argv)
{
    pid_t pid;
    int status;
    int childStatus;
    pid = fork();
    if (pid == 0)
    {
        childStatus = execvp(argv[0], argv);
        if (childStatus < 0)
        {
            std::cerr << "ERROR: wrong input" << std::endl;
        }
        exit(0);
    }
    else if (pid < 0)
    {
        std::cerr << "something went wrong!" << std::endl;
    }
    else
    {
        waitpid(pid, &status, 0);
    }
}

void StrTokenizer(char *input, char **argv)
{
    char *stringTokenized;
    stringTokenized = strtok(input, " ");
    while (stringTokenized != NULL)
    {
        *argv++ = stringTokenized;
        stringTokenized = strtok(NULL, " ");
    }

    *argv = NULL;
}
		
void PrintWorkingDirectory()
{
    char cwd[256];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        std::cout << "Current working directory: " << cwd << std::endl;
    }
    else
    {
        std::cerr << "ERROR: Failed to get current working directory" << std::endl;
    }
}

void ShowHelp()
{
    std::cout << "Available commands:" << std::endl;
    std::cout << "  pwd: Print current working directory" << std::endl;
    std::cout << "  help: Show available commands" << std::endl;
    std::cout << "  exit: Exit the shell" << std::endl;
    std::cout << "  date: Display the current Date" << std::endl;
    std::cout << "  echo: prints a statement into the terminal" << std::endl;
    
    std::cout << "  list: List files and directories in the current directory" << std::endl;
    std::cout << "  mkdir <directory>: Create a new directory" << std::endl;
    std::cout << "  change <directory>: Change the current Directory" << std::endl;
    std::cout << "  rename <file>: Rename a file" << std::endl;
    std::cout << "  perm: Show Permissions of a file" << std::endl;
    std::cout << "  size: Show size of a file" << std::endl;
    std::cout << "  count: Show line count of a file" << std::endl;
    std::cout << "  read: Display contents of a file" << std::endl;
    std::cout << "  pipe: Give Simple Working of Pipes by executing a forked ls - l command" << std::endl;
    std::cout << "  write: Overwrite content to a file" << std::endl;
    std::cout << "  append: Write content to the end of a file" << std::endl;
    std::cout << "  copy <source>: Copy a file" << std::endl;
    std::cout << "  rm <source>: Remove File" << std::endl;
}
void PrintCurrentDateTime()
{
    time_t now = time(0);
    char* dateTime = ctime(&now);
    if (dateTime != nullptr)
    {
        std::cout << "Current date and time: " << dateTime;
    }
    else
    {
        std::cerr << "ERROR: Failed to get current date and time" << std::endl;
    }
}

void EchoCommand(char **argv)
{
    ++argv;  // Skip the "echo" command itself
    while (*argv != nullptr)
    {
        std::cout << *argv << ' ';
        ++argv;
    }
    std::cout << std::endl;
}


void CreateDirectory(char **argv)
{
    if (argv[1] != nullptr)
    {
        int result = mkdir(argv[1], 0777);  // Provide appropriate permissions
        if (result == 0)
        {
            std::cout << "Directory created successfully" << std::endl;
        }
        else
        {
            std::cerr << "ERROR: Failed to create directory" << std::endl;
        }
    }
    else
    {
        std::cerr << "ERROR: Directory name not provided" << std::endl;
    }
}



void RemoveFile(char **argv)
{
    if (argv[1] != nullptr)
    {
        int result = std::remove(argv[1]);
        if (result == 0)
        {
            std::cout << "File deleted successfully" << std::endl;
        }
        else
        {
            std::cerr << "ERROR: Failed to delete file" << std::endl;
        }
    }
    else
    {
        std::cerr << "ERROR: File name not provided" << std::endl;
    }
}

int GetEnv()
{
    char *path2;
    char *Tokenized;
    path2 = getenv("PATH");
    Tokenized = strtok(path2, ":");
    int k = 0;
    while (Tokenized != NULL)
    {
        Tokenized = strtok(NULL, ":");
        k++;
    }

    return k;
}

void ListFiles()
{
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(".")) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            if (ent->d_type == DT_REG)  // Check if it's a regular file
            {
                std::cout << ent->d_name << std::endl;
            }
        }
        closedir(dir);
    }
    else
    {
        std::cerr << "ERROR: Failed to open directory" << std::endl;
    }
}

void ChangeDirectory(char **argv)
{
    if (argv[1] != nullptr)
    {
        int result = chdir(argv[1]);
        if (result == 0)
        {
            std::cout << "Current working directory changed successfully" << std::endl;
        }
        else
        {
            std::cerr << "ERROR: Failed to change working directory" << std::endl;
        }
    }
    else
    {
        std::cerr << "ERROR: Directory name not provided" << std::endl;
    }
}

void CopyFile(char **argv)
{
    if (argv[1] != nullptr && argv[2] != nullptr)
    {
        int result = std::rename(argv[1], argv[2]);
        if (result == 0)
        {
            std::cout << "File copied successfully" << std::endl;
        }
        else
        {
            std::cerr << "ERROR: Failed to copy file" << std::endl;
        }
    }
    else
    {
        std::cerr << "ERROR: Source or destination file not provided" << std::endl;
    }
}

void RenameFile(char **argv)
{
    if (argv[1] != nullptr && argv[2] != nullptr)
    {
        int result = std::rename(argv[1], argv[2]);
        if (result == 0)
        {
            std::cout << "File renamed successfully" << std::endl;
        }
        else
        {
            std::cerr << "ERROR: Failed to rename file" << std::endl;
        }
    }
    else
    {
        std::cerr << "ERROR: Source or new file name not provided" << std::endl;
    }
}

void FilePermissions(char **argv)
{
    if (argv[1] != nullptr)
    {
        struct stat fileStat;
        if (stat(argv[1], &fileStat) == 0)
        {
            std::cout << "File permissions: ";
            std::cout << ((fileStat.st_mode & S_IRUSR) ? "r" : "-");
            std::cout << ((fileStat.st_mode & S_IWUSR) ? "w" : "-");
            std::cout << ((fileStat.st_mode & S_IXUSR) ? "x" : "-");
            std::cout << ((fileStat.st_mode & S_IRGRP) ? "r" : "-");
            std::cout << ((fileStat.st_mode & S_IWGRP) ? "w" : "-");
            std::cout << ((fileStat.st_mode & S_IXGRP) ? "x" : "-");
            std::cout << ((fileStat.st_mode & S_IROTH) ? "r" : "-");
            std::cout << ((fileStat.st_mode & S_IWOTH) ? "w" : "-");
            std::cout << ((fileStat.st_mode & S_IXOTH) ? "x" : "-");
            std::cout << std::endl;
        }
        else
        {
            std::cerr << "ERROR: Failed to get file permissions" << std::endl;
        }
    }
    else
    {
        std::cerr << "ERROR: File name not provided" << std::endl;
    }
}

void FileSize(char **argv)
{
    if (argv[1] != nullptr)
    {
        struct stat fileStat;
        if (stat(argv[1], &fileStat) == 0)
        {
            std::cout << "File size: " << fileStat.st_size << " bytes" << std::endl;
        }
        else
        {
            std::cerr << "ERROR: Failed to get file size" << std::endl;
        }
    }
    else
    {
        std::cerr << "ERROR: File name not provided" << std::endl;
    }
}


void CountLines(char **argv)
{
    if (argv[1] != nullptr)
    {
        std::ifstream file(argv[1]);
        if (file)
        {
            int lineCount = 0;
            std::string line;
            while (std::getline(file, line))
            {
                lineCount++;
            }
            std::cout << "Line count: " << lineCount << std::endl;
        }
        else
        {
            std::cerr << "ERROR: Failed to open file" << std::endl;
        }
    }
    else
    {
        std::cerr << "ERROR: File name not provided" << std::endl;
    }
}

void ReadFile(char **argv)
{
    if (argv[1] != nullptr)
    {
        std::ifstream file(argv[1]);
        if (file)
        {
            std::string line;
            while (std::getline(file, line))
            {
                std::cout << line << std::endl;
            }
        }
        else
        {
            std::cerr << "ERROR: Failed to open file" << std::endl;
        }
    }
    else
    {
        std::cerr << "ERROR: File name not provided" << std::endl;
    }
}

void WriteFile(char** args)
{
    if (args[1] == nullptr || args[2] == nullptr)
    {
        std::cout << "Invalid arguments. Usage: write <filename> <content>" << std::endl;
        return;
    }

    std::string filename = args[1];
    std::string content = args[2];

    std::ofstream outputFile(filename);

    if (outputFile.is_open())
    {
        outputFile << content;
        outputFile.close();
        std::cout << "Content written to the file successfully." << std::endl;
    }
    else
    {
        std::cout << "Unable to open the file." << std::endl;
    }
}

void AppendToFile(char **argv)
{
    if (argv[1] != nullptr)
    {
        std::ofstream file(argv[1], std::ios_base::app);
        if (file)
        {
            std::cout << "Enter text to append (press Ctrl+D to finish):" << std::endl;

            int ch;
            while ((ch = std::cin.get()) != EOF)
            {
                file.put(ch);
            }

            if (!std::cin.bad()) // Check if input stream is in a good state
            {
                std::cout << "Text appended to file successfully" << std::endl;
            }
            else
            {
                std::cerr << "ERROR: Failed to read input" << std::endl;
            }
        }
        else
        {
            std::cerr << "ERROR: Failed to open file" << std::endl;
        }
    }
    else
    {
        std::cerr << "ERROR: File name not provided" << std::endl;
    }
}
void SearchFile(char **argv)
{
    if (argv[1] != nullptr && argv[2] != nullptr)
    {
        std::ifstream file(argv[1]);
        if (file)
        {
            std::string line;
            int lineCount = 0;
            while (std::getline(file, line))
            {
                lineCount++;
                if (line.find(argv[2]) != std::string::npos)
                {
                    std::cout << "Match found in line " << lineCount << ": " << line << std::endl;
                }
            }
        }
        else
        {
            std::cerr << "ERROR: Failed to open file" << std::endl;
        }
    }
    else
    {
        std::cerr << "ERROR: File name or search query not provided" << std::endl;
    }
}


void ExecuteCommand(char **argv)
{
    if (strcmp(argv[0], "pwd") == 0)
    {
        PrintWorkingDirectory();
    }
    else if (strcmp(argv[0], "help") == 0)
    {
        ShowHelp();
    }
    else if (strcmp(argv[0], "date") == 0)
    {
        PrintCurrentDateTime();
    }
    else if (strcmp(argv[0], "echo") == 0)
    {
        EchoCommand(argv);
    }
    else if (strcmp(argv[0], "mkdir") == 0)
    {
        CreateDirectory(argv);
    }
    else if (strcmp(argv[0], "env") == 0)
    {
        GetEnv();
    }
    else if (strcmp(argv[0], "list") == 0)
    {
        ListFiles();
    }
    else if (strcmp(argv[0], "change") == 0)
    {
        ChangeDirectory(argv);
    }
    else if (strcmp(argv[0], "copy") == 0)
    {
        CopyFile(argv);
    }
    else if (strcmp(argv[0], "rename") == 0)
    {
        RenameFile(argv);
    }
    else if (strcmp(argv[0], "perm") == 0)
    {
        FilePermissions(argv);
    }
    else if (strcmp(argv[0], "size") == 0)
    {
        FileSize(argv);
    }
       else if (strcmp(argv[0], "count") == 0)
    {
        CountLines(argv);
    }
    else if (strcmp(argv[0], "read") == 0)
    {
        ReadFile(argv);
    }
    else if (strcmp(argv[0], "write") == 0)
    {
        WriteFile(argv);
    }
     else if (strcmp(argv[0], "append") == 0)
    {
        AppendToFile(argv);
    }
    else if (strcmp(argv[0], "pipe") == 0)
    {
        PipeCommandExample();
    }
    else
    {
        std::cerr << "ERROR: Command not found" << std::endl;
    }
    
}
