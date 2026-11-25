#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "modules.h"

static void menu_manajemen_barang(
    Barang **daftar_barang,
    int *jumlah_barang,
    int *kapasitas_barang,
    HashTableBarang *ht,
    BSTNode **bst_root
) {
    int pilih;
    while (1) {
        printf("\n=== MENU MANAJEMEN BARANG (SORTING & SEARCHING) ===\n");
        printf("1. Tampilkan Semua Barang\n");
        printf("2. Tambah Barang Baru\n");
        printf("3. Urutkan Harga - Bubble Sort (Ascending / Termurah)\n");
        printf("4. Urutkan Harga - Bubble Sort (Descending / Termahal)\n");
        printf("5. Urutkan Nama  - Selection Sort (Ascending / A-Z)\n");
        printf("6. Urutkan Nama  - Selection Sort (Descending / Z-A)\n");
        printf("7. Cari Nama Barang (Sequential Search)\n");
        printf("8. Cari ID Barang (Binary Search)\n");
        printf("0. Kembali ke Menu Utama\n");

        if (!read_int_safe("Pilih menu: ", &pilih)) {
            printf("[!] Input tidak valid.\n");
            continue;
        }

        if (pilih == 1) {
            tampilkan_daftar_barang(*daftar_barang, *jumlah_barang);
        } else if (pilih == 2) {
            Barang baru;
            if (tambah_barang(daftar_barang, jumlah_barang, kapasitas_barang, &baru)) {
                hash_insert_or_update(ht, baru);
                *bst_root = bst_insert(*bst_root, baru);
            }
        } else if (pilih == 3) {
            sort_barang_bubble_harga(*daftar_barang, *jumlah_barang, 1);
            printf("[+] Diurutkan berdasarkan Harga (Ascending).\n");
            tampilkan_daftar_barang(*daftar_barang, *jumlah_barang);
        } else if (pilih == 4) {
            sort_barang_bubble_harga(*daftar_barang, *jumlah_barang, 0);
            printf("[+] Diurutkan berdasarkan Harga (Descending).\n");
            tampilkan_daftar_barang(*daftar_barang, *jumlah_barang);
        } else if (pilih == 5) {
            sort_barang_selection_nama(*daftar_barang, *jumlah_barang, 1);
            printf("[+] Diurutkan berdasarkan Nama (Ascending).\n");
            tampilkan_daftar_barang(*daftar_barang, *jumlah_barang);
        } else if (pilih == 6) {
            sort_barang_selection_nama(*daftar_barang, *jumlah_barang, 0);
            printf("[+] Diurutkan berdasarkan Nama (Descending).\n");
            tampilkan_daftar_barang(*daftar_barang, *jumlah_barang);
        } else if (pilih == 7) {
            char keyword[MAX_NAMA_BARANG];
            printf("Masukkan kata kunci nama barang: ");
            if (read_line_safe(keyword, sizeof(keyword)) && !is_blank_or_whitespace(keyword)) {
                sequential_search_barang_nama(*daftar_barang, *jumlah_barang, keyword);
            } else {
                printf("[!] Kata kunci tidak boleh kosong.\n");
            }
        } else if (pilih == 8) {
            int target_id;
            Barang hasil;
            if (!read_int_safe("Masukkan ID Barang yang dicari: ", &target_id)) {
                printf("[!] ID harus berupa angka.\n");
                continue;
            }
            if (binary_search_barang_id(*daftar_barang, *jumlah_barang, target_id, &hasil)) {
                printf("[+] Ditemukan via Binary Search: [%d | %s] %s - Rp%.2f (Stok: %d)\n",
                       hasil.id, hasil.kode, hasil.nama, hasil.harga, hasil.stok);
            } else {
                printf("[!] Barang dengan ID %d tidak ditemukan.\n", target_id);
            }
        } else if (pilih == 0) {
            break;
        } else {
            printf("[!] Pilihan menu tidak tersedia.\n");
        }
    }
}

