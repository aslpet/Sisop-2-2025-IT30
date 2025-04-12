#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <ctype.h>
#include <limits.h>
#include <stdbool.h>

#define BIG_PATH (PATH_MAX * 2)

int is_regular_file(const char *path) {
    struct stat path_stat;
    return stat(path, &path_stat) == 0 && S_ISREG(path_stat.st_mode);
}

int is_valid_clue_file(const char *name) {
    return strlen(name) == 5 &&
           name[1] == '.' && name[2] == 't' && name[3] == 'x' && name[4] == 't' &&
           (isdigit(name[0]) || isalpha(name[0]));
}

void download_file(const char *url, const char *output) {
    pid_t pid = fork();
    if (pid == 0) {
        char *argv[] = {"wget", "-q", (char *)url, "-O", (char *)output, NULL};
        execvp("wget", argv);
        perror("execvp wget");
        exit(1);
    } else {
        wait(NULL);
    }
}

void unzip_file(const char *file) {
    pid_t pid = fork();
    if (pid == 0) {
        char *argv[] = {"unzip", "-o", (char *)file, NULL};
        execvp("unzip", argv);
        perror("execvp unzip");
        exit(1);
    } else {
        wait(NULL);
    }
}

void move_and_clean(const char *basepath) {
    char *clue_folders[] = {"ClueA", "ClueB", "ClueC", "ClueD"};
    char target_dir[] = "Filtered";
    mkdir(target_dir, 0755);

    for (int i = 0; i < 4; i++) {
        char path[BIG_PATH];
        snprintf(path, sizeof(path), "%s/%s", basepath, clue_folders[i]);

        DIR *dir = opendir(path);
        if (!dir) continue;

        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;

            // Hitung panjang filepath lebih awal
            size_t len_path = strlen(path);
            size_t len_name = strlen(entry->d_name);
            size_t total_len = len_path + 1 + len_name + 1;

            if (total_len > BIG_PATH) {
                fprintf(stderr, "Filepath too long: %s/%s\n", path, entry->d_name);
                continue;
            }

            char filepath[BIG_PATH];
            if (strlen(path) + 1 + strlen(entry->d_name) + 1 > sizeof(filepath)) {
                fprintf(stderr, "Filepath too long: %s/%s\n", path, entry->d_name);
                continue;
            }
            
            strcpy(filepath, path);
            strcat(filepath, "/");
            strcat(filepath, entry->d_name);

            if (!is_regular_file(filepath)) continue;

            if (is_valid_clue_file(entry->d_name)) {
                size_t len_dest = strlen(target_dir) + 1 + len_name + 1;
                if (len_dest > BIG_PATH) {
                    fprintf(stderr, "Dest path too long: %s/%s\n", target_dir, entry->d_name);
                    continue;
                }

                char dest[BIG_PATH];
                if (strlen(target_dir) + 1 + strlen(entry->d_name) + 1 > sizeof(dest)) {
                    fprintf(stderr, "Dest path too long: %s/%s\n", target_dir, entry->d_name);
                    continue;
                }
                
                strcpy(dest, target_dir);
                strcat(dest, "/");
                strcat(dest, entry->d_name);

                if (rename(filepath, dest) != 0)
                    perror("rename");
            } else {
                remove(filepath);
            }
        }

        closedir(dir);
    }
}

int compare(const void *a, const void *b) {
    const char *fa = *(const char **)a;
    const char *fb = *(const char **)b;
    return strcmp(fa, fb);
}

bool is_digit_file(const char *name) {
    return isdigit(name[0]);
}

bool is_alpha_file(const char *name) {
    return isalpha(name[0]);
}

