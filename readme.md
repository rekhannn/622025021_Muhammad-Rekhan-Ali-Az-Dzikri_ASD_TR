# Sistem Kasir dan Manajemen Toko Sederhana

Program kasir dan manajemen stok toko berbasis konsol (CLI) yang dibuat menggunakan bahasa **C** untuk Tugas Rancang mata kuliah **Algoritma dan Struktur Data**.

## Fitur Utama
- **Login Multi-Peran (`auth.c`):** Akses terpisah untuk Admin dan Kasir.
- **Manajemen Barang (`barang.c`):** Tambah, lihat, ubah, hapus, cari (*Sequential & Binary Search*), dan urutkan barang (*Bubble & Selection Sort*).
- **Transaksi Kasir (`kasir.c`):** Hitung total belanja, kembalian, dan pembatalan item terakhir (*Undo* dengan Stack).
- **Struktur Data (`struktur_data.c`):** Implementasi Stack, Queue (antrean pembeli), Hash Table, BST, dan Graph.
- **Penyimpanan File (`data/`):** Data barang dan stok tersimpan otomatis dalam file `.txt`.

## Struktur Folder
```text
Tugas rancang asd 2026/
├── app.exe
├── README.md
├── data/          # File penyimpanan data (.txt)
├── src/           # Header konfigurasi & modul (config.h, modules.h)
└── include/       # File sumber C & model (main.c, auth.c, barang.c, kasir.c, struktur_data.c, until.c, models.h)
```

## Cara Menjalankan
1. **Kompilasi program:**
   ```bash
   gcc -std=c99 -Wall -Wextra -Iinclude src\main.c src\auth.c src\barang.c src\kasir.c src\struktur_data.c src\until.c -o app.exe; .\app.exe
   ```
2. **Jalankan program:**
   ```bash
   app.exe
   ```

---
Dibuat oleh **Muhammad Rekhan Ali Az Dzikri** (622025021) - 2026