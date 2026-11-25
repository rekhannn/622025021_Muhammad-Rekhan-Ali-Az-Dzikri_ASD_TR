#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "modules.h"

static int expand_daftar_barang(Barang **list, int *capacity) {
    int new_cap = (*capacity) * 2;
    Barang *temp = (Barang *)realloc(*list, (size_t)new_cap * sizeof(Barang));
    if (temp == NULL) {
        return 0;
    }
    *list = temp;
    *capacity = new_cap;
    return 1;
}

static void seed_default_barang(Barang **list, int *count, int *capacity) {
    Barang defaults[] = {
        {101, "BRG01", "Beras Premium 5kg", "Sembako", 68000.0, 25},
        {102, "BRG02", "Minyak Goreng 2L", "Sembako", 34000.0, 40},
        {103, "BRG03", "Gula Pasir 1kg", "Sembako", 16500.0, 50},
        {104, "BRG04", "Kopi Bubuk 200g", "Minuman", 18000.0, 30},
        {105, "BRG05", "Teh Celup Kotak", "Minuman", 8500.0, 45},
        {106, "BRG06", "Sabun Mandi Cair", "Kebersihan", 22000.0, 20}
    };
    int n = (int)(sizeof(defaults) / sizeof(defaults[0]));
    int i;
    for (i = 0; i < n; i++) {
        if (*count >= *capacity) {
            expand_daftar_barang(list, capacity);
        }
        (*list)[*count] = defaults[i];
        (*count)++;
    }
}

static int cari_index_barang_by_kode(const Barang *list, int count, const char *kode) {
    int i;
    for (i = 0; i < count; i++) {
        if (strcmp(list[i].kode, kode) == 0) {
            return i;
        }
    }
    return -1;
}

static void sort_barang_insertion_id_asc(Barang *list, int count) {
    int i, j;
    Barang key;
    for (i = 1; i < count; i++) {
        key = list[i];
        j = i - 1;
        while (j >= 0 && list[j].id > key.id) {
            list[j + 1] = list[j];
            j--;
        }
        list[j + 1] = key;
    }
}

void init_daftar_barang(Barang **list, int *count, int *capacity) {
    *count = 0;
    *capacity = INITIAL_BARANG_CAP;
    *list = (Barang *)malloc((size_t)(*capacity) * sizeof(Barang));
    if (*list == NULL) {
        printf("[!] Gagal mengalokasikan memori awal barang.\n");
        exit(1);
    }
}

void free_daftar_barang(Barang **list, int *count, int *capacity) {
    if (list != NULL && *list != NULL) {
        free(*list);
        *list = NULL;
    }
    if (count != NULL) {
        *count = 0;
    }
    if (capacity != NULL) {
        *capacity = 0;
    }
}

int save_barang_ke_file(const char *filename, const Barang *list, int count) {
    FILE *fp = fopen(filename, "w");
    int i;
    if (fp == NULL) {
        return 0;
    }
    for (i = 0; i < count; i++) {
        fprintf(fp, "%d|%s|%s|%s|%.2f|%d\n",
                list[i].id,
                list[i].kode,
                list[i].nama,
                list[i].kategori,
                list[i].harga,
                list[i].stok);
    }
    fclose(fp);
    return 1;
}

int load_barang_dari_file(const char *filename, Barang **list, int *count, int *capacity) {
    FILE *fp = fopen(filename, "r");
    char line[MAX_LINE];
    Barang b;

    if (fp == NULL) {
        seed_default_barang(list, count, capacity);
        save_barang_ke_file(filename, *list, *count);
        return 1;
    }

    while (fgets(line, sizeof(line), fp) != NULL) {
        trim_newline(line);
        if (sscanf(line, "%d|%19[^|]|%63[^|]|%31[^|]|%lf|%d",
                   &b.id, b.kode, b.nama, b.kategori, &b.harga, &b.stok) == 6) {
            if (*count >= *capacity) {
                if (!expand_daftar_barang(list, capacity)) {
                    fclose(fp);
                    return 0;
                }
            }
            (*list)[*count] = b;
            (*count)++;
        }
    }
    fclose(fp);
    return 1;
}

int cari_index_barang_by_id(const Barang *list, int count, int id) {
    int i;
    for (i = 0; i < count; i++) {
        if (list[i].id == id) {
            return i;
        }
    }
    return -1;
}

void tampilkan_daftar_barang(const Barang *list, int count) {
    int i;
    printf("\n+------+----------+------------------------------+---------------+--------------+-------+\n");
    printf("| %-4s | %-8s | %-28s | %-13s | %-12s | %-5s |\n",
           "ID", "KODE", "NAMA BARANG", "KATEGORI", "HARGA (Rp)", "STOK");
    printf("+------+----------+------------------------------+---------------+--------------+-------+\n");
    if (count == 0) {
        printf("| %-83s |\n", "Belum ada data barang.");
    } else {
        for (i = 0; i < count; i++) {
            printf("| %-4d | %-8s | %-28s | %-13s | %12.2f | %-5d |\n",
                   list[i].id,
                   list[i].kode,
                   list[i].nama,
                   list[i].kategori,
                   list[i].harga,
                   list[i].stok);
        }
    }
    printf("+------+----------+------------------------------+---------------+--------------+-------+\n");
}

