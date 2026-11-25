#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "modules.h"

static void build_default_graph(GraphToko *g, const Barang *list, int count) {
    int limit = (count < 6) ? count : 6;
    int i;
    init_graph_toko(g);
    g->num_nodes = limit;
    for (i = 0; i < limit; i++) {
        g->node_ids[i] = list[i].id;
        strncpy(g->node_labels[i], list[i].nama, MAX_NAMA_BARANG - 1);
        g->node_labels[i][MAX_NAMA_BARANG - 1] = '\0';
    }
    if (limit >= 6) {
        tambah_relasi_graph(g, 0, 1);
        tambah_relasi_graph(g, 1, 2);
        tambah_relasi_graph(g, 2, 3);
        tambah_relasi_graph(g, 2, 4);
        tambah_relasi_graph(g, 0, 5);
    }
    save_graph_ke_file(FILE_GRAPH, g);
}

static void dfs_recursive_helper(const GraphToko *g, int curr, int visited[]) {
    int neighbor;
    visited[curr] = 1;
    printf(" -> [%d] %s\n", curr, g->node_labels[curr]);
    for (neighbor = 0; neighbor < g->num_nodes; neighbor++) {
        if (g->adj[curr][neighbor] == 1 && !visited[neighbor]) {
            dfs_recursive_helper(g, neighbor, visited);
        }
    }
}

/* Hash Table */
void init_hash_table(HashTableBarang *ht) {
    int i;
    for (i = 0; i < HASH_TABLE_SIZE; i++) {
        ht->buckets[i] = NULL;
    }
}

unsigned int hash_function_kode(const char *kode) {
    unsigned int hash = 0;
    size_t i;
    for (i = 0; kode[i] != '\0'; i++) {
        hash = (hash * 31u) + (unsigned char)kode[i];
    }
    return hash % HASH_TABLE_SIZE;
}

void hash_insert_or_update(HashTableBarang *ht, Barang b) {
    unsigned int idx = hash_function_kode(b.kode);
    HashNode *curr = ht->buckets[idx];
    HashNode *baru;

    while (curr != NULL) {
        if (strcmp(curr->key_kode, b.kode) == 0) {
            curr->data = b;
            return;
        }
        curr = curr->next;
    }

    baru = (HashNode *)malloc(sizeof(HashNode));
    if (baru == NULL) {
        return;
    }
    strcpy(baru->key_kode, b.kode);
    baru->data = b;
    baru->next = ht->buckets[idx];
    ht->buckets[idx] = baru;
}

