#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <wait.h>
#include <math.h>

#define LOG_PATH "/home/seribu_man/SISOP_2/soal_4/debugmon.log"

// --- Fungsi umum dan Timestamp ---
void get_timestamp(char *buffer, size_t size) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buffer, size, "[%d:%m:%Y]-[%H:%M:%S]", t);
}

unsigned int get_uid_from_username(const char *username) {
    FILE *fp = fopen("/etc/passwd", "r");
    if (!fp) {
        perror("/etc/passwd");
        exit(EXIT_FAILURE);
    }

    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        char *uname = strtok(line, ":");
        strtok(NULL, ":"); 
        char *uid_str = strtok(NULL, ":");

        if (uname && uid_str && strcmp(uname, username) == 0) {
            fclose(fp);
            return (unsigned int)atoi(uid_str);
        }
    }

    fclose(fp);
    fprintf(stderr, "User '%s' tidak ditemukan\n", username);
    exit(EXIT_FAILURE);
}

unsigned int get_uid_from_status(const char *pid) {
    char path[256], line[256];
    snprintf(path, sizeof(path), "/proc/%s/status", pid);

    FILE *fp = fopen(path, "r");
    if (!fp) return (unsigned int)-1;

    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "Uid:", 4) == 0) {
            unsigned int uid;
            sscanf(line, "Uid:\t%u", &uid);
            fclose(fp);
            return uid;
        }
    }

    fclose(fp);
    return (unsigned int)-1;
}

void get_command(const char *pid, char *out, size_t size) {
    char path[256];
    snprintf(path, sizeof(path), "/proc/%s/comm", pid);
    FILE *fp = fopen(path, "r");
    if (!fp) {
        strncpy(out, "-", size);
        return;
    }

    fgets(out, size, fp);
    out[strcspn(out, "\n")] = 0;
    fclose(fp);
}

long get_memory_kb(const char *pid) {
    char path[256], line[256];
    snprintf(path, sizeof(path), "/proc/%s/status", pid);
    FILE *fp = fopen(path, "r");
    if (!fp) return 0;

    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "VmRSS:", 6) == 0) {
            long mem;
            sscanf(line, "VmRSS: %ld", &mem);
            fclose(fp);
            return mem;
        }
    }
    fclose(fp);
    return 0;
}

long get_total_memory_kb() {
    FILE *fp = fopen("/proc/meminfo", "r");
    char line[256];
    if (!fp) return 1;

    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "MemTotal:", 9) == 0) {
            long mem;
            sscanf(line, "MemTotal: %ld", &mem);
            fclose(fp);
            return mem;
        }
    }

    fclose(fp);
    return 1;
}

float get_cpu_usage(const char *pid, long uptime_jiffies, long hertz) {
    char path[256], line[1024];
    snprintf(path, sizeof(path), "/proc/%s/stat", pid);
    FILE *fp = fopen(path, "r");
    if (!fp) return 0.0f;

    fgets(line, sizeof(line), fp);
    fclose(fp);

    long utime, stime, starttime;
    sscanf(line,
           "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %ld %ld %*d %*d %*d %*d %*d %ld",
           &utime, &stime, &starttime);

    long total_time = utime + stime;
    long seconds = uptime_jiffies / hertz - starttime / hertz;
    if (seconds <= 0) return 0.0f;

    return 100.0f * ((float)total_time / hertz) / seconds;
}

// === List ===
void list_user_processes(const char *username) {
    unsigned int uid = get_uid_from_username(username);
    long total_mem_kb = get_total_memory_kb();
    long hertz = sysconf(_SC_CLK_TCK);

    FILE *uptime_fp = fopen("/proc/uptime", "r");
    if (!uptime_fp) return;
    double uptime_sec;
    fscanf(uptime_fp, "%lf", &uptime_sec);
    fclose(uptime_fp);
    long uptime_jiffies = (long)(uptime_sec * hertz);

    DIR *dir = opendir("/proc");
    struct dirent *entry;

    printf("%-8s %-20s %8s %8s\n", "PID", "COMMAND", "CPU(%)", "MEM(%)");
    printf("--------------------------------------------\n");

    FILE *log = fopen(LOG_PATH, "a");
    char timestamp[64];

    while ((entry = readdir(dir)) != NULL) {
        struct stat st;
        char fullpath[512];
        snprintf(fullpath, sizeof(fullpath), "/proc/%s", entry->d_name);

        if (isdigit(entry->d_name[0]) && stat(fullpath, &st) == 0 && S_ISDIR(st.st_mode)) {
            char *pid = entry->d_name;
            if (get_uid_from_status(pid) != uid) continue;

            char cmd[128];
            get_command(pid, cmd, sizeof(cmd));
            long mem_kb = get_memory_kb(pid);
            float mem_percent = 100.0f * mem_kb / total_mem_kb;
            float cpu_percent = get_cpu_usage(pid, uptime_jiffies, hertz);

            printf("%-8s %-20s %8.2f %8.2f\n", pid, cmd, cpu_percent, mem_percent);

            get_timestamp(timestamp, sizeof(timestamp));
            fprintf(log, "%s_%s_RUNNING\n", timestamp, cmd);
        }
    }

    fclose(log);
    closedir(dir);
}

// === Stop ===
void stop_daemon(const char *username) {
    char pidfile[256];
    snprintf(pidfile, sizeof(pidfile), "/tmp/debugmon-%s.pid", username);

    FILE *fp = fopen(pidfile, "r");
    if (!fp) {
        perror("PID file tidak ditemukan");
        return;
    }

    int pid;
    fscanf(fp, "%d", &pid);
    fclose(fp);

    if (kill(pid, SIGTERM) == 0) {
        remove(pidfile);
        printf("Daemon dihentikan.\n");
    } else {
        perror("Gagal menghentikan daemon");
    }
}

