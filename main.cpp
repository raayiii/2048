#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <string>
#include <conio.h>
#include <windows.h>
#include <vector>
#include <algorithm>

using namespace std;

// Struct untuk menyimpan state game
struct GameState {
    int papan[4][4];
    int skor;
    int gerakan;
};

// Struct untuk data statistik
struct Statistik {
    int skor;
    int gerakan;
    string tanggal;
    
    bool operator>(const Statistik& other) const {
        return skor > other.skor;
    }
};

// Variabel global
int papan[4][4];
int skor_sekarang;
int jumlah_gerakan;
bool udah_game_over;
int jumlah_undo = 0;
int jumlah_erase = 0;
GameState state_sebelumnya;

// Fungsi kasih warna
void kasih_warna(int angka) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    
    if(angka == 2) SetConsoleTextAttribute(hConsole, 11);
    else if(angka == 4) SetConsoleTextAttribute(hConsole, 9);
    else if(angka == 8) SetConsoleTextAttribute(hConsole, 13);
    else if(angka == 16) SetConsoleTextAttribute(hConsole, 12);
    else if(angka == 32) SetConsoleTextAttribute(hConsole, 4);
    else if(angka == 64) SetConsoleTextAttribute(hConsole, 6);
    else if(angka == 128) SetConsoleTextAttribute(hConsole, 14);
    else if(angka == 256) SetConsoleTextAttribute(hConsole, 10);
    else if(angka == 512) SetConsoleTextAttribute(hConsole, 2);
    else if(angka == 1024) SetConsoleTextAttribute(hConsole, 3);
    else if(angka == 2048) SetConsoleTextAttribute(hConsole, 15);
    else SetConsoleTextAttribute(hConsole, 7);
}

void reset_warna() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, 7);
}

// Fungsi untuk menyimpan state
void simpan_state() {
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            state_sebelumnya.papan[i][j] = papan[i][j];
        }
    }
    state_sebelumnya.skor = skor_sekarang;
    state_sebelumnya.gerakan = jumlah_gerakan;
}

// Fungsi untuk restore state
void restore_state() {
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            papan[i][j] = state_sebelumnya.papan[i][j];
        }
    }
    skor_sekarang = state_sebelumnya.skor;
    jumlah_gerakan = state_sebelumnya.gerakan;
}

// Fungsi untuk mendapatkan waktu saat ini
string get_waktu_sekarang() {
    time_t sekarang = time(0);
    char buffer[80];
    struct tm* timeinfo = localtime(&sekarang);
    strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", timeinfo);
    return string(buffer);
}

// Fungsi untuk membaca semua statistik
vector<Statistik> baca_semua_statistik() {
    vector<Statistik> stats;
    ifstream file("data/statistik.txt");
    
    if(file.is_open()) {
        int skor, gerakan;
        string tanggal, waktu;
        
        while(file >> skor >> gerakan >> tanggal >> waktu) {
            Statistik s;
            s.skor = skor;
            s.gerakan = gerakan;
            s.tanggal = tanggal + " " + waktu;
            stats.push_back(s);
        }
        file.close();
    }
    
    return stats;
}

// Fungsi untuk mendapatkan skor tertinggi
int get_skor_tertinggi() {
    vector<Statistik> stats = baca_semua_statistik();
    if(stats.empty()) return 0;
    
    sort(stats.begin(), stats.end(), greater<Statistik>());
    return stats[0].skor;
}

// Fungsi untuk menyimpan statistik baru
void simpan_statistik() {
    vector<Statistik> stats = baca_semua_statistik();
    
    Statistik baru;
    baru.skor = skor_sekarang;
    baru.gerakan = jumlah_gerakan;
    baru.tanggal = get_waktu_sekarang();
    
    stats.push_back(baru);
    sort(stats.begin(), stats.end(), greater<Statistik>());
    
    ofstream file("data/statistik.txt");
    if(file.is_open()) {
        for(const auto& s : stats) {
            file << s.skor << " " << s.gerakan << " " << s.tanggal << "\n";
        }
        file.close();
    }
}

