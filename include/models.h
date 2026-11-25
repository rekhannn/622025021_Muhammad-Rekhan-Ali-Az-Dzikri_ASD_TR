#ifndef MODELS_H
#define MODELS_H

#include "config.h"

typedef struct {
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];
} User;

typedef struct {
    int id;
    char kode[20];
    char nama[MAX_NAMA_BARANG];
    char kategori[MAX_KATEGORI];
    double harga;
    int stok;
} Barang;

typedef struct {
    int id_barang;
    char kode_barang[20];
    char nama_barang[MAX_NAMA_BARANG];
    int jumlah;
    double harga_satuan;
    double subtotal;
} ItemKeranjang;

typedef struct {
    int id_transaksi;
    char kasir[MAX_USERNAME];
    char nama_pelanggan[MAX_NAMA_BARANG];
    int jumlah_item;
    double total_bayar;
} Transaksi;

/* Stack untuk fitur Undo item di keranjang kasir */
typedef struct {
    ItemKeranjang items[MAX_STACK_ITEM];
    int top;
} StackKeranjang;

/* Queue untuk antrean pelanggan di kasir */
typedef struct {
    char pelanggan[MAX_QUEUE_PELANGGAN][MAX_NAMA_BARANG];
    int front;
    int rear;
    int count;
} QueuePelanggan;

/* Hash Table dengan chaining (linked list) berdasarkan kode barang unik */
typedef struct HashNode {
    char key_kode[20];
    Barang data;
    struct HashNode *next;
} HashNode;

typedef struct {
    HashNode *buckets[HASH_TABLE_SIZE];
} HashTableBarang;

/* Binary Search Tree (BST) berdasarkan ID barang */
typedef struct BSTNode {
    Barang data;
    struct BSTNode *left;
    struct BSTNode *right;
} BSTNode;

/* Graph relasi antar barang / rak di toko (Adjacency Matrix) */
typedef struct {
    int num_nodes;
    int node_ids[MAX_GRAPH_NODES];
    char node_labels[MAX_GRAPH_NODES][MAX_NAMA_BARANG];
    int adj[MAX_GRAPH_NODES][MAX_GRAPH_NODES];
} GraphToko;

#endif