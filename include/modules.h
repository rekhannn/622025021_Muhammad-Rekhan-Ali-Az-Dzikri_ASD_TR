#ifndef MODULES_H
#define MODULES_H

#include "models.h"

/* util.c */
void ensure_data_dir(void);
void clear_stdin_buffer(void);
void trim_newline(char *str);
int is_blank_or_whitespace(const char *str);
int read_line_safe(char *buffer, int size);
int read_int_safe(const char *prompt, int *out_val);
int read_double_safe(const char *prompt, double *out_val);
void str_to_lower_copy(char *dst, const char *src, size_t cap);

/* auth.c */
int user_exists(const char *filename, const char *username);
int register_user(const char *filename);
int login_user(const char *filename, char *logged_in_user, int user_cap);

/* barang.c */
void init_daftar_barang(Barang **list, int *count, int *capacity);
void free_daftar_barang(Barang **list, int *count, int *capacity);
int save_barang_ke_file(const char *filename, const Barang *list, int count);
int load_barang_dari_file(const char *filename, Barang **list, int *count, int *capacity);
int cari_index_barang_by_id(const Barang *list, int count, int id);
void tampilkan_daftar_barang(const Barang *list, int count);
int tambah_barang(Barang **list, int *count, int *capacity, Barang *out_barang_baru);
void sort_barang_bubble_harga(Barang *list, int count, int ascending);
void sort_barang_selection_nama(Barang *list, int count, int ascending);
void sequential_search_barang_nama(const Barang *list, int count, const char *keyword);
int binary_search_barang_id(const Barang *list, int count, int target_id, Barang *out_found);

/* kasir.c */
void init_queue_pelanggan(QueuePelanggan *q);
int enqueue_pelanggan(QueuePelanggan *q, const char *nama);
int dequeue_pelanggan(QueuePelanggan *q, char *out_nama, int cap);
void tampilkan_antrean(const QueuePelanggan *q);
void init_stack_keranjang(StackKeranjang *s);
int push_keranjang(StackKeranjang *s, ItemKeranjang item);
int pop_keranjang(StackKeranjang *s, ItemKeranjang *out_item);
void init_riwayat_transaksi(Transaksi **list, int *count, int *capacity);
int save_transaksi_ke_file(const char *filename, const Transaksi *list, int count);
int load_transaksi_dari_file(const char *filename, Transaksi **list, int *count, int *capacity);
void proses_transaksi_kasir(
    const char *nama_kasir,
    Barang *daftar_barang,
    int jumlah_barang,
    QueuePelanggan *antrean,
    Transaksi **riwayat_trx,
    int *jumlah_trx,
    int *kapasitas_trx
);
void tampilkan_riwayat_transaksi(const Transaksi *list, int count);

/* struktur_data.c */
void init_hash_table(HashTableBarang *ht);
unsigned int hash_function_kode(const char *kode);
void hash_insert_or_update(HashTableBarang *ht, Barang b);
HashNode *hash_search_barang(const HashTableBarang *ht, const char *kode);
void tampilkan_hash_table(const HashTableBarang *ht);
void free_hash_table(HashTableBarang *ht);
void rebuild_hash_table(HashTableBarang *ht, const Barang *list, int count);

BSTNode *bst_insert(BSTNode *root, Barang b);
BSTNode *bst_search(BSTNode *root, int id);
BSTNode *bst_find_min(BSTNode *node);
BSTNode *bst_delete(BSTNode *root, int id, int *deleted);
void bst_preorder(BSTNode *root);
void bst_inorder(BSTNode *root);
void bst_postorder(BSTNode *root);
void free_bst(BSTNode *root);

void init_graph_toko(GraphToko *g);
void tambah_relasi_graph(GraphToko *g, int idx_a, int idx_b);
int save_graph_ke_file(const char *filename, const GraphToko *g);
int load_graph_dari_file(const char *filename, GraphToko *g, const Barang *list, int count);
void tampilkan_graph(const GraphToko *g);
void bfs_graph(const GraphToko *g, int start_idx);
void dfs_graph(const GraphToko *g, int start_idx);

#endif