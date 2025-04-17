# Sisop-2-2025-IT30
### by IT30


### Anggota Kelompok

| No | Nama                              | NRP         |
|----|-----------------------------------|------------|
| 1  | Adiwidya Budi Pratama            | 5027241012 |
| 2  | Ahmad Rafi Fadhillah Dwiputra     | 5027241068 |
| 3  | Dimas Satya Andhika              | 5027241032 |

<div align=center>
	
## Soal_1
</div>

__ACTION, DRAGON CYRUS 🐉__ 
=

```c
#define BIG_PATH (PATH_MAX * 2)
```
Konstanta yang didefinisikan untuk menyimpan path file atau folder dengan ukuran lebih besar dari standar.

---
**is_regular_file**
```c
int is_regular_file(const char *path) {
    struct stat path_stat;
    return stat(path, &path_stat) == 0 && S_ISREG(path_stat.st_mode);
}
```
- Menggunakan stat() untuk mendapatkan informasi file (struct stat).
- Fungsi akan mengembalikan 1 (true) jika file adalah regular file (bukan folder, symlink, dsb), yaitu jika S_ISREG(path_stat.st_mode) benar.
- Jika gagal memanggil stat, atau file bukan regular file → return 0.
----

**is_valid_clue_file**
```c
int is_valid_clue_file(const char *name) {
return strlen(name) == 5 &&
    name[1] == '.' && name[2] == 't' && name[3] == 'x' && name[4] == 't' &&
    (isdigit(name[0]) || isalpha(name[0]));
}
```
- Mengecek apakah nama file:

    - Berjumlah tepat 5 karakter.

    - Formatnya [karakter].txt.

    - Awal namanya adalah huruf atau angka.

- Contoh valid: A.txt, 3.txt

---
**download_file**
```c
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
```
- Membuat child process dengan fork().
- Child process mengeksekusi command wget -q <url> -O <output>:
    - -q: quiet mode (tanpa output)
    - -O: simpan hasil download ke nama file tertentu
- Parent process menunggu dengan wait(NULL) sampai download selesai.
- Jika execvp gagal, perror mencetak error.

---
**unzip_file**
```c
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
```
- Seperti download_file, fungsi ini menjalankan perintah:
    - unzip -o <file> di child process
    - -o: overwrite file jika sudah ada
- Proses utama menunggu sampai unzip selesai.

---
**move_and_clean**
```c
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
```
- Memuat folder Filtered jika belum ada.
- Masuk ke masing-masing folder ClueA–ClueD.
- Setiap file diperiksa:
    - Jika valid (format clue), dipindahkan ke         Filtered/. → pakai rename(src, dest)
    - Jika tidak valid, dihapus. → pakai remove(path)

---
**compare**
```c
int compare(const void *a, const void *b) {
    const char *fa = *(const char **)a;
    const char *fb = *(const char **)b;
    return strcmp(fa, fb);
}
```
- Fungsi pembanding ini digunakan oleh qsort() untuk mengurutkan array string.
- void *a dan void *b adalah pointer ke elemen array char*, jadi:
    - *(const char **)a → ambil string fa
    - *(const char **)b → ambil string fb
- Kemudian, strcmp(fa, fb) membandingkan dua string secara alfabetis.

---
```c
**is_digit_file**
bool is_digit_file(const char *name) {
    return isdigit(name[0]);
}
```
- Mengecek apakah karakter pertama nama file adalah angka (0–9).
- isdigit() adalah fungsi dari <ctype.h>, hasilkan true jika input adalah digit.

---
**is_alpha_file**
```c
bool is_alpha_file(const char *name) {
    return isalpha(name[0]);
}
```
- Mengecek apakah karakter pertama nama file adalah huruf alfabet (a–z, A–Z).
- isalpha() juga dari <ctype.h>, hasilkan true jika huruf.

---
**combine_files**
```c
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
```
- Buka folder Filtered/.
- Pisahkan file yang diawali angka dan huruf ke array berbeda.
- Urutkan keduanya.
- Gabungkan konten file ke Combined.txt dengan urutan selang-seling:
    - digit → alpha → digit → alpha, dst.
- File yang sudah dibaca langsung dihapus.

---
**rot13_char**
```c
char rot13_char(char c) {
    if ('a' <= c && c <= 'z') return ((c - 'a' + 13) % 26) + 'a';
    if ('A' <= c && c <= 'Z') return ((c - 'A' + 13) % 26) + 'A';
    return c;
}
```
- ungsi ROT13: menggeser huruf alfabet sebanyak 13.
- Non-alfabet tidak berubah.

---
**decode_file***
```c
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
```
- Buka Combined.txt, baca tiap karakter.
- Enkripsi balik menggunakan ROT13 → tulis ke Decoded.txt.

