/*
 * filepath.c / filepath functions
 *
 * Ethan Liu & Leo Liang/ CS5600 / Northeastern University
 * Spring 2023 / April 20, 2023
 *
 */
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <libgen.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include "filepath.h"

#define CONFIG_FILE1 "config1.txt"
#define CONFIG_FILE2 "config2.txt"

void last_accessed_time(Filepath *driver, const char* driver_log, const char* Command) {
    FILE* fp;
    struct stat fileStat;
    time_t t;
    struct tm* tm_info;
    char timeStr[30];
    struct timeval tv[2];

    // Open the file in append mode
    fp = fopen(driver_log, "a");
    if (fp == NULL) {
        printf("Error: Failed to open file '%s'\n", driver_log);
        return;
    }

    // Get the current local time
    t = time(NULL);
    tm_info = localtime(&t);

    // Format the time string as "YYYY-MM-DD HH:MM:SS"
    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", tm_info);
    strcat(timeStr, " ");
    strcat(timeStr, Command);

    // Append the time string to the file
    fprintf(fp, "%s\n", timeStr);

    // Close the file
    fclose(fp);

    // Update the file's last access time
    if (stat(driver_log, &fileStat) == 0) {
        tv[0].tv_sec = fileStat.st_atime;
        tv[1].tv_sec = time(NULL);
        tv[0].tv_usec = tv[1].tv_usec = 0;
        utimes(driver_log, tv);
    } else {
        printf("Error: Failed to get file status for '%s'\n", driver_log);
    }
}


int check_directory_availability(Filepath *primary, Filepath *backup) {
    int ret_val = 0;

    if (access(primary->root_path, F_OK) != -1) {
        ret_val += 1;
    }
    if (access(backup->root_path, F_OK) != -1) {
        ret_val += 2;
    }

    switch (ret_val) {
        case 0:
            //printf("[SYSTEM]Primary and Backup are OFF\n");
            return -1;
        case 1:
            //printf("[SYSTEM]Primary ON Backup OFF\n");
            return 1;
        case 2:
            //printf("[SYSTEM]Primary OFF Backup ON\n");
            return 2;
        default:
            //printf("[SYSTEM]Primary and Back are available\n");
            return 0;
    }
}


void mount_server_storage(Filepath *primary, Filepath *backup)
{
    // Read USB drive paths from config file
    FILE *config_file1 = fopen(CONFIG_FILE1, "r");
    FILE *config_file2 = fopen(CONFIG_FILE2, "r");
    if (config_file1 == NULL && config_file2 == NULL)
    {
        perror("[SYSTEM]Failed to open config file\n");
        strcpy(primary->root_path, "");
        strcpy(backup->root_path, "");
        return;
    }

    if (fgets(primary->root_path, MAX_PATH_LEN, config_file1) == NULL)
    {
        perror("[SYSTEM]Failed to read primary USB drive path from config file\n");
        return;
    }

    primary->root_path[strcspn(primary->root_path, "\n")] = '\0';
    printf("[SYSTEM]Primary storage configured to: %s\n", primary->root_path);

    if (fgets(backup->root_path, MAX_PATH_LEN, config_file2) == NULL)
    {
        perror("[SYSTEM]Failed to read backup USB drive path from config file\n");
        return;
    }

    backup->root_path[strcspn(backup->root_path, "\n")] = '\0';
    printf("[SYSTEM]Backup usb drive configured to: %s\n", backup->root_path);

    fclose(config_file1);
    fclose(config_file2);
}

void update_full_path(char *file_name, Filepath *path)
{
    strcpy(path->directory_path, file_name);

    // Construct file path using primary USB drive path
    char *file_path = malloc(MAX_PATH_LEN + MAX_FILE_LEN + 1);
    if (file_path == NULL)
    {
        perror("[PRIMARY]Failed to allocate memory for file path");
        return;
    }
    snprintf(file_path, MAX_PATH_LEN + MAX_FILE_LEN + 1, "%s/%s", path->root_path, path->directory_path);

    // Get last modified time for file
    struct stat file_info;
    if (stat(file_path, &file_info) == -1)
    {
        //perror("[PRIMARY]File not existed in server");
    }
    path->last_modified_time = ctime(&file_info.st_mtime);
    strcpy(path->full_path, file_path);
    free(file_path);
    //printf("[PRIMARY]File path is: %s\n", path->full_path);
    return;
}