static void menu_analisis_struktur_data(
    HashTableBarang *ht,
    BSTNode **bst_root,
    GraphToko *graph,
    Barang *daftar_barang,
    int jumlah_barang
) {
    int pilih;
    while (1) {
        printf("\n=== MENU ANALISIS TOKO (HASH TABLE, BST & GRAPH) ===\n");
        printf("1. [Hash Table] Cari Cepat Barang via Kode Unik\n");
        printf("2. [Hash Table] Lihat Tabel Hash & Collision Chaining\n");
        printf("3. [BST] Cari Barang via ID di Binary Search Tree\n");
        printf("4. [BST] Hapus Node Barang dari BST\n");
        printf("5. [BST] Traversal Pre-Order, In-Order, Post-Order\n");
        printf("6. [Graph] Lihat Relasi Produk & Tambah Relasi Baru\n");
        printf("7. [Graph] Penelusuran BFS & DFS Rekomendasi Produk\n");
        printf("0. Kembali ke Menu Utama\n");

        if (!read_int_safe("Pilih menu: ", &pilih)) {
            printf("[!] Input tidak valid.\n");
            continue;
        }

        if (pilih == 1) {
            char kode[MAX_KODE];
            HashNode *found;
            rebuild_hash_table(ht, daftar_barang, jumlah_barang);
            printf("Masukkan Kode Barang (mis. BRG01): ");
            if (!read_line_safe(kode, sizeof(kode)) || is_blank_or_whitespace(kode)) {
                printf("[!] Kode tidak boleh kosong.\n");
                continue;
            }
            found = hash_search_barang(ht, kode);
            if (found != NULL) {
                printf("[+] Ditemukan (Bucket %u): %s | Harga: Rp%.2f | Stok: %d\n",
                       hash_function_kode(kode), found->data.nama,
                       found->data.harga, found->data.stok);
            } else {
                printf("[!] Kode barang '%s' tidak ditemukan di Hash Table.\n", kode);
            }
        } else if (pilih == 2) {
            rebuild_hash_table(ht, daftar_barang, jumlah_barang);
            tampilkan_hash_table(ht);
        } else if (pilih == 3) {
            int id_cari;
            BSTNode *found;
            if (!read_int_safe("Masukkan ID Barang di BST: ", &id_cari)) {
                printf("[!] ID tidak valid.\n");
                continue;
            }
            found = bst_search(*bst_root, id_cari);
            if (found != NULL) {
                printf("[+] Node BST ditemukan: [ID %d] %s | Stok: %d | Rp%.2f\n",
                       found->data.id, found->data.nama, found->data.stok, found->data.harga);
            } else {
                printf("[!] ID %d tidak ditemukan di dalam BST.\n", id_cari);
            }
        } else if (pilih == 4) {
            int id_hapus, deleted = 0;
            if (!read_int_safe("Masukkan ID Barang yang akan dihapus dari BST: ", &id_hapus)) {
                printf("[!] ID tidak valid.\n");
                continue;
            }
            *bst_root = bst_delete(*bst_root, id_hapus, &deleted);
            if (deleted) {
                printf("[+] Node ID %d berhasil dihapus dari struktur BST.\n", id_hapus);
            } else {
                printf("[!] Node ID %d tidak ditemukan di BST.\n", id_hapus);
            }
        } else if (pilih == 5) {
            printf("\n--- BST TRAVERSAL ---\n");
            printf("Pre-Order  : ");
            bst_preorder(*bst_root);
            printf("\nIn-Order   : ");
            bst_inorder(*bst_root);
            printf("\nPost-Order : ");
            bst_postorder(*bst_root);
            printf("\n");
        } else if (pilih == 6) {
            int a, b;
            tampilkan_graph(graph);
            printf("\nIngin menambah relasi baru? Masukkan dua indeks simpul (atau -1 untuk batal).\n");
            if (read_int_safe("Indeks simpul pertama : ", &a) && a >= 0) {
                if (read_int_safe("Indeks simpul kedua   : ", &b) && b >= 0) {
                    tambah_relasi_graph(graph, a, b);
                    save_graph_ke_file(FILE_GRAPH, graph);
                    printf("[+] Relasi berhasil ditambahkan dan disimpan!\n");
                }
            }
        } else if (pilih == 7) {
            int start_idx;
            tampilkan_graph(graph);
            if (!read_int_safe("\nMasukkan indeks simpul awal BFS & DFS: ", &start_idx)) {
                printf("[!] Indeks tidak valid.\n");
                continue;
            }
            bfs_graph(graph, start_idx);
            dfs_graph(graph, start_idx);
        } else if (pilih == 0) {
            break;
        } else {
            printf("[!] Pilihan menu tidak tersedia.\n");
        }
    }
}