// === Daemon ===
void run_as_daemon(const char *username) {
    pid_t pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS);

    umask(0);
    pid_t sid = setsid();
    if (sid < 0) exit(EXIT_FAILURE);
    if ((chdir("/")) < 0) exit(EXIT_FAILURE);

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    char pidpath[256];
    snprintf(pidpath, sizeof(pidpath), "/tmp/debugmon-%s.pid", username);
    FILE *pidfp = fopen(pidpath, "w");
    if (pidfp) {
        fprintf(pidfp, "%d", getpid());
        fclose(pidfp);
    }

    unsigned int uid = get_uid_from_username(username);
    long total_mem_kb = get_total_memory_kb();
    long hertz = sysconf(_SC_CLK_TCK);
    char timestamp[64];

    while (1) {
        FILE *uptime_fp = fopen("/proc/uptime", "r");
        if (!uptime_fp) continue;
        double uptime_sec;
        fscanf(uptime_fp, "%lf", &uptime_sec);
        fclose(uptime_fp);
        long uptime_jiffies = (long)(uptime_sec * hertz);

        DIR *dir = opendir("/proc");
        struct dirent *entry;
        FILE *log = fopen(LOG_PATH, "a");

        while ((entry = readdir(dir)) != NULL) {
            struct stat st;
            char fullpath[512];
            snprintf(fullpath, sizeof(fullpath), "/proc/%s", entry->d_name);

            if (isdigit(entry->d_name[0]) && stat(fullpath, &st) == 0 && S_ISDIR(st.st_mode)) {
                char *pid = entry->d_name;
                if (get_uid_from_status(pid) != uid) continue;

                char cmd[128];
                get_command(pid, cmd, sizeof(cmd));
                get_timestamp(timestamp, sizeof(timestamp));
                fprintf(log, "%s_%s_RUNNING\n", timestamp, cmd);
            }
        }

        fprintf(log, "----------------------------------------\n");
        fclose(log);
        closedir(dir);
        sleep(5);
    }
}

// === Fail ===
void fail(const char *username) {
    pid_t pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS);

    umask(0);
    setsid();
    chdir("/");

    char pidfile[256];
    snprintf(pidfile, sizeof(pidfile), "/tmp/debugmon-fail-%s.pid", username);
    FILE *fp = fopen(pidfile, "w");
    if (fp) {
        fprintf(fp, "%d", getpid());
        fclose(fp);
    }

    const char *whitelist[] = {
        "bash", "sh", "zsh", "sudo", "revert_daemon"
    };
    int whitelist_size = sizeof(whitelist) / sizeof(whitelist[0]);

    unsigned int uid = get_uid_from_username(username);
    char timestamp[64];

    while (1) {
        DIR *dir = opendir("/proc");
        if (!dir) continue;
        struct dirent *entry;
        FILE *log = fopen(LOG_PATH, "a");

        while ((entry = readdir(dir)) != NULL) {
            if (!isdigit(entry->d_name[0])) continue;

            char proc_path[512];
            snprintf(proc_path, sizeof(proc_path), "/proc/%s", entry->d_name);
            struct stat st;
            if (stat(proc_path, &st) == -1 || !S_ISDIR(st.st_mode)) continue;

            if (st.st_uid != uid) continue;

            int pid_int = atoi(entry->d_name);
            if (pid_int == getpid()) continue;

            char cmdline[128] = "";
            get_command(entry->d_name, cmdline, sizeof(cmdline));

            int allowed = 0;
            for (int i = 0; i < whitelist_size; ++i) {
                if (strcmp(cmdline, whitelist[i]) == 0) {
                    allowed = 1;
                    break;
                }
            }

            if (!allowed) {
                if (kill(pid_int, SIGKILL) == 0) {
                    get_timestamp(timestamp, sizeof(timestamp));
                    if (log) fprintf(log, "%s_KILLED_%s\n", timestamp, cmdline);
                }
            }
        }

        if (log) fclose(log);
        closedir(dir);
        sleep(2);
    }
}

// === Revert ===
void revert(const char *username) {
    if (getuid() != 0) {
        fprintf(stderr, "Revert hanya boleh dilakukan oleh root.\n");
        return;
    }

    char pidfile[256];
    snprintf(pidfile, sizeof(pidfile), "/tmp/debugmon-fail-%s.pid", username);

    FILE *fp = fopen(pidfile, "r");
    if (!fp) {
        perror("PID file tidak ditemukan");
        return;
    }

    int pid;
    fscanf(fp, "%d", &pid);
    fclose(fp);

    if (kill(pid, SIGTERM) == 0) {
        remove(pidfile);
        FILE *log = fopen(LOG_PATH, "a");
        char timestamp[64];
        get_timestamp(timestamp, sizeof(timestamp));
        fprintf(log, "%s_REVERT_SUCCESS\n", timestamp);
        fclose(log);
    } else {
        perror("Gagal menghentikan daemon");
    }
}

// --- Main ---
int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Gunakan: %s [list|daemon|stop|fail|revert] <username>\n", argv[0]);
        return 1;
    }

    const char *mode = argv[1];
    const char *username = argv[2];

    if (strcmp(mode, "list") == 0) {
        list_user_processes(username);
    } else if (strcmp(mode, "daemon") == 0) {
        run_as_daemon(username);
    } else if (strcmp(mode, "stop") == 0) {
        stop_daemon(username);
    } else if (strcmp(mode, "fail") == 0) {
        fail(username);
    } else if (strcmp(mode, "revert") == 0) {
        revert(username);
    } else {
        fprintf(stderr, "Mode tidak dikenali: %s\n", mode);
        return 1;
    }

    return 0;
}