void update_backup_full_path(Filepath *primary, Filepath *backup)
{
    strncpy(backup->directory_path, primary->directory_path, MAX_PATH_LEN);
    char *file_path = malloc(MAX_PATH_LEN + MAX_FILE_LEN + 1);
    if (file_path == NULL)
    {
        perror("[SYSTEM]Failed to allocate memory for file path");
        return;
    }
    snprintf(file_path, MAX_PATH_LEN + MAX_FILE_LEN + 1, "%s/%s", backup->root_path, primary->directory_path);
    strcpy(backup->full_path, file_path);
    //printf("[SYSTEM]PRIMARY file path: %s/%s\n", primary->root_path, primary->directory_path);
    //printf("[SYSTEM]BACKUP file path: %s\n", backup->full_path);
    free(file_path);
    return;
}

void copy_file(const char *src_path, const char *dst_path)
{
    FILE *src_file = fopen(src_path, "rb");
    FILE *dst_file = fopen(dst_path, "wb");

    if (src_file == NULL || dst_file == NULL)
    {
        printf("[SYSTEM]Error: could not copy file %s to %s\n", src_path, dst_path);
        return;
    }

    char buffer[1024];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), src_file)) > 0)
    {
        fwrite(buffer, 1, bytes_read, dst_file);
    }

    fclose(src_file);
    fclose(dst_file);
}

