#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "modules.h"

void init_queue_pelanggan(QueuePelanggan *q) {
    q->front = 0;
    q->rear = -1;
    q->count = 0;
}

int enqueue_pelanggan(QueuePelanggan *q, const char *nama) {
    if (q->count >= MAX_QUEUE_PELANGGAN) {
        return 0;
    }
    q->rear = (q->rear + 1) % MAX_QUEUE_PELANGGAN;
    strncpy(q->pelanggan[q->rear], nama, MAX_NAMA_BARANG - 1);
    q->pelanggan[q->rear][MAX_NAMA_BARANG - 1] = '\0';
    q->count++;
    return 1;
}

int dequeue_pelanggan(QueuePelanggan *q, char *out_nama, int cap) {
    if (q->count <= 0) {
        return 0;
    }
    if (out_nama != NULL && cap > 0) {
        strncpy(out_nama, q->pelanggan[q->front], (size_t)cap - 1);
        out_nama[cap - 1] = '\0';
    }
    q->front = (q->front + 1) % MAX_QUEUE_PELANGGAN;
    q->count--;
    return 1;
}

void tampilkan_antrean(const QueuePelanggan *q) {
    int i;
    printf("\n=== ANTREAN PELANGGAN KASIR (QUEUE - FIFO) ===\n");
    if (q->count == 0) {
        printf("Antrean saat ini kosong.\n");
        return;
    }
    for (i = 0; i < q->count; i++) {
        int idx = (q->front + i) % MAX_QUEUE_PELANGGAN;
        printf("%d. %s%s\n", i + 1, q->pelanggan[idx], (i == 0) ? " <-- Berikutnya dilayani" : "");
    }
}

void init_stack_keranjang(StackKeranjang *s) {
    s->top = -1;
}

int push_keranjang(StackKeranjang *s, ItemKeranjang item) {
    if (s->top >= MAX_STACK_ITEM - 1) {
        return 0;
    }
    s->top++;
    s->items[s->top] = item;
    return 1;
}

int pop_keranjang(StackKeranjang *s, ItemKeranjang *out_item) {
    if (s->top < 0) {
        return 0;
    }
    if (out_item != NULL) {
        *out_item = s->items[s->top];
    }
    s->top--;
    return 1;
}

void init_riwayat_transaksi(Transaksi **list, int *count, int *capacity) {
    *count = 0;
    *capacity = INITIAL_TRANSAKSI_CAP;
    *list = (Transaksi *)malloc((size_t)(*capacity) * sizeof(Transaksi));
    if (*list == NULL) {
        printf("[!] Gagal mengalokasikan memori riwayat transaksi.\n");
        exit(1);
    }
}

int save_transaksi_ke_file(const char *filename, const Transaksi *list, int count) {
    FILE *fp = fopen(filename, "w");
    int i;
    if (fp == NULL) {
        return 0;
    }
    for (i = 0; i < count; i++) {
        fprintf(fp, "%d|%s|%s|%d|%.2f\n",
                list[i].id_transaksi,
                list[i].kasir,
                list[i].nama_pelanggan,
                list[i].jumlah_item,
                list[i].total_bayar);
    }
    fclose(fp);
    return 1;
}

int load_transaksi_dari_file(const char *filename, Transaksi **list, int *count, int *capacity) {
    FILE *fp = fopen(filename, "r");
    char line[MAX_LINE];
    Transaksi t;

    if (fp == NULL) {
        return 1;
    }
    while (fgets(line, sizeof(line), fp) != NULL) {
        trim_newline(line);
        if (sscanf(line, "%d|%31[^|]|%63[^|]|%d|%lf",
                   &t.id_transaksi, t.kasir, t.nama_pelanggan,
                   &t.jumlah_item, &t.total_bayar) == 5) {
            if (*count >= *capacity) {
                int new_cap = (*capacity) * 2;
                Transaksi *tmp = (Transaksi *)realloc(*list, (size_t)new_cap * sizeof(Transaksi));
                if (tmp == NULL) {
                    fclose(fp);
                    return 0;
                }
                *list = tmp;
                *capacity = new_cap;
            }
            (*list)[*count] = t;
            (*count)++;
        }
    }
    fclose(fp);
    return 1;
}

