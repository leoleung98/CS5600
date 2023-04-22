/*
 * filepath.h / Header file for filepath functions
 *
 * Ethan Liu & Leo Liang/ CS5600 / Northeastern University
 * Spring 2023 / April 20, 2023
 *
 */

#include <stdbool.h>
#include <unistd.h>

#ifndef FILEPATH_H
#define FILEPATH_H

#define MAX_PATH_LEN 1000
#define MAX_FILE_LEN 1000

enum Command
// enum used to define different command use in server.c
{
    PUT,
    RM,
    MD
};

typedef struct
// data structures to manage the filepath for Primary USB and Back USB
{
    char root_path[MAX_PATH_LEN]; //root directory of the USB Ex. /Volumes/Primary 
    char directory_path[MAX_FILE_LEN]; //any path within the root EX. /Volumes/Primary/Folder1/Folder2/Findme.txt
    char full_path[MAX_FILE_LEN]; // The root and directory/file combined full path. Ex. /Volumes/PrimaryFolder1/Folder2/Findme.txt or /Volumes/PrimaryFolder3
    char *last_modified_time; // The last modified time of each USB drive
} Filepath;

void last_accessed_time(Filepath *driver, const char *driver_log, const char *Command); // Check for last access time of a root path
int check_directory_availability(Filepath *primary, Filepath *backup);                  // check if the directory is available
void mount_server_storage(Filepath *primary, Filepath *backup);                         // mount the usb driver root path to the program
void update_full_path(char *file_name, Filepath *path);                                 // update the full path of each usb driver for data access for primary driver
void update_backup_full_path(Filepath *primary, Filepath *backup);                      // update the full path of each usb driver for data access for back driver
void copy_file(const char *src_path, const char *dst_path);                             // use for mirror function, copies anything that's not directory
void copy_directory(const char *source_path, const char *destination_path);             // use for mirror function, copies any directory
int is_directory(const char *path);                                                     // check to see if the file path is an directory or not
void find_missing_files(const char *primary_dir, const char *backup_dir);               // find any missing file in usb
void check_for_missing_directories(const char *primary_dir, const char *backup_dir);    // check for any missing directories in ubs
bool create_filepath_1(const char *file_path);                                          // create a file path in directory
bool create_directory_with_parents_1(const char *dir_path, mode_t mode);                // create a file path with directory
void mirror_filepath(Filepath *primary, Filepath *backup, enum Command command);        // mirror a filepath from primary to backup dirve

#endif /* FILEPATH_H */