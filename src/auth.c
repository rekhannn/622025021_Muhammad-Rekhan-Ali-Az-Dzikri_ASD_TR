#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "modules.h"

int user_exists(const char *filename, const char *username) {
    FILE *fp = fopen(filename, "r");
    char line[MAX_LINE];
    char file_user[MAX_USERNAME];
    char file_pass[MAX_PASSWORD];

    if (fp == NULL) {
        return 0;
    }
    while (fgets(line, sizeof(line), fp) != NULL) {
        trim_newline(line);
        if (sscanf(line, "%31[^|]|%31[^\n]", file_user, file_pass) == 2) {
            if (strcmp(file_user, username) == 0) {
                fclose(fp);
                return 1;
            }
        }
    }
    fclose(fp);
    return 0;
}

int register_user(const char *filename) {
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];
    FILE *fp;

    printf("\n=== REGISTER AKUN KASIR / ADMIN ===\n");
    printf("Masukkan Username baru : ");
    if (!read_line_safe(username, sizeof(username)) || is_blank_or_whitespace(username)) {
        printf("[!] Username tidak boleh kosong.\n");
        return 0;
    }
    if (strchr(username, '|') != NULL || strchr(username, ' ') != NULL) {
        printf("[!] Username tidak boleh mengandung spasi atau karakter '|'.\n");
        return 0;
    }
    if (user_exists(filename, username)) {
        printf("[!] Username sudah digunakan! Silakan pilih username lain.\n");
        return 0;
    }

    printf("Masukkan Password baru : ");
    if (!read_line_safe(password, sizeof(password)) || is_blank_or_whitespace(password)) {
        printf("[!] Password tidak boleh kosong.\n");
        return 0;
    }
    if (strchr(password, '|') != NULL) {
        printf("[!] Password tidak boleh mengandung karakter '|'.\n");
        return 0;
    }

    fp = fopen(filename, "a");
    if (fp == NULL) {
        printf("[!] Gagal membuka file database user.\n");
        return 0;
    }
    fprintf(fp, "%s|%s\n", username, password);
    fclose(fp);

    printf("[+] Akun '%s' berhasil didaftarkan!\n", username);
    return 1;
}

int login_user(const char *filename, char *logged_in_user, int user_cap) {
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];
    char line[MAX_LINE];
    char file_user[MAX_USERNAME];
    char file_pass[MAX_PASSWORD];
    FILE *fp;

    printf("\n=== LOGIN SISTEM KASIR ===\n");
    printf("Username : ");
    if (!read_line_safe(username, sizeof(username)) || is_blank_or_whitespace(username)) {
        printf("[!] Login gagal: Username tidak boleh kosong.\n");
        return 0;
    }
    printf("Password : ");
    if (!read_line_safe(password, sizeof(password)) || is_blank_or_whitespace(password)) {
        printf("[!] Login gagal: Password tidak boleh kosong.\n");
        return 0;
    }

    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("[!] Login gagal: Belum ada akun terdaftar. Silakan Register dulu.\n");
        return 0;
    }

    while (fgets(line, sizeof(line), fp) != NULL) {
        trim_newline(line);
        if (sscanf(line, "%31[^|]|%31[^\n]", file_user, file_pass) == 2) {
            if (strcmp(file_user, username) == 0 && strcmp(file_pass, password) == 0) {
                strncpy(logged_in_user, username, (size_t)user_cap - 1);
                logged_in_user[user_cap - 1] = '\0';
                fclose(fp);
                printf("[+] Login berhasil! Selamat datang, %s.\n", logged_in_user);
                return 1;
            }
        }
    }

    fclose(fp);
    printf("[!] Login gagal: Username atau password salah.\n");
    return 0;
}