---
**validate_password**
```c
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

    const char *correct = "BewareOfAmpy";
    if (strcmp(password, correct) == 0) {
        printf("YEAYYY password benar. Pintu terbuka!\n");
    } else {
        printf("Password salah. Naga Cyrus membakar mu dengan api neraka\n");
    }
}
```
- Ambil baris pertama dari Decoded.txt.
- Bandingkan password dengan string "BewareOfAmpy".
- Output pesan sesuai hasil validasi.

---
**main**
```c
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
            printf("Folder clue sudah ada\n");
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
```
```c
if (argc == 3 && strcmp(argv[1], "-m") == 0) {
```
mengecek argumen yang diberikan

```c
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
```
memilih mode yang dijalankan

```c
} else if (argc == 1) {
```
Jika tidak ada argumen, program akan memulai proses inisialisasi.

```c
if (stat("Clues", &st) == 0 && S_ISDIR(st.st_mode)) {
    printf("Folder clue sudah ada\n");
```
jika folder Clues sudah ada, hentikan proses (tidak perlu download ulang).

```c
Copy code
const char *url = "...";
const char *zipname = "Clues.zip";
download_file(url, zipname);
unzip_file(zipname);
remove(zipname);
```
mengunduh Clues.zip, mengekstrak isinya, lalu menghapus file ZIP-nya.

```c
fprintf(stderr, "Usage:\n");
```
menampilkanp panduan jika argumen tidak valid

<div align=center>
	
## Soal_2
</div>


<div align=center>

[Author: Afnaan / honque]

Dok dok dorokdok dok rodok. Anomali malware yang dikembangkan oleh Andriana di PT Mafia Security Cabang Ngawi yang hanya keluar di malam pengerjaan soal shift modul 2. Konon katanya anomali ini akan mendatangi praktikan sisop yang tidak mengerjakan soal ini. Ihh takutnyeee. Share ke teman teman kalian yang tidak mengerjakan soal ini

