/*
 * client.c / client socket program to interact with server
 *
 * Ethan Liu & Leo Liang/ CS5600 / Northeastern University
 * Spring 2023 / April 20, 2023
 *
 */
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>

#define BUFFER_SIZE 8196

bool send_file(int socket_desc,
  const char * file_path) {
  // Open the file for reading:
  FILE * fp = fopen(file_path, "rb");
  if (fp == NULL) {
    printf("[-]Unable to open file for reading: %s\n", file_path);
    char * cant_find = "Client can't find file";
    if (send(socket_desc, cant_find, strlen(cant_find), 0) < 0) {
      printf("[-]Unable to send message\n");
      return -1;
    }
    return false;
  }

  // Read and send the file data in chunks:
  char buffer[BUFFER_SIZE];
  size_t nread;
  int i = 0;

  char * cant_find = "Client find file";
  if (send(socket_desc, cant_find, strlen(cant_find), 0) < 0) {
    printf("[-]Unable to send message\n");
    return -1;
  }

  // Send file size
  fseek(fp, 0, SEEK_END);
  long filesize = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  if (send(socket_desc, & filesize, sizeof(filesize), 0) < 0) {
    printf("[-]Error sending file size\n");
    fclose(fp);
    return false;
  }

  while ((nread = fread(buffer, 1, BUFFER_SIZE, fp)) > 0) {
    if (send(socket_desc, buffer, nread, 0) < 0) {
      printf("[-]Error sending file data\n");
      break;
    }
    usleep(50000);
  }
  // Close the file:
  fclose(fp);
  return true;
}

bool receive_file(int socket_desc,
  const char * file_path) {
  // Open the file for writing:
  //char buffer[BUFFER_SIZE];
  //ssize_t nread = recv(socket_desc, buffer, BUFFER_SIZE, 0);

  FILE * fp = fopen(file_path, "wb");
  if (fp == NULL) {
    printf("[-]Unable to open file for writing: %s\n", file_path);
    return false;
  }

  // Receive and write the file data in chunks:
  char buffer[BUFFER_SIZE];
  ssize_t nread;
  size_t total_received = 0;
  bool eof_received = false;
  while (!eof_received && (nread = recv(socket_desc, buffer, sizeof(buffer), 0)) > 0) {
    // Check for end-of-file marker
    if (strncmp(buffer, "[EOF]", 5) == 0) {
      eof_received = 1;
      break;
    }
    //printf("---%d---", (int*) nread);
    if (fwrite(buffer, 1, nread, fp) != nread) {
      printf("[-]Error writing file data\n");
      break;
    }
    total_received += nread;
    usleep(50000);
  }

  printf("[+]Total bytes received: %zu\n", total_received);
  // Close the file:
  fclose(fp);
  return true;
}

