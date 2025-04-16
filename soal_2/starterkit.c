#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <signal.h>
#include <sys/wait.h>

#define STARTER_KIT_ZIP "starter_kit.zip"
#define STARTER_KIT_DIR "starter_kit"
#define QUARANTINE_DIR "quarantine"
#define LOG_FILE "activity.log"
#define PID_FILE "/tmp/starterkit_decrypt.pid"
#define DOWNLOAD_URL "https://drive.google.com/uc?export=download&id=1_5GxIGfQr3mNKuavJbte_AoRkEQLXSKS"

int stop_daemon = 0;

void handle_sigterm(int sig) {
    stop_daemon = 1;
}

void write_log_action(const char *action, const char *detail) {
    FILE *log = fopen(LOG_FILE, "a");
    if (!log) return;

    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char timestamp[32];
    strftime(timestamp, sizeof(timestamp), "%d-%m-%Y][%H:%M:%S", tm_info);

    if (detail)
        fprintf(log, "[%s] - %s - %s\n", timestamp, detail, action);
    else
        fprintf(log, "[%s] - %s\n", timestamp, action);

    fclose(log);
}

void download_and_extract() {
    struct stat st;
    if (stat(STARTER_KIT_DIR, &st) == 0) return;

    printf("⏬ Download dan mengekstrak starter kit, tunggu kejap...\n");

    pid_t pid = fork();
    if (pid == 0) {
        char *args[] = {"sh", "-c", "wget -q --show-progress -O starter_kit.zip 'https://drive.google.com/uc?export=download&id=1_5GxIGfQr3mNKuavJbte_AoRkEQLXSKS'", NULL};
        execv("/bin/sh", args);
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        wait(NULL);
    }

    mkdir(STARTER_KIT_DIR, 0755);

    pid = fork();
    if (pid == 0) {
        char *args[] = {"sh", "-c", "unzip -q starter_kit.zip -d starter_kit", NULL};
        execv("/bin/sh", args);
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        wait(NULL);
    }

    remove(STARTER_KIT_ZIP);
    printf("✅ Starter kit berhasil terdownload dan diekstrak di starter_kit.\n");
}

char *base64_decode(const char *input) {
    static char output[256];
    memset(output, 0, sizeof(output));

    const char base64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    int val = 0, valb = -8;
    size_t len = 0;

    for (const char *p = input; *p && *p != '='; p++) {
        const char *pos = strchr(base64_table, *p);
        if (!pos) return NULL;
        val = (val << 6) + (pos - base64_table);
        valb += 6;
        if (valb >= 0) {
            output[len++] = (char)((val >> valb) & 0xFF);
            valb -= 8;
            if (len >= sizeof(output) - 1) break;
        }
    }

    output[len] = '\0';
    if (len < 1) return NULL;

    for (size_t i = 0; i < len; i++) {
        if (output[i] == '\n' || output[i] == '\r') {
            output[i] = '\0';
            break;
        }
    }

    return output;
}

void decrypt_process() {
    FILE *pidfile = fopen(PID_FILE, "w");
    if (pidfile) {
        fprintf(pidfile, "%d", getpid());
        fclose(pidfile);
    } else {
        return;
    }

    char message[128];
    snprintf(message, sizeof(message), "Successfully started decryption process with PID %d.", getpid());
    write_log_action(message, NULL);
    printf("🔓 Proses decryption dimulai : (PID %d). Segala activity akan tercatat dalam '%s'.\n", getpid(), LOG_FILE);

    while (!stop_daemon) {
        DIR *dir = opendir(QUARANTINE_DIR);
        if (!dir) {
            sleep(10);
            continue;
        }

        struct dirent *entry;
        while ((entry = readdir(dir))) {
            if (entry->d_type == DT_REG) {
                char clean_name[256];
                strncpy(clean_name, entry->d_name, sizeof(clean_name));
                clean_name[sizeof(clean_name)-1] = '\0';
                char *newline = strchr(clean_name, '\n');
                if (newline) *newline = '\0';
                if (clean_name[0] == '\'' && clean_name[strlen(clean_name)-1] == '\'') {
                    memmove(clean_name, clean_name+1, strlen(clean_name));
                    clean_name[strlen(clean_name)-2] = '\0';
                }

                char *decoded = base64_decode(clean_name);
                if (decoded && strlen(decoded) > 0 && strcmp(clean_name, decoded) != 0) {
                    char oldpath[512], newpath[512];
                    snprintf(oldpath, sizeof(oldpath), "%s/%s", QUARANTINE_DIR, entry->d_name);
                    snprintf(newpath, sizeof(newpath), "%s/%s", QUARANTINE_DIR, decoded);
                    rename(oldpath, newpath);
                }
            }
        }
        closedir(dir);
        sleep(10);
    }
}