// Fungsi untuk menyimpan game
void simpen_game_sekarang() {
    ofstream file("data/save_game.txt");
    
    if(file.is_open()) {
        file << skor_sekarang << " " << jumlah_gerakan << " " << jumlah_undo << " " << jumlah_erase << "\n";
        
        for(int i = 0; i < 4; i++) {
            for(int j = 0; j < 4; j++) {
                file << papan[i][j] << " ";
            }
        }
        file << "\n";
        
        file.close();
    }
}

// Fungsi untuk memuat game
bool muat_game_tersimpan() {
    ifstream file("data/save_game.txt");
    
    if(file.is_open()) {
        file >> skor_sekarang >> jumlah_gerakan >> jumlah_undo >> jumlah_erase;
        
        for(int i = 0; i < 4; i++) {
            for(int j = 0; j < 4; j++) {
                file >> papan[i][j];
            }
        }
        
        file.close();
        return true;
    }
    
    return false;
}

// Fungsi untuk menampilkan top 3 statistik
void tampilkan_statistik() {
    system("cls");
    
    cout << "\n+----------------------------------+\n";
    cout << "|      TOP 3 SKOR TERTINGGI        |\n";
    cout << "+----------------------------------+\n\n";
    
    vector<Statistik> stats = baca_semua_statistik();
    sort(stats.begin(), stats.end(), greater<Statistik>());
    
    if(stats.empty()) {
        cout << "Belum ada riwayat permainan!\n";
    } else {
        int tampil = min(3, (int)stats.size());
        for(int i = 0; i < tampil; i++) {
            cout << (i+1) << ". Skor: " << stats[i].skor 
                 << " | Gerakan: " << stats[i].gerakan 
                 << " | " << stats[i].tanggal << "\n";
        }
    }
    
    cout << "\nTekan tombol apapun untuk kembali...";
    _getch();
}

// Fungsi reset papan
void reset_papan() {
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            papan[i][j] = 0;
        }
    }
}

// Fungsi untuk menambah angka random atau power-up
void tambahin_angka_random() {
    int kotak_kosong[16][2];
    int jumlah_kosong = 0;
    
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            if(papan[i][j] == 0) {
                kotak_kosong[jumlah_kosong][0] = i;
                kotak_kosong[jumlah_kosong][1] = j;
                jumlah_kosong++;
            }
        }
    }
    
    if(jumlah_kosong > 0) {
        int pilih_random = rand() % jumlah_kosong;
        int chance = rand() % 100;
        
        // 5% chance untuk undo item
        if(chance < 5 && jumlah_gerakan > 0) {
            jumlah_undo++;
            cout << "\n[!] Dapat item UNDO!\n";
            Sleep(800);
        }
        // 5% chance untuk erase item
        else if(chance < 10) {
            jumlah_erase++;
            cout << "\n[!] Dapat item ERASE!\n";
            Sleep(800);
        }
        
        // Tetap tambahkan angka normal
        int angka_baru = (rand() % 10 < 9) ? 2 : 4;
        papan[kotak_kosong[pilih_random][0]][kotak_kosong[pilih_random][1]] = angka_baru;
    }
}

// Fungsi untuk menampilkan papan dengan power-ups
void tampilkan_papan() {
    system("cls");
    
    cout << "\n";
    cout << "+----------------------------------+\n";
    cout << "| SCORE:            " << setw(14) << left << skor_sekarang << "|\n";
    cout << "| BEST:             " << setw(14) << left << get_skor_tertinggi() << "|\n";
    cout << "| MOVES:            " << setw(14) << left << jumlah_gerakan << "|\n";
    cout << "+----------------------------------+\n";
    cout << "| UNDO:             " << setw(14) << left << jumlah_undo << "|\n";
    cout << "| ERASE:            " << setw(14) << left << jumlah_erase << "|\n";
    cout << "+----------------------------------+\n\n";
    
    cout << "+--------+--------+--------+--------+\n";
    
    for(int i = 0; i < 4; i++) {
        cout << "|";
        
        for(int j = 0; j < 4; j++) {
            cout << " ";
            
            if(papan[i][j] == 0) {
                cout << "      ";
            }
            else {
                kasih_warna(papan[i][j]);
                cout << setw(6) << right << papan[i][j];
                reset_warna();
            }
            
            cout << " |";
        }
        cout << "\n";
        
        if(i < 3) {
            cout << "+--------+--------+--------+--------+\n";
        }
    }
    
    cout << "+--------+--------+--------+--------+\n\n";
    
    cout << "Arrow Keys / WASD => Move\n";
    cout << "U => Use Undo | E => Use Erase\n";
    cout << "ESC => Exit to Menu\n";
    
    if(udah_game_over) {
        cout << "\n+----------------------------------+\n";
        cout << "|         GAME OVER!               |\n";
        cout << "| Press R to restart or ESC        |\n";
        cout << "+----------------------------------+\n";
    }
}