HashNode *hash_search_barang(const HashTableBarang *ht, const char *kode) {
    unsigned int idx = hash_function_kode(kode);
    HashNode *curr = ht->buckets[idx];
    while (curr != NULL) {
        if (strcmp(curr->key_kode, kode) == 0) {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}

void tampilkan_hash_table(const HashTableBarang *ht) {
    int i;
    printf("\n=== ISI HASH TABLE KODE BARANG (CHAINING COLLISION HANDLING) ===\n");
    for (i = 0; i < HASH_TABLE_SIZE; i++) {
        HashNode *curr = ht->buckets[i];
        if (curr != NULL) {
            printf("Bucket [%02d]: ", i);
            while (curr != NULL) {
                printf("[%s: %s (Rp%.0f)]", curr->key_kode, curr->data.nama, curr->data.harga);
                if (curr->next != NULL) {
                    printf(" -> ");
                }
                curr = curr->next;
            }
            printf("\n");
        }
    }
}

void free_hash_table(HashTableBarang *ht) {
    int i;
    for (i = 0; i < HASH_TABLE_SIZE; i++) {
        HashNode *curr = ht->buckets[i];
        while (curr != NULL) {
            HashNode *next = curr->next;
            free(curr);
            curr = next;
        }
        ht->buckets[i] = NULL;
    }
}

void rebuild_hash_table(HashTableBarang *ht, const Barang *list, int count) {
    int i;
    free_hash_table(ht);
    for (i = 0; i < count; i++) {
        hash_insert_or_update(ht, list[i]);
    }
}

/* Binary Search Tree */
BSTNode *bst_insert(BSTNode *root, Barang b) {
    if (root == NULL) {
        BSTNode *node = (BSTNode *)malloc(sizeof(BSTNode));
        if (node == NULL) {
            return NULL;
        }
        node->data = b;
        node->left = NULL;
        node->right = NULL;
        return node;
    }
    if (b.id < root->data.id) {
        root->left = bst_insert(root->left, b);
    } else if (b.id > root->data.id) {
        root->right = bst_insert(root->right, b);
    } else {
        root->data = b;
    }
    return root;
}

BSTNode *bst_search(BSTNode *root, int id) {
    if (root == NULL || root->data.id == id) {
        return root;
    }
    if (id < root->data.id) {
        return bst_search(root->left, id);
    }
    return bst_search(root->right, id);
}

BSTNode *bst_find_min(BSTNode *node) {
    BSTNode *curr = node;
    while (curr != NULL && curr->left != NULL) {
        curr = curr->left;
    }
    return curr;
}

BSTNode *bst_delete(BSTNode *root, int id, int *deleted) {
    if (root == NULL) {
        return NULL;
    }

    if (id < root->data.id) {
        root->left = bst_delete(root->left, id, deleted);
    } else if (id > root->data.id) {
        root->right = bst_delete(root->right, id, deleted);
    } else {
        *deleted = 1;
        if (root->left == NULL) {
            BSTNode *temp = root->right;
            free(root);
            return temp;
        } else if (root->right == NULL) {
            BSTNode *temp = root->left;
            free(root);
            return temp;
        } else {
            BSTNode *succ = bst_find_min(root->right);
            root->data = succ->data;
            root->right = bst_delete(root->right, succ->data.id, deleted);
        }
    }
    return root;
}

void bst_preorder(BSTNode *root) {
    if (root == NULL) {
        return;
    }
    printf("[%d: %s] ", root->data.id, root->data.nama);
    bst_preorder(root->left);
    bst_preorder(root->right);
}

void bst_inorder(BSTNode *root) {
    if (root == NULL) {
        return;
    }
    bst_inorder(root->left);
    printf("[%d: %s] ", root->data.id, root->data.nama);
    bst_inorder(root->right);
}

void bst_postorder(BSTNode *root) {
    if (root == NULL) {
        return;
    }
    bst_postorder(root->left);
    bst_postorder(root->right);
    printf("[%d: %s] ", root->data.id, root->data.nama);
}

void free_bst(BSTNode *root) {
    if (root == NULL) {
        return;
    }
    free_bst(root->left);
    free_bst(root->right);
    free(root);
}

/* Graph */
void init_graph_toko(GraphToko *g) {
    int i, j;
    g->num_nodes = 0;
    for (i = 0; i < MAX_GRAPH_NODES; i++) {
        g->node_ids[i] = 0;
        g->node_labels[i][0] = '\0';
        for (j = 0; j < MAX_GRAPH_NODES; j++) {
            g->adj[i][j] = 0;
        }
    }
}

void tambah_relasi_graph(GraphToko *g, int idx_a, int idx_b) {
    if (idx_a >= 0 && idx_a < g->num_nodes &&
        idx_b >= 0 && idx_b < g->num_nodes &&
        idx_a != idx_b) {
        g->adj[idx_a][idx_b] = 1;
        g->adj[idx_b][idx_a] = 1;
    }
}

int save_graph_ke_file(const char *filename, const GraphToko *g) {
    FILE *fp = fopen(filename, "w");
    int i, j;
    if (fp == NULL) {
        return 0;
    }
    fprintf(fp, "%d\n", g->num_nodes);
    for (i = 0; i < g->num_nodes; i++) {
        fprintf(fp, "%d|%s\n", g->node_ids[i], g->node_labels[i]);
    }
    for (i = 0; i < g->num_nodes; i++) {
        for (j = i + 1; j < g->num_nodes; j++) {
            if (g->adj[i][j] == 1) {
                fprintf(fp, "EDGE %d %d\n", i, j);
            }
        }
    }
    fclose(fp);
    return 1;
}

int load_graph_dari_file(const char *filename, GraphToko *g, const Barang *list, int count) {
    FILE *fp = fopen(filename, "r");
    char line[MAX_LINE];
    int n = 0, i;

    if (fp == NULL) {
        build_default_graph(g, list, count);
        return 1;
    }

    init_graph_toko(g);
    if (fgets(line, sizeof(line), fp) == NULL ||
        sscanf(line, "%d", &n) != 1 || n <= 0 || n > MAX_GRAPH_NODES) {
        fclose(fp);
        build_default_graph(g, list, count);
        return 1;
    }

    g->num_nodes = n;
    for (i = 0; i < n; i++) {
        if (fgets(line, sizeof(line), fp) != NULL) {
            trim_newline(line);
            sscanf(line, "%d|%63[^\n]", &g->node_ids[i], g->node_labels[i]);
        }
    }

    while (fgets(line, sizeof(line), fp) != NULL) {
        int u, v;
        if (sscanf(line, "EDGE %d %d", &u, &v) == 2) {
            tambah_relasi_graph(g, u, v);
        }
    }
    fclose(fp);
    return 1;
}

void tampilkan_graph(const GraphToko *g) {
    int i, j;
    printf("\n=== GRAPH RELASI PRODUK & BUNDLING RAK TOKO ===\n");
    for (i = 0; i < g->num_nodes; i++) {
        int ada = 0;
        printf("[%d] %s (ID %d) terhubung dengan: ", i, g->node_labels[i], g->node_ids[i]);
        for (j = 0; j < g->num_nodes; j++) {
            if (g->adj[i][j] == 1) {
                printf("%s (%d), ", g->node_labels[j], j);
                ada = 1;
            }
        }
        if (!ada) {
            printf("(tidak ada relasi)");
        }
        printf("\n");
    }
}

void bfs_graph(const GraphToko *g, int start_idx) {
    int visited[MAX_GRAPH_NODES] = {0};
    int queue[MAX_GRAPH_NODES];
    int front = 0, rear = 0;

    if (start_idx < 0 || start_idx >= g->num_nodes) {
        printf("[!] Indeks simpul awal tidak valid.\n");
        return;
    }

    visited[start_idx] = 1;
    queue[rear++] = start_idx;

    printf("\n[BFS] Rekomendasi Bundling Bertingkat mulai dari '%s':\n", g->node_labels[start_idx]);
    while (front < rear) {
        int curr = queue[front++];
        int neighbor;
        printf(" -> [%d] %s\n", curr, g->node_labels[curr]);
        for (neighbor = 0; neighbor < g->num_nodes; neighbor++) {
            if (g->adj[curr][neighbor] == 1 && !visited[neighbor]) {
                visited[neighbor] = 1;
                queue[rear++] = neighbor;
            }
        }
    }
}

void dfs_graph(const GraphToko *g, int start_idx) {
    int visited[MAX_GRAPH_NODES] = {0};
    if (start_idx < 0 || start_idx >= g->num_nodes) {
        printf("[!] Indeks simpul awal tidak valid.\n");
        return;
    }
    printf("\n[DFS] Penelusuran Jalur Rak / Relasi Mendalam mulai dari '%s':\n", g->node_labels[start_idx]);
    dfs_recursive_helper(g, start_idx, visited);
}