void move_files(const char *src, const char *dst, const char *label) {
    DIR *dir = opendir(src);
    if (!dir) return;

    int found = 0;
    struct dirent *entry;
    while ((entry = readdir(dir))) {
        if (entry->d_type == DT_REG) {
            found = 1;
            char oldpath[512], newpath[512];
            snprintf(oldpath, sizeof(oldpath), "%s/%s", src, entry->d_name);
            snprintf(newpath, sizeof(newpath), "%s/%s", dst, entry->d_name);

            struct stat st;
            if (stat(newpath, &st) == 0) continue;

            rename(oldpath, newpath);

            if (strcmp(label, "quarantine") == 0) {
                write_log_action("Successfully moved to quarantine directory.", entry->d_name);
            } else {
                write_log_action("Successfully returned to starter kit directory.", entry->d_name);
            }
        }
    }
    if (!found) {
        printf("⚠️ Tidak ada file dalam %s untuk dipindah\n", src);
    } else {
        if (strcmp(label, "quarantine") == 0) {
            printf("📁 Seluruh file berhasil dipindahkan ke quarantine.\n");
        } else {
            printf("📂 Seluruh file berhasil dikembalikan ke starter kit.\n");
        }
    }
    closedir(dir);
}

void eradicate_files() {
    DIR *dir = opendir(QUARANTINE_DIR);
    if (!dir) {
        printf("⚠️ Tidak ada quarantine folder.\n");
        return;
    }

    int found = 0;
    struct dirent *entry;
    while ((entry = readdir(dir))) {
        if (entry->d_type == DT_REG) {
            found = 1;
            char path[512];
            snprintf(path, sizeof(path), "%s/%s", QUARANTINE_DIR, entry->d_name);
            remove(path);
            write_log_action("Successfully deleted.", entry->d_name);
        }
    }
    if (!found) {
        printf("⚠️ Files dalam quarantine tidak ditemukan untuk eradicate.\n");
    } else {
        printf("🗑️ Semua file telah dihapus dalam quarantine.\n");
    }
    closedir(dir);
}

void shutdown_daemon() {
    FILE *pidfile = fopen(PID_FILE, "r");
    if (!pidfile) {
        printf("❌ Error: Tidak ada proses decrypt yang running.\n");
        return;
    }
    int pid;
    if (fscanf(pidfile, "%d", &pid) != 1) {
        printf("❌ Error: Gagal untuk membaca PID file.\n");
        fclose(pidfile);
        return;
    }
    fclose(pidfile);
    kill(pid, SIGTERM);
    remove(PID_FILE);

    char message[128];
    snprintf(message, sizeof(message), "Successfully shut off decryption process with PID %d.", pid);
    write_log_action(message, NULL);
    printf("🔒 Proses decrytion telah berhasil dimatikan.\n");
}

int main(int argc, char *argv[]) {
    signal(SIGTERM, handle_sigterm);
    mkdir(QUARANTINE_DIR, 0755);
    download_and_extract();

    if (argc < 2) {
        printf("📌 Usage: ./starterkit [--decrypt | --quarantine | --return | --eradicate | --shutdown]\n");
        return 1;
    }

    printf("📝 Seluruh activity akan disimpan dalam '%s'.\n", LOG_FILE);

    if (strcmp(argv[1], "--decrypt") == 0) {
        FILE *pidfile = fopen(PID_FILE, "r");
        if (pidfile) {
            fclose(pidfile);
            printf("⚠️ Proses decryption telah berjalan.\n");
            return 1;
        }
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            return 1;
        }
        if (pid > 0) {
            return 0;
        }
        setsid();
        umask(0);
        decrypt_process();

    } else if (strcmp(argv[1], "--quarantine") == 0) {
        move_files(STARTER_KIT_DIR, QUARANTINE_DIR, "quarantine");

    } else if (strcmp(argv[1], "--return") == 0) {
        move_files(QUARANTINE_DIR, STARTER_KIT_DIR, "return");

    } else if (strcmp(argv[1], "--eradicate") == 0) {
        eradicate_files();

    } else if (strcmp(argv[1], "--shutdown") == 0) {
        shutdown_daemon();

    } else {
        printf("❌ Error: Command '%s' tidak dikenali. Gunakan command usage yang ada.\n", argv[1]);
        printf("📌 Usage: ./starterkit [--decrypt | --quarantine | --return | --eradicate | --shutdown]\n");
        return 1;
    }

    return 0;
}