// Fungsi erase dengan selection box
void gunakan_erase() {
    if(jumlah_erase <= 0) {
        cout << "\nTidak ada erase yang tersedia!\n";
        Sleep(1000);
        return;
    }
    
    int sel_x = 0, sel_y = 0;
    
    while(true) {
        system("cls");
        cout << "\n=== MODE ERASE ===\n";
        cout << "Gunakan Arrow/WASD untuk memilih kotak\n";
        cout << "ENTER untuk menghapus | ESC untuk cancel\n\n";
        
        cout << "+--------+--------+--------+--------+\n";
        
        for(int i = 0; i < 4; i++) {
            cout << "|";
            
            for(int j = 0; j < 4; j++) {
                if(i == sel_x && j == sel_y) {
                    cout << "[";
                } else {
                    cout << " ";
                }
                
                if(papan[i][j] == 0) {
                    cout << "     ";
                } else {
                    cout << setw(5) << right << papan[i][j];
                }
                
                if(i == sel_x && j == sel_y) {
                    cout << "]|";
                } else {
                    cout << " |";
                }
            }
            cout << "\n+--------+--------+--------+--------+\n";
        }
        
        char tombol = _getch();
        
        if(tombol == -32 || tombol == 0) {
            tombol = _getch();
        }
        
        if(tombol == 27) { // ESC
            return;
        }
        else if(tombol == 13) { // ENTER
            if(papan[sel_x][sel_y] != 0) {
                papan[sel_x][sel_y] = 0;
                jumlah_erase--;
                cout << "\nKotak berhasil dihapus!\n";
                Sleep(1000);
                return;
            }
        }
        else if(tombol == 72 || tombol == 'w' || tombol == 'W') { // Atas
            sel_x = (sel_x - 1 + 4) % 4;
        }
        else if(tombol == 80 || tombol == 's' || tombol == 'S') { // Bawah
            sel_x = (sel_x + 1) % 4;
        }
        else if(tombol == 75 || tombol == 'a' || tombol == 'A') { // Kiri
            sel_y = (sel_y - 1 + 4) % 4;
        }
        else if(tombol == 77 || tombol == 'd' || tombol == 'D') { // Kanan
            sel_y = (sel_y + 1) % 4;
        }
    }
}

// Fungsi geser (tetap sama, tapi disederhanakan)
bool geser_kiri() {
    bool ada_perubahan = false;
    
    for(int i = 0; i < 4; i++) {
        int posisi_tulis = 0;
        
        for(int j = 0; j < 4; j++) {
            if(papan[i][j] != 0 && papan[i][j] > 0) {
                if(j != posisi_tulis) {
                    papan[i][posisi_tulis] = papan[i][j];
                    papan[i][j] = 0;
                    ada_perubahan = true;
                }
                posisi_tulis++;
            }
        }
    }
    
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 3; j++) {
            if(papan[i][j] > 0 && papan[i][j] == papan[i][j + 1]) {
                papan[i][j] *= 2;
                skor_sekarang += papan[i][j];
                papan[i][j + 1] = 0;
                ada_perubahan = true;
                
                for(int k = j + 1; k < 3; k++) {
                    papan[i][k] = papan[i][k + 1];
                    papan[i][k + 1] = 0;
                }
            }
        }
    }
    
    return ada_perubahan;
}

