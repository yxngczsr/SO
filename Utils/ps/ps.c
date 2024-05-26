#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

void list_processes() {
    DIR *dir;
    struct dirent *entry;

    if ((dir = opendir("/proc")) == NULL) {
        perror("Failed to open /proc directory");
        exit(1);
    }

    while ((entry = readdir(dir)) != NULL) {
        // Check if the directory entry is a PID directory
        int pid;
        if ((pid = atoi(entry->d_name)) == 0) continue; // Skip non-numeric entries

        char path[256];
        sprintf(path, "/proc/%s/comm", entry->d_name);

        FILE *fp = fopen(path, "r");
        if (fp != NULL) {
            char process_name[256];
            if (fgets(process_name, sizeof(process_name), fp) != NULL) {
                process_name[strcspn(process_name, "\n")] = 0; // Remove newline character
                printf("PID: %5d - Process Name: %s\n", pid, process_name);
            }
            fclose(fp);
        }
    }

    closedir(dir);
}

int main(int argc, char *argv[]) {
    printf("Listing active processes:\n");
    list_processes();
    return 0;
}