int main(int argc, char * argv[]) {
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
  char server_message[BUFFER_SIZE], client_message[BUFFER_SIZE];

  // Clean buffers:
  memset(server_message, '\0', sizeof(server_message));
  memset(client_message, '\0', sizeof(client_message));

  // Create socket:
  socket_desc = socket(AF_INET, SOCK_STREAM, 0);

  if (socket_desc < 0) {
    printf("[+]Unable to create socket\n");
    return -1;
  }

  int enable_keepalive = 1;
  if (setsockopt(socket_desc, SOL_SOCKET, SO_KEEPALIVE, &enable_keepalive, sizeof(enable_keepalive)) < 0) {
    perror("[-]Error setting SO_KEEPALIVE");
    return -1;
  }

  printf("[+]Socket created successfully\n");

  // Set port and IP the same as server-side:
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  server_addr.sin_addr.s_addr = (argc == 3) ? inet_addr(host) : INADDR_ANY;

  // Send connection request to server:
  if (connect(socket_desc, (struct sockaddr * ) & server_addr, sizeof(server_addr)) < 0) {
    printf("[-]]Unable to connect\n");
    return -1;
  }
  printf("[+]Connected with server successfully\n");
  while (1) {
    // Get user input
    printf("Enter command: ");
    fgets(client_message, sizeof(client_message), stdin);
    client_message[strcspn(client_message, "\n")] = 0; // Remove newline character

    char *client_message_copy = (char *)malloc(2.5 * BUFFER_SIZE * sizeof(char));
    strcpy(client_message_copy, client_message);

    if (client_message[0] != '\0') {
      // Tokenize the message and check if the first word is "fget":
      char * token = strtok(client_message_copy, " ");
      if (strcmp(token, "fget") == 0) {
        printf("[+]Sending fget command to server\n");

        // Check if the second word is "PUT" or "GET":
        token = strtok(NULL, " ");
        if (token != NULL) {
          if (strcmp(token, "PUT") == 0) {
            printf("[+]Uploading file to server\n");

            // Check if the third word is a file path:
            char file_path_server[BUFFER_SIZE], file_path_client[BUFFER_SIZE];
            token = strtok(NULL, " ");
            if (token != NULL) {
              snprintf(file_path_server, BUFFER_SIZE, "%s", token);
              token = strtok(NULL, " ");

              // Check if the fourth word is a file path:
              if (token != NULL) {
                  snprintf(file_path_client, BUFFER_SIZE, "%s", token);
              } 
              else {
                // If the fourth word is missing, use the server file path as the client file path
                snprintf(file_path_client, BUFFER_SIZE, "%s", file_path_server);
                // And use only the local file name for the server file path
                char *local_file_name = strrchr(file_path_server, '/');
                if (local_file_name != NULL) {
                  local_file_name++; // move past the '/'
                  snprintf(file_path_server, BUFFER_SIZE, "./%s", local_file_name);
                } else {
                  printf("[-]Invalid server file path\n");
                  return -1;
                }
              }

              // Send the "PUT" command and file paths to the server:
              snprintf(client_message_copy, 2.5 * BUFFER_SIZE, "fget PUT %s %s", file_path_server, file_path_client);
              if (send(socket_desc, client_message_copy, strlen(client_message_copy), 0) < 0) {
                printf("[-]Unable to send message\n");
                return -1;
              }
              // Receive confirmation message from server:
              if (recv(socket_desc, server_message, sizeof(server_message), 0) < 0) {
                printf("[-]Error while receiving server's msg\n");
                return -1;
              }
              printf("[+]Server's response: %s\n", server_message);

              // Send the file to the server:
              if (send_file(socket_desc, file_path_client)) {
                printf("[+]File sent successfully.\n");
              } else {
                printf("[-]Error sending file.\n");
              }
            }
          }
          else if (strcmp(token, "GET") == 0) {
            printf("[+]Downloading file from server\n");

            // Check if the third word is a file path:
            char file_path_server[BUFFER_SIZE], file_path_client[BUFFER_SIZE];
            token = strtok(NULL, " ");
            if (token != NULL) {
              snprintf(file_path_server, BUFFER_SIZE, "%s", token);
              token = strtok(NULL, " ");

              // Check if the fourth word is a file path:
              if (token != NULL) {
                snprintf(file_path_client, BUFFER_SIZE, "%s", token);
              } else {
                // If the fourth word is missing, use the remote file name as the local file name
                char *remote_file_name = strrchr(file_path_server, '/');
                if (remote_file_name != NULL) {
                  remote_file_name++; // move past the '/'
                  snprintf(file_path_client, BUFFER_SIZE, "./%s", remote_file_name);
                } else {
                  printf("[-]Invalid remote file path\n");
                  return -1;
                }
              }

              // Send the "GET" command and file paths to the server:
              snprintf(client_message_copy, 2.5 * BUFFER_SIZE, "fget GET %s %s", file_path_server, file_path_client);
              int get_file = send(socket_desc, client_message_copy, strlen(client_message_copy), 0);
              recv(socket_desc, server_message, sizeof(server_message), 0);
              if (get_file < 0) {
                printf("[-]Unable to send message\n");
                return -1;
              }
              // Receive the file from the server:
              if (strcmp(server_message, "File found\n") == 0) {
                printf("[+]Server's response: %s\n", server_message);
                if (receive_file(socket_desc, file_path_client)) {
                    printf("[+]File recieved successfully.\n");
                } else {
                    printf("[-]Error recieving file.\n");
                }
              } else {
                printf("[+]Server's response: %s\n", server_message);
              }
            }
          }
          else if (strcmp(token, "MD") == 0) {
            printf("[+]File path creation request sent to server.\n");

            char file_path_server[BUFFER_SIZE];
            token = strtok(NULL, " ");
            if (token != NULL) {
              snprintf(file_path_server, BUFFER_SIZE, "%s", token);
              snprintf(client_message_copy, 2.5 * BUFFER_SIZE, "fget MD %s", file_path_server);
              if (send(socket_desc, client_message_copy, strlen(client_message_copy), 0) < 0) {
                printf("[-]Unbale to send message.\n");
                return -1;
              }

              if (recv(socket_desc, server_message, sizeof(server_message), 0) < 0) {
                printf("[-]Error while receiving server's msg\n");
                return -1;
              }

              printf("[+]Server's response: %s\n", server_message);
            }
          } else if (strcmp(token, "RM") == 0) {
            printf("[+]File removal request sent to server.\n");

            char file_path_server[BUFFER_SIZE];
            token = strtok(NULL, " ");
            if (token != NULL) {
              snprintf(file_path_server, BUFFER_SIZE, "%s", token);
              snprintf(client_message_copy, 2.5 * BUFFER_SIZE, "fget RM %s", file_path_server);
              if (send(socket_desc, client_message_copy, strlen(client_message_copy), 0) < 0) {
                printf("[-]Unable to send message.\n");
                return -1;
              }

              if (recv(socket_desc, server_message, sizeof(server_message), 0) < 0) {
                printf("[-]Error while receiving server's msg\n");
                return -1;
              }

              if (strncmp(server_message, "[-]Directory contains ", 22) == 0) {
                printf("%s", server_message);

                char client_response[BUFFER_SIZE];
                fgets(client_response, sizeof(client_response), stdin);
                send(socket_desc, client_response, strlen(client_response), 0);
                memset(server_message, 0, sizeof(server_message)); // Reset server_message
                recv(socket_desc, server_message, sizeof(server_message), 0);
                printf("%s", server_message);
              } else {
                printf("%s", server_message);
              }
            }
          } else if (strcmp(token, "INFO") == 0) {
            printf("[+]File information requested.\n");

            char file_path_server[BUFFER_SIZE];
            token = strtok(NULL, " ");
            if (token != NULL) {
              snprintf(file_path_server, BUFFER_SIZE, "%s", token);
              snprintf(client_message_copy, 2.5 * BUFFER_SIZE, "fget INFO %s", file_path_server);
              if (send(socket_desc, client_message_copy, strlen(client_message_copy), 0) < 0) {
                printf("[-]Unbale to send message.\n");
                return -1;
              }

              if (recv(socket_desc, server_message, sizeof(server_message), 0) < 0) {
                printf("[-]Error while receiving server's msg\n");
                return -1;
              }

              printf("[+]Server's response: \n%s\n", server_message);
            }
          }
        }
      } else {
        // Send the message to server:
        if (send(socket_desc, client_message, strlen(client_message), 0) < 0) {
          printf("[-]Unable to send message\n");
          return -1;
        }

        // Receive the server's response:
        if (recv(socket_desc, server_message, sizeof(server_message), 0) < 0) {
          printf("[-]Error while receiving server's msg\n");
          return -1;
        }

        printf("[+]Server's response: %s\n", server_message);
      }
    }
    // If the user entered "QUIT", close the connection to the server
    if (strcmp(client_message, "QUIT") == 0) {
      printf("[+]Closing connection...\n");
      close(socket_desc);
      break;
    }

    // If the user entered "SHUTDOWN", close the connection and exit
    if (strcmp(client_message, "SHUTDOWN") == 0) {
      printf("[+]Closing connection and shutting down the server...\n");
      close(socket_desc);
      break;
    }

    // Check the connection status
    int error = 0;
    socklen_t len = sizeof(error);
    int connection_status = getsockopt(socket_desc, SOL_SOCKET, SO_ERROR, &error, &len);
    if (connection_status != 0 || error != 0) {
      printf("[-]Connection to the server lost\n");
      close(socket_desc);
      break;
    }

    // Clear buffers
    memset(server_message, '\0', sizeof(server_message));
    memset(client_message, '\0', sizeof(client_message));
    free(client_message_copy);
  }
  return 0;
}