void proses_transaksi_kasir(
    const char *nama_kasir,
    Barang *daftar_barang,
    int jumlah_barang,
    QueuePelanggan *antrean,
    Transaksi **riwayat_trx,
    int *jumlah_trx,
    int *kapasitas_trx
) {
    char pelanggan_aktif[MAX_NAMA_BARANG];
    StackKeranjang keranjang;
    int pilihan;

    if (jumlah_barang <= 0) {
        printf("[!] Data barang kosong. Tambahkan barang terlebih dahulu.\n");
        return;
    }

    if (!dequeue_pelanggan(antrean, pelanggan_aktif, sizeof(pelanggan_aktif))) {
        printf("\nAntrean kasir kosong. Masukkan nama pelanggan langsung: ");
        if (!read_line_safe(pelanggan_aktif, sizeof(pelanggan_aktif)) ||
            is_blank_or_whitespace(pelanggan_aktif)) {
            strcpy(pelanggan_aktif, "Pelanggan Umum");
        }
    } else {
        printf("\n[+] Memanggil pelanggan dari antrean (Queue): %s\n", pelanggan_aktif);
    }

    init_stack_keranjang(&keranjang);

    while (1) {
        int i;
        double total_sementara = 0.0;
        printf("\n--- KERANJANG BELANJA [%s] (Kasir: %s) ---\n", pelanggan_aktif, nama_kasir);
        if (keranjang.top < 0) {
            printf("(Keranjang masih kosong)\n");
        } else {
            for (i = 0; i <= keranjang.top; i++) {
                printf("%d. %s (x%d) @ Rp%.2f = Rp%.2f\n",
                       i + 1,
                       keranjang.items[i].nama_barang,
                       keranjang.items[i].jumlah,
                       keranjang.items[i].harga_satuan,
                       keranjang.items[i].subtotal);
                total_sementara += keranjang.items[i].subtotal;
            }
            printf("Total Sementara: Rp%.2f\n", total_sementara);
        }

        printf("\n1. Tambah Item ke Keranjang (Push Stack)\n");
        printf("2. Batalkan Item Terakhir / Undo (Pop Stack)\n");
        printf("3. Selesaikan Pembayaran & Cetak Struk\n");
        printf("0. Batalkan Seluruh Transaksi\n");
        if (!read_int_safe("Pilih aksi kasir: ", &pilihan)) {
            printf("[!] Input tidak valid.\n");
            continue;
        }

        if (pilihan == 1) {
            int id_cari, qty, idx_brg;
            ItemKeranjang item;

            tampilkan_daftar_barang(daftar_barang, jumlah_barang);
            if (!read_int_safe("Masukkan ID Barang yang dibeli : ", &id_cari)) {
                printf("[!] ID tidak valid.\n");
                continue;
            }
            idx_brg = cari_index_barang_by_id(daftar_barang, jumlah_barang, id_cari);
            if (idx_brg == -1) {
                printf("[!] Barang dengan ID %d tidak ditemukan.\n", id_cari);
                continue;
            }
            if (!read_int_safe("Jumlah beli                    : ", &qty) || qty <= 0) {
                printf("[!] Jumlah beli harus lebih dari 0.\n");
                continue;
            }
            if (qty > daftar_barang[idx_brg].stok) {
                printf("[!] Stok tidak mencukupi (tersedia: %d).\n", daftar_barang[idx_brg].stok);
                continue;
            }

            item.id_barang = daftar_barang[idx_brg].id;
            strcpy(item.kode_barang, daftar_barang[idx_brg].kode);
            strcpy(item.nama_barang, daftar_barang[idx_brg].nama);
            item.jumlah = qty;
            item.harga_satuan = daftar_barang[idx_brg].harga;
            item.subtotal = daftar_barang[idx_brg].harga * qty;

            if (push_keranjang(&keranjang, item)) {
                daftar_barang[idx_brg].stok -= qty;
                printf("[+] '%s' x%d masuk keranjang.\n", item.nama_barang, qty);
            } else {
                printf("[!] Keranjang penuh!\n");
            }
        } else if (pilihan == 2) {
            ItemKeranjang batal;
            if (pop_keranjang(&keranjang, &batal)) {
                int idx_brg = cari_index_barang_by_id(daftar_barang, jumlah_barang, batal.id_barang);
                if (idx_brg != -1) {
                    daftar_barang[idx_brg].stok += batal.jumlah;
                }
                printf("[UNDO] Item terakhir '%s' (x%d) dibatalkan & stok dikembalikan.\n",
                       batal.nama_barang, batal.jumlah);
            } else {
                printf("[!] Keranjang kosong, tidak ada item untuk di-undo.\n");
            }
        } else if (pilihan == 3) {
            double tunai = 0.0, total = 0.0;
            int total_qty = 0, i_item;
            Transaksi trx_baru;

            if (keranjang.top < 0) {
                printf("[!] Keranjang masih kosong.\n");
                continue;
            }
            for (i_item = 0; i_item <= keranjang.top; i_item++) {
                total += keranjang.items[i_item].subtotal;
                total_qty += keranjang.items[i_item].jumlah;
            }
            printf("\nTotal Tagihan: Rp%.2f\n", total);
            if (!read_double_safe("Masukkan Uang Tunai (Rp): ", &tunai) || tunai < total) {
                printf("[!] Pembayaran gagal: Uang tunai kurang atau tidak valid.\n");
                continue;
            }

            trx_baru.id_transaksi = 1001 + (*jumlah_trx);
            strncpy(trx_baru.kasir, nama_kasir, MAX_USERNAME - 1);
            trx_baru.kasir[MAX_USERNAME - 1] = '\0';
            strncpy(trx_baru.nama_pelanggan, pelanggan_aktif, MAX_NAMA_BARANG - 1);
            trx_baru.nama_pelanggan[MAX_NAMA_BARANG - 1] = '\0';
            trx_baru.jumlah_item = total_qty;
            trx_baru.total_bayar = total;

            if (*jumlah_trx >= *kapasitas_trx) {
                int new_cap = (*kapasitas_trx) * 2;
                Transaksi *tmp = (Transaksi *)realloc(*riwayat_trx, (size_t)new_cap * sizeof(Transaksi));
                if (tmp != NULL) {
                    *riwayat_trx = tmp;
                    *kapasitas_trx = new_cap;
                }
            }
            if (*jumlah_trx < *kapasitas_trx) {
                (*riwayat_trx)[*jumlah_trx] = trx_baru;
                (*jumlah_trx)++;
            }

            save_barang_ke_file(FILE_BARANG, daftar_barang, jumlah_barang);
            save_transaksi_ke_file(FILE_TRANSAKSI, *riwayat_trx, *jumlah_trx);

            printf("\n================ STRUK PEMBAYARAN ================\n");
            printf("No. Transaksi : TRX-%d\n", trx_baru.id_transaksi);
            printf("Kasir         : %s\n", trx_baru.kasir);
            printf("Pelanggan     : %s\n", trx_baru.nama_pelanggan);
            printf("Total Item    : %d\n", trx_baru.jumlah_item);
            printf("Total Belanja : Rp%.2f\n", total);
            printf("Tunai         : Rp%.2f\n", tunai);
            printf("Kembalian     : Rp%.2f\n", tunai - total);
            printf("==================================================\n");
            break;
        } else if (pilihan == 0) {
            ItemKeranjang batal;
            while (pop_keranjang(&keranjang, &batal)) {
                int idx_brg = cari_index_barang_by_id(daftar_barang, jumlah_barang, batal.id_barang);
                if (idx_brg != -1) {
                    daftar_barang[idx_brg].stok += batal.jumlah;
                }
            }
            printf("[!] Transaksi dibatalkan dan seluruh stok keranjang dikembalikan.\n");
            break;
        } else {
            printf("[!] Pilihan tidak tersedia.\n");
        }
    }
}

void tampilkan_riwayat_transaksi(const Transaksi *list, int count) {
    int i;
    double omzet = 0.0;
    printf("\n=== RIWAYAT TRANSAKSI PENJUALAN ===\n");
    if (count == 0) {
        printf("Belum ada transaksi tercatat.\n");
        return;
    }
    for (i = 0; i < count; i++) {
        printf("TRX-%d | Kasir: %-10s | Pelanggan: %-18s | Item: %-3d | Total: Rp%.2f\n",
               list[i].id_transaksi,
               list[i].kasir,
               list[i].nama_pelanggan,
               list[i].jumlah_item,
               list[i].total_bayar);
        omzet += list[i].total_bayar;
    }
    printf("---------------------------------------------------------------------------\n");
    printf("Total Omzet Keseluruhan: Rp%.2f\n", omzet);
}