void copy_directory(const char *source_path, const char *destination_path)
{
    DIR *source_dir = opendir(source_path);
    if (source_dir == NULL)
    {
        return;
    }
    DIR *destination_dir = opendir(destination_path);
    if (destination_dir == NULL)
    {
        mkdir(destination_path, 0777);
    }
    else
    {
        closedir(destination_dir);
    }
    struct dirent *entry;
    while ((entry = readdir(source_dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }
        char source_subpath[strlen(source_path) + strlen(entry->d_name) + 2];
        sprintf(source_subpath, "%s/%s", source_path, entry->d_name);
        char destination_subpath[strlen(destination_path) + strlen(entry->d_name) + 2];
        sprintf(destination_subpath, "%s/%s", destination_path, entry->d_name);
        if (entry->d_type == DT_DIR)
        {
            copy_directory(source_subpath, destination_subpath);
        }
        else
        {
            copy_file(source_subpath, destination_subpath);
        }
    }
    closedir(source_dir);
}

int is_directory(const char *path)
{
    struct stat path_stat;
    if (stat(path, &path_stat) != 0)
    {
        perror("stat failed");
        return 0;
    }
    return S_ISDIR(path_stat.st_mode);
}

void find_missing_files(const char *primary_dir, const char *backup_dir)
{
    DIR *primary_dir_ptr = opendir(primary_dir);
    if (!primary_dir_ptr)
    {
        printf("Failed to open primary directory");
        return;
    }

    struct dirent *dir_entry;

    while ((dir_entry = readdir(primary_dir_ptr)) != NULL)
    {
        if (strcmp(dir_entry->d_name, ".") == 0 || strcmp(dir_entry->d_name, "..") == 0)
        {
            continue;
        }

        char primary_path[MAX_PATH_LEN];
        snprintf(primary_path, MAX_PATH_LEN, "%s/%s", primary_dir, dir_entry->d_name);

        if (is_directory(primary_path))
        {
            // Recursively search subdirectories
            char backup_path[MAX_PATH_LEN];
            snprintf(backup_path, MAX_PATH_LEN, "%s/%s", backup_dir, dir_entry->d_name);
            find_missing_files(primary_path, backup_path);
        }
        else
        {
            // Check if the file exists in the backup directory
            char backup_path[MAX_PATH_LEN];
            snprintf(backup_path, MAX_PATH_LEN, "%s/%s", backup_dir, dir_entry->d_name);
            if (access(backup_path, F_OK) != 0)
            {
                // printf("Missing file: %s\n", primary_path);
                printf("Directory '%s' is missing in '%s'\n", primary_path, backup_dir);
                unlink(primary_path);
            }
        }
    }

    closedir(primary_dir_ptr);
}

void check_for_missing_directories(const char *primary_dir, const char *backup_dir)
{
    DIR *primary_dir_ptr = opendir(primary_dir);
    if (primary_dir_ptr == NULL)
    {
        perror("Could not open directory 1");
        return;
    }

    DIR *backup_dir_ptr = opendir(backup_dir);
    if (backup_dir_ptr == NULL)
    {
        perror("Could not open directory 2");
        return;
    }

    struct dirent *primary_dir_entry;
    while ((primary_dir_entry = readdir(primary_dir_ptr)) != NULL)
    {
        if (primary_dir_entry->d_type == DT_DIR && strcmp(primary_dir_entry->d_name, ".") != 0 && strcmp(primary_dir_entry->d_name, "..") != 0)
        {
            // Check if this directory exists in the second directory
            char primary_dir_path[1000];
            char backup_dir_path[1000];
            snprintf(primary_dir_path, 1000, "%s/%s", primary_dir, primary_dir_entry->d_name);
            snprintf(backup_dir_path, 1000, "%s/%s", backup_dir, primary_dir_entry->d_name);

            struct stat primary_dir_stat, backup_dir_stat;
            if (stat(primary_dir_path, &primary_dir_stat) == 0 && stat(backup_dir_path, &backup_dir_stat) == 0 && S_ISDIR(primary_dir_stat.st_mode) && S_ISDIR(backup_dir_stat.st_mode))
            {
                // The directory exists in both directories
            }
            else
            {
                // The directory exists in directory 1 but not in directory 2
                printf("Directory '%s' is missing in '%s'\n", primary_dir_path, backup_dir);
                rmdir(primary_dir_path);
            }
        }
    }

    closedir(primary_dir_ptr);
    closedir(backup_dir_ptr);
}

void remove_directory_or_file(const char *path)
{
    if (is_directory(path) == 1)
    {
        rmdir(path);
    }
    else
    {
        unlink(path);
    }
    return;
}

bool create_directory_with_parents_1(const char *dir_path, mode_t mode)
{
    char tmp[1000];
    char *p = NULL;
    size_t len;
    struct stat st = {
        0};

    snprintf(tmp, sizeof(tmp), "%s", dir_path);
    len = strlen(tmp);

    if (tmp[len - 1] == '/')
        tmp[len - 1] = 0;

    for (p = tmp + 1; *p; p++)
    {
        if (*p == '/')
        {
            *p = 0;
            if (stat(tmp, &st) != 0)
            {
                if (mkdir(tmp, mode) != 0)
                {
                    return false;
                }
            }
            *p = '/';
        }
    }

    if (stat(tmp, &st) != 0)
    {
        if (mkdir(tmp, mode) != 0)
        {
            return false;
        }
    }

    return true;
}

bool create_filepath_1(const char *file_path)
{
    struct stat st = {
        0};
    char *ext = strrchr(file_path, '.'); // Find the last occurrence of '.' in the file path

    // Extract the directory path from the file path
    char dir_path[1000];
    strcpy(dir_path, file_path);
    dirname(dir_path);

    if (ext != NULL && strcmp(ext, ".txt") == 0)
    { // Check if the file extension is '.txt'
        if (stat(file_path, &st) == 0)
        {
            return false;
        }

        if (create_directory_with_parents_1(dir_path, 0777))
        {
            FILE *fp = fopen(file_path, "w");
            if (fp == NULL)
            {
                return false;
            }
            fclose(fp);
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    { 
        if (stat(file_path, &st) == 0)
        {
            return false;
        }
        if (create_directory_with_parents_1(file_path, 0777))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
}

void mirror_filepath(Filepath *primary, Filepath *backup, enum Command command)
{
    switch (command)
    {
    case PUT:
        copy_file(primary->full_path, backup->full_path);
        break;
    case RM:
        remove_directory_or_file(primary->full_path);
        remove_directory_or_file(backup->full_path);
        break;
    case MD:
        create_filepath_1(backup->full_path);
        break;
    }
}