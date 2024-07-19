#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

char *convertPath(char *path);
char *get_txt_path();
void add_path(char *name, char *path);
char *read_path(char *name, char *mode);
void read_cmd(char *cmd[]);
void del_cmd(char *name);
void change_cmd(char *nae, char *path);

char *convertPath(char *path)
{
    int i;
    for (i = 0; path[i] != '\0'; i++)
    {
        if (path[i] == '/')
        {
            path[i] = '\\';
        }
    }
    return path;
}

char *get_txt_path()
{
    char cwd[PATH_MAX];
    char *filepath = __FILE__;
    if (_fullpath(cwd, filepath, PATH_MAX) == NULL)
    {
        perror("realpath");
        return NULL;
    }

    char folder_path[PATH_MAX];
    char *last_slash = strrchr(cwd, '\\');
    if (last_slash != NULL)
    {
        *last_slash = '\0'; // 截断字符串
        strcpy(folder_path, cwd);
    }
    else
    {
        strcpy(folder_path, cwd);
    }

    char *filename = (char *)malloc(PATH_MAX * sizeof(char));
    strcpy(filename, folder_path);
    strcat(filename, "\\pathlist.txt");
    return filename;
}

void add_path(char *name, char *path)
{
    char *filename = get_txt_path();
    FILE *file = fopen(filename, "a+");
    if (fseek(file, 0, SEEK_END) != 0)
    {
        perror("filed to open file");
        return;
    }

    char buffer[256];
    snprintf(buffer, sizeof(buffer), "%s\n%s\n", name, path);
    fprintf(file, buffer);

    fclose(file);
    printf("Successfully add: name= %s , path= %s ", name, path);
}

char *read_path(char *name, char *mode)
{
    char *filename = get_txt_path();
    FILE *file = fopen(filename, "r");
    if (fseek(file, 0, SEEK_SET) != 0)
    {
        perror("filed to open file");
        return NULL;
    }

    char *line = (char *)malloc(PATH_MAX * sizeof(char));
    int isreturn = 0;
    int index = 0;
    while (fgets(line, 256, file) != NULL)
    {
        if (strcmp(mode, "a") == 0)
        {
            printf("%s", line);
            continue;
        }

        if (isreturn == 1)
        {
            fclose(file);
            return line;
        }
        if (index % 2 == 1)
        {
            index++;
            continue;
        }
        if (line[strlen(line) - 1] == '\n')
        {
            line[strlen(line) - 1] = '\0';
        }

        if (strcmp(line, name) == 0)
        {
            isreturn = 1;
        }
        index++;
    }

    fclose(file);
    return NULL;
}

void read_cmd(char *cmd[])
{
    if (strcmp(cmd[1], "a") == 0)
    {
        if (read_path(cmd[2], "s"))
        {
            char *y[50];
            printf("Do you want to change the var?(y/n)");
            fgets(y, 50, stdin);
            if (strcmp(y, "y\n") == 0 || strcmp(y, "Y\n") == 0)
            {
                change_cmd(cmd[2], cmd[3]);
            }
        }
        else
        {
            add_path(cmd[2], cmd[3]);
        }
    }
    else if (strcmp(cmd[1], "d") == 0)
    {
        del_cmd(cmd[2]);
    }
    else if (strcmp(cmd[1], "r") == 0)
    {
        read_path("", "a");
    }

    else
    {
        char FilePath[256];
        if (read_path(cmd[1], "s"))
        {
            strcpy(FilePath, read_path(cmd[1], "s"));
        }
        else
        {
            strcpy(FilePath, cmd[1]);
        }
        strcpy(FilePath, convertPath(FilePath));

        char command[256];
        sprintf(command, "explorer \"%s\"", FilePath);
        system(command);
    }
}

void del_cmd(char *name)
{
    int is_return = 0;
    char *filename = get_txt_path();
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        perror("failed to open file");
        return;
    }

    char *temp_filename = "temp.txt";
    FILE *temp_file = fopen(temp_filename, "w");
    if (temp_file == NULL)
    {
        perror("failed to create temp file");
        fclose(file);
        return;
    }

    char *line = (char *)malloc(PATH_MAX * sizeof(char));
    while (fgets(line, 256, file) != NULL)
    {
        if (line[strlen(line) - 1] == '\n')
        {
            line[strlen(line) - 1] = '\0';
        }

        if (strcmp(line, name) == 0 || is_return == 1)
        {
            if (is_return == 1)
            {
                is_return = 0;
                continue;
            }
            if (is_return == 0)
            {
                is_return = 1;
            }
            continue;
        }
        fputs(line, temp_file); // 将不需要删除的内容写入临时文件
        if (line != "")
        {
            fputs("\n", temp_file); // 写入换行符
        }
    }

    fclose(file);
    fclose(temp_file);

    // 替换原始文件
    if (remove(filename) != 0)
    {
        perror("failed to delete original file");
        return;
    }
    if (rename(temp_filename, filename) != 0)
    {
        perror("failed to rename temp file");
        return;
    }
    printf("Successfully delete: name= %s", name);
    return;
}

void change_cmd(char *name, char *path)
{
    int is_return = 0;
    char *filename = get_txt_path();
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        perror("failed to open file");
        return;
    }

    char *temp_filename = "temp.txt";
    FILE *temp_file = fopen(temp_filename, "w");
    if (temp_file == NULL)
    {
        perror("failed to create temp file");
        fclose(file);
        return;
    }

    char *line = (char *)malloc(PATH_MAX * sizeof(char));
    while (fgets(line, 256, file) != NULL)
    {
        if (line[strlen(line) - 1] == '\n')
        {
            line[strlen(line) - 1] = '\0';
        }

        if (is_return == 1)
        {
            fputs(path, temp_file);
            fputs("\n", temp_file);
            is_return = 0;
            continue;
        }
        if (strcmp(line, name) == 0)
        {
            is_return = 1;
        }

        fputs(line, temp_file); // 将不需要删除的内容写入临时文件
        if (line != "")
        {
            fputs("\n", temp_file); // 写入换行符
        }
    }

    fclose(file);
    fclose(temp_file);

    // 替换原始文件
    if (remove(filename) != 0)
    {
        perror("failed to delete original file");
        return;
    }
    if (rename(temp_filename, filename) != 0)
    {
        perror("failed to rename temp file");
        return;
    }
    printf("Successfully change: name= %s , path= %s ", name, path);
    return;
}

int main(int argc, char *argv[])
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // 获取当前控制台窗口
    HWND consoleWindow = GetConsoleWindow();

    // 隐藏控制台窗口
    ShowWindow(consoleWindow, SW_HIDE);

    if (argc == 1)
    {
        return 0;
    }
    read_cmd(argv);
    return 0;
}
