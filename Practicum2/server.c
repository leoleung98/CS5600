/*
 * server.c / server socket program to make responses to client
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
#include <pthread.h>
#include <sys/select.h>
#include <sys/time.h>
#include "filepath.h"

#define SELECT_TIMEOUT 500 // Timeout in milliseconds
#define BUFFER_SIZE 8196

int server_shutdown = 0;
pthread_mutex_t server_shutdown_mutex = PTHREAD_MUTEX_INITIALIZER;

int count_files_in_directory(const char * dir_path);

// Helper function to reduce code redundancy, it is used to send server's message to client
void send_server_msg(int socket_desc,
  const char * server_msg) {
  if (send(socket_desc, server_msg, strlen(server_msg), 0) < 0) {
    printf("[-]Can't return message back to client.\n");
  }
}

bool check_file(int socket_desc,
  const char * file_path, char * server_msg) {
  struct stat st = {
    0
  };
  if (stat(file_path, & st) == -1) {
    strcpy(server_msg, "Server file not existed.\n");
    send_server_msg(socket_desc, server_msg);
    printf("[-]Can't find file path.\n");
    return false;
  }
  strcpy(server_msg, "File found\n");
  send_server_msg(socket_desc, server_msg);
  return true;
}

bool send_file(int socket_desc,
  const char * file_path, char * server_msg) {

  if (!check_file(socket_desc, file_path, server_msg)) {
    return false;
  }
  // Open the file for reading:
  FILE * fp = fopen(file_path, "rb");

  if (fp == NULL) {
    printf("[-]Unable to open file for reading: %s\n", file_path);
    fclose(fp);
    return false;
  }
  usleep(100000);
  // Read and send the file data in chunks:
  char * buffer[BUFFER_SIZE];
  size_t nread;
  ssize_t nsent;
  while ((nread = fread(buffer, 1, BUFFER_SIZE, fp)) > 0) {
    nsent = send(socket_desc, buffer, nread, 0);
    if (nsent < 0) {
      printf("[-]Error sending file data\n");
      break;
    } else if (nsent == 0) {
      printf("[+]Connection closed by remote host\n");
      break;
    }
    usleep(50000);
  }

  // Close the file:
  fclose(fp);

  // Send end-of-file marker
  char eof_marker[] = "[EOF]";
  if (send(socket_desc, eof_marker, sizeof(eof_marker), 0) < 0) {
    printf("[-]Can't send EOF marker\n");
    return false;
  }

  // Check if the entire file was sent successfully:
  if (nread == 0) {
    return true;
  } else {
    return false;
  }
}

bool receive_file(int socket_desc,
  const char * file_path) {
  // Open the file for writing:
  FILE * fp = fopen(file_path, "wb");

  if (fp == NULL) {
    printf("[-]Unable to open file for writing: %s\n", file_path);
    return false;
  }

  // Receive and write the file data in chunks:
  char buffer[BUFFER_SIZE];
  ssize_t nread;

  // Receive file size
  long filesize;
  if (recv(socket_desc, & filesize, sizeof(filesize), 0) < 0) {
    printf("[-]Error receiving file size\n");
    return -1;
  }

  // Receive and write the file data in chunks:
  long bytes_received = 0;
  while (bytes_received < filesize) {
    nread = recv(socket_desc, buffer, sizeof(buffer), 0);
    if (nread < 0) {
      printf("[-]Error receiving file data\n");
      break;
    }
    bytes_received += nread;
    if (fwrite(buffer, 1, nread, fp) != nread) {
      printf("[-]Error writing file data\n");
      break;
    }
  }

  usleep(100000);
  // Close the file:
  fclose(fp);

  if (nread < 0) {
    printf("[-]Error receiving file data\n");
    return false;
  }

  return true;
}

// % fget RM command
bool remove_directory(const char * path) {
  DIR * d = opendir(path);

  if (d) {
    struct dirent * entry;
    int status = 0;
    bool has_error = false;

    while ((entry = readdir(d)) != NULL) {
      if (strcmp(entry -> d_name, ".") == 0 || strcmp(entry -> d_name, "..") == 0) {
        continue;
      }

      char subpath[BUFFER_SIZE];
      snprintf(subpath, sizeof(subpath), "%s/%s", path, entry -> d_name);

      struct stat st;
      stat(subpath, & st);

      if (S_ISDIR(st.st_mode)) {
        status = remove_directory(subpath); // Recursively call remove_directory for nested folders
        if (status != 0) {
          has_error = true;
        }
      } else {
        status = remove(subpath);
        if (status != 0) {
          has_error = true;
        }
      }
    }
    closedir(d);

    // Always attempt to delete the parent directory, regardless of errors encountered
    status = rmdir(path);
    if (status != 0) {
      has_error = true;
    }

    return !has_error;
  }
  return false;
}

int delete_file(int socket_desc,
  const char * file_path, char * server_msg) {
  struct stat st = {
    0
  };
  if (stat(file_path, & st) != 0) {
    strcpy(server_msg, "[-]File or directory not found for removal.\n");
    send_server_msg(socket_desc, server_msg);
    return 1;
  }

  if (S_ISDIR(st.st_mode)) {
    int file_count = count_files_in_directory(file_path);

    if (file_count > 0) {
      snprintf(server_msg, BUFFER_SIZE, "[-]Directory contains %d files. Are you sure you want to delete it? (yes/no): ", file_count);
      send_server_msg(socket_desc, server_msg);

      // Receive client's response
      char client_response[BUFFER_SIZE];
      recv(socket_desc, client_response, sizeof(client_response), 0);

      if (strncmp(client_response, "yes", 3) != 0) {
        strcpy(server_msg, "[+]Directory deletion canceled by the client.\n");
        send_server_msg(socket_desc, server_msg);
        return 2;
      }
    }
    remove_directory(file_path);
    rmdir(file_path);
    if (stat(file_path, & st) != 0) {
      strcpy(server_msg, "[+]Directory removed successfully.\n");
      send_server_msg(socket_desc, server_msg);
      return 0;
    } else {
      strcpy(server_msg, "[-]Error removing directory.\n");
      send_server_msg(socket_desc, server_msg);
      return 3;
    }
  } else if (S_ISREG(st.st_mode)) {
    if (remove(file_path) == 0) {
      strcpy(server_msg, "[+]File removed successfully.\n");
      send_server_msg(socket_desc, server_msg);
      return 0;
    } else {
      strcpy(server_msg, "[-]Error removing file.\n");
      send_server_msg(socket_desc, server_msg);
      return 3;
    }
  } else {
    strcpy(server_msg, "[-]Path is neither a file nor a directory.\n");
    send_server_msg(socket_desc, server_msg);
    return 1;
  }
}

int count_files_in_directory(const char * path) {
  DIR * d = opendir(path);
  int file_count = 0;

  if (d) {
    struct dirent * entry;

    while ((entry = readdir(d)) != NULL) {
      if (strcmp(entry -> d_name, ".") == 0 || strcmp(entry -> d_name, "..") == 0) {
        continue;
      }

      char subpath[BUFFER_SIZE];
      snprintf(subpath, sizeof(subpath), "%s/%s", path, entry -> d_name);

      struct stat st;
      stat(subpath, & st);

      if (S_ISDIR(st.st_mode)) {
        file_count += count_files_in_directory(subpath); // Recursively count files in subdirectories
      } else if (S_ISREG(st.st_mode) || S_ISLNK(st.st_mode)) {
        file_count++;
      }
    }
    closedir(d);
  }

  return file_count;
}

bool create_directory_with_parents(const char * dir_path, mode_t mode) {
  char tmp[BUFFER_SIZE];
  char * p = NULL;
  size_t len;
  struct stat st = {
    0
  };

  snprintf(tmp, sizeof(tmp), "%s", dir_path);
  len = strlen(tmp);

  if (tmp[len - 1] == '/')
    tmp[len - 1] = 0;

  for (p = tmp + 1;* p; p++) {
    if ( * p == '/') {
      * p = 0;
      if (stat(tmp, & st) != 0) {
        if (mkdir(tmp, mode) != 0) {
          return false;
        }
      }
      * p = '/';
    }
  }

  if (stat(tmp, & st) != 0) {
    if (mkdir(tmp, mode) != 0) {
      return false;
    }
  }

  return true;
}

bool create_filepath(int socket_desc,
  const char * file_path, char * server_msg) {
  struct stat st = {
    0
  };
  char * ext = strrchr(file_path, '.'); // Find the last occurrence of '.' in the file path

  // Extract the directory path from the file path
  char dir_path[BUFFER_SIZE];
  strcpy(dir_path, file_path);
  dirname(dir_path);

  if (ext != NULL && strcmp(ext, ".txt") == 0) { // Check if the file extension is '.txt'
    if (stat(file_path, & st) == 0) {
      strcpy(server_msg, "[-]File path already existed.\n");
      send_server_msg(socket_desc, server_msg);
      return false;
    }

    if (create_directory_with_parents(dir_path, 0777)) {
      FILE * fp = fopen(file_path, "w"); // Create the file for writing
      if (fp == NULL) {
        strcpy(server_msg, "[-]Failed to create file.\n");
        send_server_msg(socket_desc, server_msg);
        return false;
      }
      fclose(fp);
      strcpy(server_msg, "[+]File created successfully.\n");
      send_server_msg(socket_desc, server_msg);
      return true;
    } else {
      strcpy(server_msg, "[-]Failed to create directories.\n");
      send_server_msg(socket_desc, server_msg);
      return false;
    }
  } else { // Otherwise, create a directory
    if (stat(file_path, & st) == 0) {
      strcpy(server_msg, "[-]File path already existed.\n");
      send_server_msg(socket_desc, server_msg);
      return false;
    }
    if (create_directory_with_parents(file_path, 0777)) {
      strcpy(server_msg, "[+]Directory created successfully.\n");
      send_server_msg(socket_desc, server_msg);
      return true;
    } else {
      strcpy(server_msg, "[-]Failed to create directory.\n");
      send_server_msg(socket_desc, server_msg);
      return false;
    }
  }
}

// % fget INFO command
bool get_file_info(int socket_desc,
  const char * file_path, char * server_msg) {
  struct stat file_stat;
  if (stat(file_path, & file_stat) == -1) {
    strcpy(server_msg, "[-]Error getting file info.\n");
    send_server_msg(socket_desc, server_msg);
    return false;
  }
  char file_info[BUFFER_SIZE];
  char * filename = basename((char * ) file_path);
  snprintf(file_info, sizeof(file_info), "File information for '%s':\nSize: %ld bytes\nPermissions: %o\nOwner: %d\nGroup: %d\nLast access: %sLast modification: %s",
    filename, (long) file_stat.st_size, file_stat.st_mode & 0777, file_stat.st_uid, file_stat.st_gid, ctime( & file_stat.st_atime), ctime( & file_stat.st_mtime));
  strcpy(server_msg, file_info);
  send_server_msg(socket_desc, server_msg);
  return true;
}

typedef struct {
  int sock;
  struct sockaddr_in addr;
  int listening_socket;
}
client_info;
Filepath primary, backup;

void * client_handler(void * client_info_ptr) {

  // Extract client information from the pointer
  client_info * info = (client_info * ) client_info_ptr;
  int client_sock = info -> sock;
  struct sockaddr_in client_addr = info -> addr;
  char client_ip[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, & (client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
  int client_port = ntohs(client_addr.sin_port);

  // Log the client's IP address and port number
  printf("[+]Client connected at IP: %s and port: %d\n", client_ip, client_port);

  char server_message[BUFFER_SIZE], client_message[BUFFER_SIZE];

  // Clean buffers:
  memset(server_message, '\0', sizeof(server_message));
  memset(client_message, '\0', sizeof(client_message));

  while (1) {
    printf("\n");
    // Receive client's message:
    if (recv(client_sock, client_message, sizeof(client_message), 0) < 0) {
      printf("[-]Couldn't receive\n");
      break;
    }

    if (strlen(client_message) == 0) {
      printf("[-]Client %d disconnected\n", client_port);
      break;
    }

    printf("[+]Msg from client %d: %s\n", client_port, client_message);

    // Process the client's message here
    char * token = strtok(client_message, " ");
    if (token != NULL) {
      if (strcmp(token, "fget") == 0) {
        printf("[+]Received fget command from client\n");

        // Check if the second word is "PUT", "GET", "RM", "MD", "INFO":
        token = strtok(NULL, " ");
        if (token != NULL) {
          if (strcmp(token, "PUT") == 0) {
            int start_function = check_directory_availability( & primary, & backup);
            if (start_function == -1) {
              // Respond to client:
              strcpy(server_message, "\n\nPrimary and Backup storages are OFFLINE\n");
              printf("[SYSTEM] Primary storage [OFF] Backup storage [OFF]\n");
              if (send(client_sock, server_message, strlen(server_message), 0) < 0) {
                printf("[-]Can't send\n");
              }
            } else {
              strcpy(server_message, "\n\nMessage received\n");
              if (send(client_sock, server_message, strlen(server_message), 0) < 0) {
                printf("[-]Can't send\n");
              }
              printf("[+]Receiving file from client\n");

              // Check if the third and fourth words are file paths:
              char file_path_server[BUFFER_SIZE], file_path_client[BUFFER_SIZE];
              token = strtok(NULL, " ");
              if (token != NULL) {
                snprintf(file_path_server, BUFFER_SIZE, "%s", token);

                token = strtok(NULL, " ");
                if (token != NULL) {
                  snprintf(file_path_client, BUFFER_SIZE, "%s", token);

                  // Send confirmation message to client:
                  strcpy(server_message, "Message received\n");
                  if (send(client_sock, server_message, strlen(server_message), 0) < 0) {
                    printf("[-]Can't send\n");
                  }
                  // Receive the file from the client:
                  recv(client_sock, client_message, sizeof(client_message), 0);
                  if (strncmp(client_message, "Client can't find file", strlen("Client can't find file")) == 0) {
                    printf("[-]Client can't find file.\n");
                  } else if (strncmp(client_message, "Client find file", strlen("Client find file")) == 0) {
                    if (start_function == 0) {
                      printf("[SYSTEM] Primary storage [ON] Backup storage [ON]\n");
                      update_full_path(file_path_server, & primary);
                      update_backup_full_path( & primary, & backup);
                      if (receive_file(client_sock, primary.full_path)) {
                        mirror_filepath( & primary, & backup, PUT);
                        printf("[+]File recieved successfully.\n");
                        last_accessed_time( & primary, "primary_time_log.txt", "PUT");
                        last_accessed_time( & backup, "backup_time_log.txt", "PUT");
                      } else {
                        printf("[-]Error recieving file.\n");
                      }
                    } else if (start_function == 1) {
                      printf("[SYSTEM] Primary storage [ON] Backup storage [OFF]\n");
                      update_full_path(file_path_server, & primary);
                      update_backup_full_path( & primary, & backup);
                      if (receive_file(client_sock, primary.full_path)) {
                        // mirror_filepath(&primary, &backup, PUT);
                        printf("[+]File recieved successfully.\n");
                        last_accessed_time( & primary, "primary_time_log.txt", "PUT");
                      } else {
                        printf("[-]Error recieving file.\n");
                      }
                    } else if (start_function == 2) {
                      printf("[SYSTEM] Primary storage [OFF] Backup storage [ON]\n"); // update_full_path(file_path_server, &primary);
                      update_backup_full_path( & primary, & backup);
                      if (receive_file(client_sock, backup.full_path)) {
                        // mirror_filepath(&primary, &backup, PUT);
                        printf("[+]File recieved successfully.\n");
                        last_accessed_time( & backup, "backup_time_log.txt", "PUT");
                      } else {
                        printf("[-]Error recieving file.\n");
                      }
                    }
                  }
                  memset(primary.full_path, '\0', sizeof(primary.full_path));
                  memset(backup.full_path, '\0', sizeof(backup.full_path));
                }
              }
            }
          } else if (strcmp(token, "GET") == 0) {
            int start_function = check_directory_availability( & primary, & backup);
            if (start_function == -1) {
              // Respond to client:
              strcpy(server_message, "\n\nPrimary and Backup storages are OFFLINE!!!\n");
              printf("[SYSTEM] Primary storage [-] Backup storage [-]\n");

              if (send(client_sock, server_message, strlen(server_message), 0) < 0) {
                printf("[-]Can't send\n");
              }
            } else {
              // Handle GET command
              printf("[+]Sending file to client\n");

              // Check if the third and fourth words are file paths:
              char file_path_server[BUFFER_SIZE], file_path_client[BUFFER_SIZE];
              token = strtok(NULL, " ");
              if (token != NULL) {
                snprintf(file_path_server, BUFFER_SIZE, "%s", token);
                token = strtok(NULL, " ");
                if (token != NULL) {
                  snprintf(file_path_client, BUFFER_SIZE, "%s", token);

                  if (start_function == 0 || start_function == 1) {
                    printf("[SYSTEM] Primary storage [ON]\n");
                    update_full_path(file_path_server, & primary);
                    // Send the file to the client:
                    if (send_file(client_sock, primary.full_path, server_message)) {
                      printf("[+]File/Message send successfully.\n");
                    } else {
                      printf("[-]Error sending file/message.\n");
                    }
                  } else if (start_function == 2) {
                    printf("[SYSTEM] Primary storage [OFF] Backup storage [ON]\n");
                    update_full_path(file_path_server, & primary);
                    update_backup_full_path( & primary, & backup);
                    // Send the file to the client:
                    if (send_file(client_sock, backup.full_path, server_message)) {
                      printf("[+]File/Message send successfully.\n");
                    } else {
                      printf("[-]Error sending file/message.\n");
                    }
                  }
                  memset(primary.full_path, '\0', sizeof(primary.full_path));
                  memset(backup.full_path, '\0', sizeof(backup.full_path));
                }
              }
            }
          } else if (strcmp(token, "MD") == 0) {
            int start_function = check_directory_availability( & primary, & backup);
            if (start_function == -1) {
              // Respond to client:
              strcpy(server_message, "\n\nPrimary and Backup storages are OFFLINE!!!\n");
              printf("[SYSTEM] Primary storage [-] Backup storage [-]\n");

              if (send(client_sock, server_message, strlen(server_message), 0) < 0) {
                printf("[-]Can't send\n");
              }
            } else {
              // Handle MD command
              printf("[+]Creating file path to server.\n");

              char file_path_server[BUFFER_SIZE];
              token = strtok(NULL, " ");
              if (token != NULL) {
                snprintf(file_path_server, BUFFER_SIZE, "%s", token);

                if (start_function == 0) {
                  printf("[SYSTEM] Primary storage [ON]\n");
                  update_full_path(file_path_server, & primary);
                  update_backup_full_path( & primary, & backup);

                  if (create_filepath(client_sock, primary.full_path, server_message)) {
                    mirror_filepath( & primary, & backup, MD);
                    printf("[+]File path created: %s\n", file_path_server);
                    last_accessed_time( & primary, "primary_time_log.txt", "MD");
                    last_accessed_time( & backup, "backup_time_log.txt", "MD");
                  } else {
                    printf("[-]File path already existed.\n");
                  }
                } else if (start_function == 1) {
                  printf("[SYSTEM] Primary storage [ON Bakcup storage [OFF]\n");
                  update_full_path(file_path_server, & primary);
                  update_backup_full_path( & primary, & backup);

                  if (create_filepath(client_sock, primary.full_path, server_message)) {
                    printf("[+]File path created: %s\n", file_path_server);
                    last_accessed_time( & primary, "primary_time_log.txt", "MD");
                  } else {
                    printf("[-]File path already existed.\n");
                  }
                } else if (start_function == 2) {
                  printf("[SYSTEM] Primary storage [OFF] Backup storage [ON]\n");
                  update_full_path(file_path_server, & primary);
                  update_backup_full_path( & primary, & backup);

                  if (create_filepath(client_sock, backup.full_path, server_message)) {
                    last_accessed_time( & backup, "backup_time_log.txt", "MD");
                    printf("[+]File path created: %s\n", file_path_server);
                  } else {
                    printf("[-]File path already existed.\n");
                  }
                }
                memset(primary.full_path, '\0', sizeof(primary.full_path));
                memset(backup.full_path, '\0', sizeof(backup.full_path));
              }
            }
          } else if (strcmp(token, "RM") == 0) {
            int start_function = check_directory_availability( & primary, & backup);
            if (start_function == -1) {
              // Respond to client:
              strcpy(server_message, "\n\nPrimary and Backup storages are OFFLINE!!!\n");
              printf("[SYSTEM] Primary storage [-] Backup storage [-]\n");

              if (send(client_sock, server_message, strlen(server_message), 0) < 0) {
                printf("[-]Can't send\n");
              }
            } else {
              // Handle RM command
              printf("[+]File is being removed from server.\n");

              char file_path_server[BUFFER_SIZE];
              token = strtok(NULL, " ");
              if (token != NULL) {
                snprintf(file_path_server, BUFFER_SIZE, "%s", token);

                if (start_function == 0) {
                  printf("[SYSTEM] Primary storage [ON] Backup storage [ON]\n");

                  update_full_path(file_path_server, & primary);
                  update_backup_full_path( & primary, & backup);
                  int result = delete_file(client_sock, primary.full_path, server_message);
                  if (result == 0) {
                    mirror_filepath( & primary, & backup, RM);
                    last_accessed_time( & primary, "primary_time_log.txt", "RM");
                    last_accessed_time( & backup, "backup_time_log.txt", "RM");
                    printf("[+]File removed: %s\n", primary.full_path);
                  } else if (result == 1) {
                    printf("[-]No such file found on server.\n");
                  }
                } else if (start_function == 1) {
                  printf("[SYSTEM] Primary storage [ON] Backup storage [OFF]\n");

                  update_full_path(file_path_server, & primary);
                  update_backup_full_path( & primary, & backup);

                  int result = delete_file(client_sock, primary.full_path, server_message);
                  if (result == 0) {
                    // mirror_filepath(&primary, &backup, RM);
                    printf("[+]File removed: %s\n", primary.full_path);
                    last_accessed_time( & primary, "primary_time_log.txt", "RM");
                  } else if (result == 1) {
                    printf("[-]No such file found on server.\n");
                  }
                } else if (start_function == 2) {
                  printf("[SYSTEM] Primary storage [OFF] Backup storage [ON]\n");

                  update_full_path(file_path_server, & primary);
                  update_backup_full_path( & primary, & backup);

                  int result = delete_file(client_sock, backup.full_path, server_message);
                  if (result == 0) {
                    // mirror_filepath(&primary, &backup, RM);
                    printf("[+]File removed: %s\n", backup.full_path);
                    last_accessed_time( & backup, "backup_time_log.txt", "RM");
                  } else if (result == 1) {
                    printf("[-]No such file found on server.\n");
                  }
                }
                memset(primary.full_path, '\0', sizeof(primary.full_path));
                memset(backup.full_path, '\0', sizeof(backup.full_path));
              }
            }
          } else if (strcmp(token, "INFO") == 0) {
            int start_function = check_directory_availability( & primary, & backup);
            if (start_function == -1) {
              // Respond to client:
              strcpy(server_message, "\n\nPrimary and Backup storages are OFFLINE!!!\n");
              printf("[SYSTEM] Primary storage [-] Backup storage [-]\n");

              if (send(client_sock, server_message, strlen(server_message), 0) < 0) {
                printf("[-]Can't send\n");
              }
            } else {
              // Handle INFO command
              printf("[+]Getting file information.\n");

              char file_path_server[BUFFER_SIZE];
              token = strtok(NULL, " ");
              if (token != NULL) {
                snprintf(file_path_server, BUFFER_SIZE, "%s", token);

                if (start_function == 0 || start_function == 1) {
                  printf("[SYSTEM] Primary storage [ON]\n");

                  update_full_path(file_path_server, & primary);
                  update_backup_full_path( & primary, & backup);

                  if (get_file_info(client_sock, primary.full_path, server_message)) {
                    printf("[+]File information sent to client\n");
                  } else {
                    printf("[-]No such file found on server.\n");
                  }
                } else if (start_function == 2) {
                  printf("[SYSTEM] Primary storage [OFF] Backup storage [ON]\n");

                  update_full_path(file_path_server, & primary);
                  update_backup_full_path( & primary, & backup);
                  if (get_file_info(client_sock, backup.full_path, server_message)) {
                    printf("[+]File information sent to client\n");
                  } else {
                    printf("[-]No such file found on server.\n");
                  }
                }
                memset(primary.full_path, '\0', sizeof(primary.full_path));
                memset(backup.full_path, '\0', sizeof(backup.full_path));
              }
            }
          }
        }
      } else {
        // Respond to client:
        strcpy(server_message, "Message received.");

        if (send(client_sock, server_message, strlen(server_message), 0) < 0) {
          printf("[-]Can't send\n");
          break;
        }
      }
    }

    // If the client sends "QUIT", close the connection
    if (strcmp(client_message, "QUIT") == 0) {
      printf("[+]Client requested to close the connection\n");
      break;
    }

    // If the client sends "SHUTDOWN", turn off the server
    if (strcmp(client_message, "SHUTDOWN") == 0) {
      printf("[+]Client requested to shutdown the server\n");
      pthread_mutex_lock( & server_shutdown_mutex);
      server_shutdown = 1;
      pthread_mutex_unlock( & server_shutdown_mutex);
      // Close the listening socket
      close(info -> listening_socket);
      printf("[+]Server closing\n");
      break;
    }

    // Clean buffers for the next iteration:
    memset(server_message, '\0', sizeof(server_message));
    memset(client_message, '\0', sizeof(client_message));
  }
  // Close the client socket and exit the thread
  close(client_sock);
  pthread_exit(NULL);
}

int main(int argc, char * argv[]) {
  // Initialize the mutex
  pthread_mutex_init( & server_shutdown_mutex, NULL);
  const char * host;
  int port = 2000;
  if (argc == 2) {
    port = atoi(argv[1]);
  } else if (argc == 3) {
    host = argv[1];
    port = atoi(argv[2]);
  }

  int socket_desc;
  struct sockaddr_in server_addr;
  pthread_t thread_id;

  // Create socket:
  socket_desc = socket(AF_INET, SOCK_STREAM, 0);

  if (socket_desc < 0) {
    printf("[-]Error while creating socket\n");
    return -1;
  }
  printf("[+]Socket created successfully\n");
  // Set port and IP:
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  server_addr.sin_addr.s_addr = (argc == 3) ? inet_addr(host) : INADDR_ANY;

  // Bind to the set port and IP:
  if (bind(socket_desc, (struct sockaddr * ) & server_addr, sizeof(server_addr)) < 0) {
    printf("[-]Couldn't bind to the port\n");
    return -1;
  }
  printf("[+]Done with binding\n");
  mount_server_storage( & primary, & backup);
  // Listen for clients:
  if (listen(socket_desc, 1) < 0) {
    printf("[-]Error while listening\n");
    return -1;
  }
  printf("\n[+]Listening for incoming connections.....\n");

  // Accept incoming connections and create a new thread for each client:
  while (!server_shutdown) {
    pthread_mutex_lock( & server_shutdown_mutex);
    if (server_shutdown) {
      pthread_mutex_unlock( & server_shutdown_mutex);
      printf("[----------]shutdown successfully\n");
      break;
    }
    pthread_mutex_unlock( & server_shutdown_mutex);

    fd_set read_fds;
    struct timeval timeout;

    FD_ZERO( & read_fds);
    FD_SET(socket_desc, & read_fds);

    timeout.tv_sec = 0;
    timeout.tv_usec = SELECT_TIMEOUT * 1000;

    int activity = select(socket_desc + 1, & read_fds, NULL, NULL, & timeout);

    if (activity < 0) {
      printf("[-]Error in select\n");
      break;
    } else if (activity == 0) {
      // Timeout occurred, check server_shutdown again
      continue;
    }

    int client_sock;
    socklen_t client_size;
    struct sockaddr_in client_addr;

    client_size = sizeof(client_addr);
    client_sock = accept(socket_desc, (struct sockaddr * ) & client_addr, & client_size);

    if (client_sock < 0) {
      printf("[-]Can't accept\n");
      return -1;
    }

    // Create a client_info struct and fill it with relevant data
    client_info * info = (client_info * ) malloc(sizeof(client_info));
    info -> sock = client_sock;
    info -> addr = client_addr;
    info -> listening_socket = socket_desc;

    // Create a new thread for the client connection:
    if (pthread_create( & thread_id, NULL, client_handler, (void * ) info) < 0) {
      printf("[-]Error while creating thread\n");
      return -1;
    }

    // Optionally, detach the thread so that it can clean up its resources upon completion:
    pthread_detach(thread_id);
  }
  printf("server detatch1");
  // Destroy the mutex after the while loop:
  pthread_mutex_destroy( & server_shutdown_mutex);
  printf("server detatch2");
  // Close the server socket (although this line will not be reached in this example):
  close(socket_desc);
  printf("server detatch3");
  return 0;
}