int main(void) {
    Barang *daftar_barang = NULL;
    int jumlah_barang = 0, kapasitas_barang = 0;

    Transaksi *riwayat_trx = NULL;
    int jumlah_trx = 0, kapasitas_trx = 0;

    QueuePelanggan antrean_kasir;
    HashTableBarang hash_barang;
    BSTNode *bst_root = NULL;
    GraphToko graph_toko;

    char current_user[MAX_USERNAME];
    int is_logged_in = 0;
    int i;

    ensure_data_dir();

    init_daftar_barang(&daftar_barang, &jumlah_barang, &kapasitas_barang);
    load_barang_dari_file(FILE_BARANG, &daftar_barang, &jumlah_barang, &kapasitas_barang);

    init_riwayat_transaksi(&riwayat_trx, &jumlah_trx, &kapasitas_trx);
    load_transaksi_dari_file(FILE_TRANSAKSI, &riwayat_trx, &jumlah_trx, &kapasitas_trx);

    init_queue_pelanggan(&antrean_kasir);
    init_hash_table(&hash_barang);
    for (i = 0; i < jumlah_barang; i++) {
        hash_insert_or_update(&hash_barang, daftar_barang[i]);
        bst_root = bst_insert(bst_root, daftar_barang[i]);
    }

    load_graph_dari_file(FILE_GRAPH, &graph_toko, daftar_barang, jumlah_barang);

    while (1) {
        if (!is_logged_in) {
            int pilih_auth;
            printf("\n====================================================\n");
            printf("   SISTEM KASIR & MANAJEMEN TOKO SEDERHANA (ASD)    \n");
            printf("====================================================\n");
            printf("1. Login\n");
            printf("2. Register Akun Baru\n");
            printf("0. Keluar Program\n");
            if (!read_int_safe("Pilih menu: ", &pilih_auth)) {
                printf("[!] Input tidak valid. Masukkan angka 0-2.\n");
                continue;
            }

            if (pilih_auth == 1) {
                if (login_user(FILE_USERS, current_user, sizeof(current_user))) {
                    is_logged_in = 1;
                }
            } else if (pilih_auth == 2) {
                register_user(FILE_USERS);
            } else if (pilih_auth == 0) {
                printf("Terima kasih telah menggunakan aplikasi. Sampai jumpa!\n");
                break;
            } else {
                printf("[!] Pilihan tidak tersedia.\n");
            }
        } else {
            int pilih_main;
            printf("\n====================================================\n");
            printf(" MENU UTAMA | Kasir Aktif: %-24s \n", current_user);
            printf("====================================================\n");
            printf("1. Manajemen Barang (Lihat, Tambah, Sort, Search)\n");
            printf("2. Tambah Antrean Pelanggan Kasir (Queue Enqueue)\n");
            printf("3. Lihat Antrean Pelanggan Saat Ini\n");
            printf("4. Proses Transaksi Kasir (Queue + Stack Undo)\n");
            printf("5. Riwayat Transaksi & Laporan Omzet\n");
            printf("6. Analisis Struktur Data (Hash Table, BST, Graph)\n");
            printf("9. Logout\n");
            printf("0. Keluar Program\n");

            if (!read_int_safe("Pilih menu: ", &pilih_main)) {
                printf("[!] Input tidak valid. Masukkan angka yang tersedia.\n");
                continue;
            }

            if (pilih_main == 1) {
                menu_manajemen_barang(&daftar_barang, &jumlah_barang, &kapasitas_barang,
                                      &hash_barang, &bst_root);
            } else if (pilih_main == 2) {
                char nama_plg[MAX_NAMA_BARANG];
                printf("Masukkan nama pelanggan yang mengantre: ");
                if (!read_line_safe(nama_plg, sizeof(nama_plg)) || is_blank_or_whitespace(nama_plg)) {
                    printf("[!] Nama pelanggan tidak boleh kosong.\n");
                } else if (enqueue_pelanggan(&antrean_kasir, nama_plg)) {
                    printf("[+] '%s' berhasil masuk ke antrean kasir.\n", nama_plg);
                } else {
                    printf("[!] Antrean penuh.\n");
                }
            } else if (pilih_main == 3) {
                tampilkan_antrean(&antrean_kasir);
            } else if (pilih_main == 4) {
                proses_transaksi_kasir(current_user, daftar_barang, jumlah_barang,
                                       &antrean_kasir, &riwayat_trx, &jumlah_trx, &kapasitas_trx);
            } else if (pilih_main == 5) {
                tampilkan_riwayat_transaksi(riwayat_trx, jumlah_trx);
            } else if (pilih_main == 6) {
                menu_analisis_struktur_data(&hash_barang, &bst_root, &graph_toko,
                                              daftar_barang, jumlah_barang);
            } else if (pilih_main == 9) {
                is_logged_in = 0;
                current_user[0] = '\0';
                printf("[+] Anda berhasil logout.\n");
            } else if (pilih_main == 0) {
                printf("Menyimpan data dan menutup aplikasi...\n");
                break;
            } else {
                printf("[!] Pilihan tidak tersedia.\n");
            }
        }
    }

    save_barang_ke_file(FILE_BARANG, daftar_barang, jumlah_barang);
    save_transaksi_ke_file(FILE_TRANSAKSI, riwayat_trx, jumlah_trx);
    free_daftar_barang(&daftar_barang, &jumlah_barang, &kapasitas_barang);
    if (riwayat_trx != NULL) {
        free(riwayat_trx);
        riwayat_trx = NULL;
    }
    free_hash_table(&hash_barang);
    free_bst(bst_root);

    return 0;
}