bool geser_kanan() {
    bool ada_perubahan = false;
    
    for(int i = 0; i < 4; i++) {
        int posisi_tulis = 3;
        
        for(int j = 3; j >= 0; j--) {
            if(papan[i][j] != 0 && papan[i][j] > 0) {
                if(j != posisi_tulis) {
                    papan[i][posisi_tulis] = papan[i][j];
                    papan[i][j] = 0;
                    ada_perubahan = true;
                }
                posisi_tulis--;
            }
        }
    }
    
    for(int i = 0; i < 4; i++) {
        for(int j = 3; j > 0; j--) {
            if(papan[i][j] > 0 && papan[i][j] == papan[i][j - 1]) {
                papan[i][j] *= 2;
                skor_sekarang += papan[i][j];
                papan[i][j - 1] = 0;
                ada_perubahan = true;
                
                for(int k = j - 1; k > 0; k--) {
                    papan[i][k] = papan[i][k - 1];
                    papan[i][k - 1] = 0;
                }
            }
        }
    }
    
    return ada_perubahan;
}

bool geser_atas() {
    bool ada_perubahan = false;
    
    for(int j = 0; j < 4; j++) {
        int posisi_tulis = 0;
        
        for(int i = 0; i < 4; i++) {
            if(papan[i][j] != 0 && papan[i][j] > 0) {
                if(i != posisi_tulis) {
                    papan[posisi_tulis][j] = papan[i][j];
                    papan[i][j] = 0;
                    ada_perubahan = true;
                }
                posisi_tulis++;
            }
        }
    }
    
    for(int j = 0; j < 4; j++) {
        for(int i = 0; i < 3; i++) {
            if(papan[i][j] > 0 && papan[i][j] == papan[i + 1][j]) {
                papan[i][j] *= 2;
                skor_sekarang += papan[i][j];
                papan[i + 1][j] = 0;
                ada_perubahan = true;
                
                for(int k = i + 1; k < 3; k++) {
                    papan[k][j] = papan[k + 1][j];
                    papan[k + 1][j] = 0;
                }
            }
        }
    }
    
    return ada_perubahan;
}

bool geser_bawah() {
    bool ada_perubahan = false;
    
    for(int j = 0; j < 4; j++) {
        int posisi_tulis = 3;
        
        for(int i = 3; i >= 0; i--) {
            if(papan[i][j] != 0 && papan[i][j] > 0) {
                if(i != posisi_tulis) {
                    papan[posisi_tulis][j] = papan[i][j];
                    papan[i][j] = 0;
                    ada_perubahan = true;
                }
                posisi_tulis--;
            }
        }
    }
    
    for(int j = 0; j < 4; j++) {
        for(int i = 3; i > 0; i--) {
            if(papan[i][j] > 0 && papan[i][j] == papan[i - 1][j]) {
                papan[i][j] *= 2;
                skor_sekarang += papan[i][j];
                papan[i - 1][j] = 0;
                ada_perubahan = true;
                
                for(int k = i - 1; k > 0; k--) {
                    papan[k][j] = papan[k - 1][j];
                    papan[k - 1][j] = 0;
                }
            }
        }
    }
    
    return ada_perubahan;
}

bool cek_game_over() {
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            if(papan[i][j] == 0) return false;
        }
    }
    
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 3; j++) {
            if(papan[i][j] == papan[i][j + 1] && papan[i][j] > 0) return false;
        }
    }
    
    for(int j = 0; j < 4; j++) {
        for(int i = 0; i < 3; i++) {
            if(papan[i][j] == papan[i + 1][j] && papan[i][j] > 0) return false;
        }
    }
    
    return true;
}

// Fungsi untuk mengumpulkan power-up
void kumpulkan_powerup(int nilai) {
    if(nilai == -1) {
        jumlah_undo++;
    } else if(nilai == -2) {
        jumlah_erase++;
    }
}