void combine_files() {
    DIR *dir = opendir("Filtered");
    if (!dir) {
        perror("opendir Filtered");
        return;
    }

    struct dirent *entry;
    char *digits[512], *alphas[512];
    int dcount = 0, acount = 0;

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type != DT_REG || !is_valid_clue_file(entry->d_name)) continue;

        if (is_digit_file(entry->d_name))
            digits[dcount++] = strdup(entry->d_name);
        else if (is_alpha_file(entry->d_name))
            alphas[acount++] = strdup(entry->d_name);
    }
    closedir(dir);

    qsort(digits, dcount, sizeof(char *), compare);
    qsort(alphas, acount, sizeof(char *), compare);

    FILE *out = fopen("Combined.txt", "w");
    if (!out) {
        perror("fopen Combined.txt");
        return;
    }

    int i = 0, j = 0;
    bool pick_digit = true;

    while (i < dcount || j < acount) {
        const char *fname = NULL;
        if (pick_digit && i < dcount) {
            fname = digits[i++];
        } else if (!pick_digit && j < acount) {
            fname = alphas[j++];
        }
        pick_digit = !pick_digit;

        if (fname) {
            char fullpath[BIG_PATH];
            snprintf(fullpath, sizeof(fullpath), "Filtered/%s", fname);

            FILE *in = fopen(fullpath, "r");
            if (in) {
                int c;
                while ((c = fgetc(in)) != EOF) {
                    fputc(c, out);
                }
                fclose(in);
                remove(fullpath);
            } else {
                perror("fopen input file");
            }
        }
    }

    fclose(out);

    for (int k = 0; k < dcount; k++) free(digits[k]);
    for (int k = 0; k < acount; k++) free(alphas[k]);
}

char rot13_char(char c) {
    if ('a' <= c && c <= 'z') return ((c - 'a' + 13) % 26) + 'a';
    if ('A' <= c && c <= 'Z') return ((c - 'A' + 13) % 26) + 'A';
    return c;
}

void decode_file() {
    FILE *in = fopen("Combined.txt", "r");
    if (!in) {
        perror("fopen Combined.txt");
        return;
    }

    FILE *out = fopen("Decoded.txt", "w");
    if (!out) {
        perror("fopen Decoded.txt");
        fclose(in);
        return;
    }

    int c;
    while ((c = fgetc(in)) != EOF) {
        fputc(rot13_char(c), out);
    }

    fclose(in);
    fclose(out);
}

void validate_password() {
    FILE *fp = fopen("Decoded.txt", "r");
    if (!fp) {
        perror("fopen Decoded.txt");
        return;
    }

    char password[256];
    if (fgets(password, sizeof(password), fp) == NULL) {
        fprintf(stderr, "Decoded.txt is empty or invalid\n");
        fclose(fp);
        return;
    }
    size_t len = strlen(password);
    if (len > 0 && password[len - 1] == '\n') password[len - 1] = '\0';

    fclose(fp);

    const char *correct = "dragonkey123";
    if (strcmp(password, correct) == 0) {
        printf("Password accepted. Pintu terbuka!\n");
    } else {
        printf("Password salah. Naga Cyrus menatapmu dengan tajam...\n");
    }
}

int main(int argc, char *argv[]) {
    if (argc == 3 && strcmp(argv[1], "-m") == 0) {
        if (strcmp(argv[2], "Filter") == 0) {
            move_and_clean("Clues");
            return 0;
        } else if (strcmp(argv[2], "Combine") == 0) {
            combine_files();
            return 0;
        } else if (strcmp(argv[2], "Decode") == 0) {
            decode_file();
            return 0;
        } else if (strcmp(argv[2], "Validate") == 0) {
            validate_password();
            return 0;
        }
    } else if (argc == 1) {
        struct stat st;
        if (stat("Clues", &st) == 0 && S_ISDIR(st.st_mode)) {
            printf("Clues folder already exists. Skipping download.\n");
            return 0;
        }

        const char *url = "https://drive.usercontent.google.com/u/0/uc?id=1xFn1OBJUuSdnApDseEczKhtNzyGekauK&export=download";
        const char *zipname = "Clues.zip";

        download_file(url, zipname);
        unzip_file(zipname);
        remove(zipname);

        return 0;
    }

    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "  ./action               # Download and unzip if not exists\n");
    fprintf(stderr, "  ./action -m Filter     # Filter valid clue files\n");
    fprintf(stderr, "  ./action -m Combine    # Combine filtered files\n");
    fprintf(stderr, "  ./action -m Decode     # Decode Combined.txt using ROT13\n");
    fprintf(stderr, "  ./action -m Validate   # Check if password is correct\n");
    return 1;
}
