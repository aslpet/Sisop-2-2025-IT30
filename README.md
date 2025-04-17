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

<div align=center>
	
## Soal_2
</div>


<div align=center>
	
## Soal_3
</div>


<div align=center>
	
## Soal_4
</div>