void mulai_game_baru() {
    skor_sekarang = 0;
    jumlah_gerakan = 0;
    jumlah_undo = 0;
    jumlah_erase = 0;
    udah_game_over = false;
    
    reset_papan();
    tambahin_angka_random();
    tambahin_angka_random();
    
    tampilkan_papan();
    
    char tombol;
    bool berhasil_geser = false;
    
    while(true) {
        tombol = _getch();
        
        if(tombol == -32 || tombol == 0) {
            tombol = _getch();
        }
        
        if(tombol == 27) { // ESC
            simpen_game_sekarang();
            break;
        }
        
        if(udah_game_over && (tombol == 'r' || tombol == 'R')) {
            mulai_game_baru();
            return;
        }
        
        if(udah_game_over) continue;
        
        // Undo
        if((tombol == 'u' || tombol == 'U') && jumlah_undo > 0) {
            restore_state();
            jumlah_undo--;
            tampilkan_papan();
            continue;
        }
        
        // Erase
        if((tombol == 'e' || tombol == 'E') && jumlah_erase > 0) {
            gunakan_erase();
            tampilkan_papan();
            continue;
        }
        
        simpan_state();
        berhasil_geser = false;
        
        // Kontrol dengan arrow keys dan WASD
        if(tombol == 72 || tombol == 'w' || tombol == 'W') {
            berhasil_geser = geser_atas();
        }
        else if(tombol == 80 || tombol == 's' || tombol == 'S') {
            berhasil_geser = geser_bawah();
        }
        else if(tombol == 75 || tombol == 'a' || tombol == 'A') {
            berhasil_geser = geser_kiri();
        }
        else if(tombol == 77 || tombol == 'd' || tombol == 'D') {
            berhasil_geser = geser_kanan();
        }
        
        if(berhasil_geser) {
            jumlah_gerakan++;
            tambahin_angka_random();
            tampilkan_papan();
            
            if(cek_game_over()) {
                udah_game_over = true;
                simpan_statistik();
                tampilkan_papan();
            }
        }
    }
}

void lanjutin_game() {
    if(muat_game_tersimpan()) {
        udah_game_over = false;
        tampilkan_papan();
        
        char tombol;
        bool berhasil_geser = false;
        
        while(true) {
            tombol = _getch();
            
            if(tombol == -32 || tombol == 0) {
                tombol = _getch();
            }
            
            if(tombol == 27) {
                simpen_game_sekarang();
                break;
            }
            
            if(udah_game_over && (tombol == 'r' || tombol == 'R')) {
                mulai_game_baru();
                return;
            }
            
            if(udah_game_over) continue;
            
            if((tombol == 'u' || tombol == 'U') && jumlah_undo > 0) {
                restore_state();
                jumlah_undo--;
                tampilkan_papan();
                continue;
            }
            
            if((tombol == 'e' || tombol == 'E') && jumlah_erase > 0) {
                gunakan_erase();
                tampilkan_papan();
                continue;
            }
            
            simpan_state();
            berhasil_geser = false;
            
            if(tombol == 72 || tombol == 'w' || tombol == 'W') {
                berhasil_geser = geser_atas();
            }
            else if(tombol == 80 || tombol == 's' || tombol == 'S') {
                berhasil_geser = geser_bawah();
            }
            else if(tombol == 75 || tombol == 'a' || tombol == 'A') {
                berhasil_geser = geser_kiri();
            }
            else if(tombol == 77 || tombol == 'd' || tombol == 'D') {
                berhasil_geser = geser_kanan();
            }
            
            if(berhasil_geser) {
                jumlah_gerakan++;
                tambahin_angka_random();
                tampilkan_papan();
                
                if(cek_game_over()) {
                    udah_game_over = true;
                    simpan_statistik();
                    tampilkan_papan();
                }
            }
        }
    } else {
        system("cls");
        cout << "\n\nBelum ada game tersimpan!\n\n";
        cout << "Tekan tombol apapun untuk kembali...";
        _getch();
    }
}

void tampilkan_menu() {
    system("cls");
    
    cout << "\n";
    cout << "+----------------------------------+\n";
    cout << "|         GAME 2048                |\n";
    cout << "+----------------------------------+\n\n";
    cout << "1. Mulai Game Baru\n";
    cout << "2. Lanjutin Game\n";
    cout << "3. Lihat Top 3 Skor\n";
    cout << "4. Keluar (ESC)\n\n";
    cout << "Pilih menu (1-4): ";
}

int main() {
    srand(time(0));
    
    system("mkdir data 2>nul");
    
    while(true) {
        tampilkan_menu();
        
        char pilihan = _getch();
        
        if(pilihan == '1') {
            mulai_game_baru();
        }
        else if(pilihan == '2') {
            lanjutin_game();
        }
        else if(pilihan == '3') {
            tampilkan_statistik();
        }
        else if(pilihan == '4' || pilihan == 27) {
            system("cls");
            cout << "\n\nTerima kasih sudah bermain!\n\n";
            break;
        }
    }
    
    return 0;
}