#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <pwd.h>
#include <ctype.h>

#define MAX_PATH 1024
#define MAX_PROC 1024

typedef struct {
    char pid[16];
    char user[32];
    double cpu_usage;
    double mem_usage;
    unsigned long io_read;
    unsigned long io_write;
    char command[256];
} ProcessInfo;

void get_username(uid_t uid, char *username) {
    struct passwd *pw = getpwuid(uid);
    if (pw) {
        strcpy(username, pw->pw_name);
    } else {
        sprintf(username, "%d", uid);
    }
}

void read_stat_file(const char *pid, ProcessInfo *proc) {
    char path[MAX_PATH];
    snprintf(path, sizeof(path), "/proc/%s/stat", pid);
    FILE *file = fopen(path, "r");
    if (!file) return;

    unsigned long utime, stime;
    fscanf(file, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu %lu",
           &utime, &stime);
    fclose(file);

    proc->cpu_usage = (double)(utime + stime) / sysconf(_SC_CLK_TCK);
}

void read_status_file(const char *pid, ProcessInfo *proc) {
    char path[MAX_PATH];
    snprintf(path, sizeof(path), "/proc/%s/status", pid);
    FILE *file = fopen(path, "r");
    if (!file) return;

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "Uid:", 4) == 0) {
            uid_t uid;
            sscanf(line, "Uid:\t%u", &uid);
            get_username(uid, proc->user);
        } else if (strncmp(line, "VmRSS:", 6) == 0) {
            unsigned long mem;
            sscanf(line, "VmRSS:\t%lu", &mem);
            proc->mem_usage = mem / 1024.0; // Convert to MB
        }
    }
    fclose(file);
}

void read_cmdline_file(const char *pid, ProcessInfo *proc) {
    char path[MAX_PATH];
    snprintf(path, sizeof(path), "/proc/%s/cmdline", pid);
    FILE *file = fopen(path, "r");
    if (!file) return;

    fgets(proc->command, sizeof(proc->command), file);
    fclose(file);
}

void read_io_file(const char *pid, ProcessInfo *proc) {
    char path[MAX_PATH];
    snprintf(path, sizeof(path), "/proc/%s/io", pid);
    FILE *file = fopen(path, "r");
    if (!file) return;

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "read_bytes:", 11) == 0) {
            sscanf(line, "read_bytes: %lu", &proc->io_read);
        } else if (strncmp(line, "write_bytes:", 12) == 0) {
            sscanf(line, "write_bytes: %lu", &proc->io_write);
        }
    }
    fclose(file);
}

void get_process_info(ProcessInfo *proc, const char *pid) {
    strcpy(proc->pid, pid);
    read_stat_file(pid, proc);
    read_status_file(pid, proc);
    read_cmdline_file(pid, proc);
    read_io_file(pid, proc);
}

void print_process_info(ProcessInfo *proc) {
    printf("%-8s %-8s %5.1f %5.1f %10lu %10lu %-20s\n",
           proc->pid, proc->user, proc->cpu_usage, proc->mem_usage,
           proc->io_read, proc->io_write, proc->command);
}

void show_powertop() {
    system("powertop");
}

void show_iotop() {
    system("sudo iotop");
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        if (strcmp(argv[1], "powertop") == 0) {
            show_powertop();
            return 0;
        } else if (strcmp(argv[1], "iotop") == 0) {
            show_iotop();
            return 0;
        }
    }

    DIR *dir = opendir("/proc");
    if (!dir) {
        perror("Failed to open /proc");
        return 1;
    }

    printf("%-8s %-8s %5s %5s %10s %10s %-20s\n",
           "PID", "USER", "CPU%", "MEM%", "IO-READ", "IO-WRITE", "COMMAND");

    struct dirent *entry;
    ProcessInfo proc[MAX_PROC];
    int count = 0;

    while ((entry = readdir(dir)) != NULL && count < MAX_PROC) {
        if (entry->d_type == DT_DIR && isdigit(entry->d_name[0])) {
            get_process_info(&proc[count++], entry->d_name);
        }
    }
    closedir(dir);

    for (int i = 0; i < count; i++) {
        print_process_info(&proc[i]);
    }

    return 0;
}