![image](https://github.com/user-attachments/assets/a5aa5a54-1cca-42a2-930a-9e7cc9966b19)

A. Malware ini bekerja secara daemon dan menginfeksi perangkat korban dan menyembunyikan diri dengan mengganti namanya menjadi /init.

## Soal_3
</div>


<div align=center>
	
## Soal_4
</div>


# Doraemon: Nobita dan Alat Pengintai Proses

```c
#define LOG_PATH "/home/seribu_man/SISOP_2/soal_4/debugmon.log"
```
Lokasi log file tempat semua aktivitas dicatat, seperti proses yang sedang berjalan, proses yang dihentikan, dan status daemon.

---
**get_timestamp**
```c
void get_timestamp(char *buffer, size_t size) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buffer, size, "[%d:%m:%Y]-[%H:%M:%S]", t);
}
```
- Mengisi buffer dengan waktu lokal sekarang dalam format yang telah ditentukan.
- Dipakai untuk memberi timestamp di setiap log entri.

---
**get_uid_from_username**
```c
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
```
Fungsi ini bertugas untuk mengambil UID dari user yang namanya diberikan sebagai argumen, dengan cara membaca file `/etc/passwd` . File tersebut berisi info user dalam format terpisah oleh titik dua (`:`), lalu fungsi ini membaca tiap baris, memisahkan field-nya menggunakan `strtok`, dan membandingkan username di baris tersebut dengan input. Jika cocok, UID diambil dari field ketiga dan dikembalikan sebagai hasil fungsi. Kalau tidak ditemukan sampai akhir file, program langsung keluar karena user dianggap tidak valid.

---
**get_uid_from_status**
```c
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
```
Fungsi ini digunakan untuk mengambil UID dari sebuah proses berdasarkan PID-nya, dengan cara membuka file `/proc/<pid>/status` dan mencari baris yang diawali dengan "Uid:". File tersebut menyimpan berbagai informasi status proses, dan UID berada di baris itu. Jika file tidak bisa dibuka (misalnya proses sudah mati), maka fungsi mengembalikan -1 sebagai tanda gagal. Ini penting untuk menyaring hanya proses milik user target.

---
**get_command**
```c
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
```
Fungsi ini mengambil nama executable dari suatu proses berdasarkan PID, dengan cara membaca file `/proc/<pid>/comm` yang berisi nama singkat command tersebut. Nama tersebut kemudian disalin ke buffer output, dan newline (jika ada) dihapus dari akhir string. Kalau file tidak bisa dibuka, fungsi hanya mengisi `out` dengan karakter "-" sebagai fallback.

---
**get_memory_kb**
```c
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
```
Fungsi ini membaca penggunaan memori (RSS, dalam KB) dari suatu proses berdasarkan PID, dengan membuka `/proc/<pid>/status` dan mencari baris yang diawali dengan "VmRSS:". Nilai memori di baris tersebut diambil dan dikembalikan. Jika file tidak bisa dibuka atau baris tidak ditemukan, maka fungsi mengembalikan 0 sebagai default.

---
**get_total_memory_kb**
```c
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
```
Fungsi ini mengambil total memori sistem (RAM fisik) dalam satuan kilobyte, dengan membuka file `/proc/meminfo` dan mencari baris "MemTotal:". Nilai tersebut kemudian dikembalikan. Jika terjadi kesalahan saat membuka file atau parsing, maka nilai fallback 1 dikembalikan untuk menghindari pembagian dengan nol di tempat lain.

---
**get_cpu_usage**
```c
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
```
Fungsi ini menghitung persentase penggunaan CPU dari sebuah proses, dengan membaca file `/proc/<pid>/stat` dan mengambil nilai utime, stime, dan starttime. Waktu proses dijalankan dikurangi dari uptime sistem untuk tahu berapa lama proses hidup, lalu dibagi dengan waktu CPU aktifnya untuk mendapatkan persentase. Jika proses terlalu baru (waktu hidupnya nol), hasil dikembalikan sebagai 0%.

---
**List**
```c
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
```
Fungsi ini menampilkan semua proses milik user tertentu dalam bentuk tabel berisi PID, nama proses, CPU usage, dan memori usage. Ia membuka folder `/proc`, menyaring direktori yang merupakan PID, lalu memeriksa UID pemilik proses dan hanya mencetak jika cocok dengan UID user target. Setiap entri juga dicatat ke log `/home/seribu_man/SISOP_2/soal_4/debugmon.log` dengan format timestamp_nama_RUNNING.

---
**Daemon**
```c
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
```
Fungsi ini menjalankan debugmon sebagai daemon (background process) untuk user tertentu, mencatat proses milik user tersebut setiap 5 detik ke file log. Proses utama akan fork dan keluar, lalu child process membuat session baru, menutup file descriptor standar, dan menyimpan PID-nya di file `/tmp/debugmon-<username>.pid`. Setiap iterasi, ia membuka `/proc`, mencari proses milik user target, dan mencatat statusnya ke log lengkap dengan timestamp.

---
**Stop**
```c
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
```
Fungsi ini menghentikan daemon debugmon milik user tertentu dengan membaca file PID yang terletak di `/tmp/debugmon-<username>.pid`, mengambil PID-nya, lalu mengirim sinyal `SIGTERM` untuk mematikan proses tersebut. Jika berhasil, file PID dihapus dan pesan keberhasilan ditampilkan.

---
**Fail**
```c
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
```
Fungsi ini menjalankan mode fail-safe, yaitu daemon yang aktif memantau proses milik user target, dan jika menemukan proses yang bukan bagian dari whitelist (seperti bash, sh, sudo, zsh, revert_daemon), maka proses tersebut akan langsung dihentikan menggunakan `SIGKILL`. Setiap eksekusi proses yang dibunuh akan dicatat di log dengan format timestamp_KILLED_nama. Daemon ini juga menyimpan PID-nya di `/tmp/debugmon-fail-<username>.pid`.

---
**Revert**
```c
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
```
Fungsi ini menghentikan mode `fail` dengan membaca file PID dari `/tmp/debugmon-fail-<username>.pid` dan mengirimkan `SIGTERM` ke daemon tersebut. Fungsi ini hanya bisa dijalankan oleh root (diperiksa dengan `getuid()`), dan jika berhasil, mencatat `REVERT_SUCCESS` ke log. Jika gagal, pesan error akan ditampilkan.

---
**Main**
```c
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
```
- Fungsi ini memproses argumen dari command line untuk menentukan mode operasi program.
- Memastikan jumlah argumen tepat 3 (program + mode + username), jika tidak, tampilkan panduan penggunaan.
- argv[1] menentukan mode: bisa list, daemon, stop, fail, atau revert.
- argv[2] adalah nama user yang prosesnya akan dimonitor.
- Setiap mode memanggil fungsi berbeda:
    - list → tampilkan proses milik user tersebut.
    - daemon → jalankan monitoring daemon di background.
    - stop → hentikan daemon monitoring.
    - fail → jalankan mode pemusnah proses non-whitelist.
    - revert → hentikan mode fail (hanya bisa oleh root).
- Jika mode tidak dikenali, tampilkan pesan error dan keluar dengan status 1.
  
---
**Revisi soal_4**
Seharusnya ketika mode daemon di panggil mode tersebut akan membuat sebuah file log yang berisi proses yang akan terus diupdate selang beberapa detik, tetapi file log tidak muncul
![alt text](https://github.com/DimasSatya446/GambarSISOP2/blob/main/Screenshot%202025-04-17%20230757.png?raw=true)