int tambah_barang(Barang **list, int *count, int *capacity, Barang *out_barang_baru) {
    Barang b;
    printf("\n=== TAMBAH DATA BARANG ===\n");
    if (!read_int_safe("ID Barang (angka unik, mis. 107) : ", &b.id) || b.id <= 0) {
        printf("[!] ID harus berupa angka positif.\n");
        return 0;
    }
    if (cari_index_barang_by_id(*list, *count, b.id) != -1) {
        printf("[!] ID Barang %d sudah terdaftar.\n", b.id);
        return 0;
    }

    printf("Kode Barang (mis. BRG07)         : ");
    if (!read_line_safe(b.kode, sizeof(b.kode)) || is_blank_or_whitespace(b.kode)) {
        printf("[!] Kode barang tidak boleh kosong.\n");
        return 0;
    }
    if (cari_index_barang_by_kode(*list, *count, b.kode) != -1) {
        printf("[!] Kode Barang '%s' sudah digunakan.\n", b.kode);
        return 0;
    }

    printf("Nama Barang                      : ");
    if (!read_line_safe(b.nama, sizeof(b.nama)) || is_blank_or_whitespace(b.nama)) {
        printf("[!] Nama barang tidak boleh kosong.\n");
        return 0;
    }

    printf("Kategori                         : ");
    if (!read_line_safe(b.kategori, sizeof(b.kategori)) || is_blank_or_whitespace(b.kategori)) {
        printf("[!] Kategori tidak boleh kosong.\n");
        return 0;
    }

    if (!read_double_safe("Harga Satuan (Rp)                : ", &b.harga) || b.harga < 0) {
        printf("[!] Harga tidak valid.\n");
        return 0;
    }

    if (!read_int_safe("Stok Awal                        : ", &b.stok) || b.stok < 0) {
        printf("[!] Stok tidak valid.\n");
        return 0;
    }

    if (*count >= *capacity) {
        if (!expand_daftar_barang(list, capacity)) {
            printf("[!] Gagal menambah kapasitas memori.\n");
            return 0;
        }
    }

    (*list)[*count] = b;
    (*count)++;
    if (out_barang_baru != NULL) {
        *out_barang_baru = b;
    }
    save_barang_ke_file(FILE_BARANG, *list, *count);
    printf("[+] Barang '%s' berhasil ditambahkan dan disimpan!\n", b.nama);
    return 1;
}

void sort_barang_bubble_harga(Barang *list, int count, int ascending) {
    int i, j;
    Barang temp;
    for (i = 0; i < count - 1; i++) {
        for (j = 0; j < count - i - 1; j++) {
            int harus_tukar = ascending ? (list[j].harga > list[j + 1].harga)
                                        : (list[j].harga < list[j + 1].harga);
            if (harus_tukar) {
                temp = list[j];
                list[j] = list[j + 1];
                list[j + 1] = temp;
            }
        }
    }
}

void sort_barang_selection_nama(Barang *list, int count, int ascending) {
    int i, j, target_idx;
    Barang temp;
    for (i = 0; i < count - 1; i++) {
        target_idx = i;
        for (j = i + 1; j < count; j++) {
            int cmp = strcmp(list[j].nama, list[target_idx].nama);
            if ((ascending && cmp < 0) || (!ascending && cmp > 0)) {
                target_idx = j;
            }
        }
        if (target_idx != i) {
            temp = list[i];
            list[i] = list[target_idx];
            list[target_idx] = temp;
        }
    }
}

void sequential_search_barang_nama(const Barang *list, int count, const char *keyword) {
    char key_low[MAX_NAMA_BARANG];
    char nama_low[MAX_NAMA_BARANG];
    int i, found = 0;

    str_to_lower_copy(key_low, keyword, sizeof(key_low));
    printf("\nHasil Sequential Search untuk kata kunci \"%s\":\n", keyword);
    for (i = 0; i < count; i++) {
        str_to_lower_copy(nama_low, list[i].nama, sizeof(nama_low));
        if (strstr(nama_low, key_low) != NULL) {
            printf(" - [ID: %d | Kode: %s] %s | Kategori: %s | Rp%.2f | Stok: %d\n",
                   list[i].id, list[i].kode, list[i].nama,
                   list[i].kategori, list[i].harga, list[i].stok);
            found++;
        }
    }
    if (found == 0) {
        printf("[!] Data barang dengan nama \"%s\" tidak ditemukan.\n", keyword);
    }
}

int binary_search_barang_id(const Barang *list, int count, int target_id, Barang *out_found) {
    Barang *copy;
    int left = 0, right = count - 1;
    int result_idx = -1;

    if (count <= 0) {
        return 0;
    }
    copy = (Barang *)malloc((size_t)count * sizeof(Barang));
    if (copy == NULL) {
        return 0;
    }

    memcpy(copy, list, (size_t)count * sizeof(Barang));
    sort_barang_insertion_id_asc(copy, count);

    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (copy[mid].id == target_id) {
            result_idx = mid;
            break;
        } else if (copy[mid].id < target_id) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }

    if (result_idx != -1 && out_found != NULL) {
        *out_found = copy[result_idx];
    }
    free(copy);
    return (result_idx != -1);
}