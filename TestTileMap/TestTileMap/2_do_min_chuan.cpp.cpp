#include <iostream>
#include <queue>
#include <random>
#include <iomanip>
using namespace std;

const int MAX_SIZE = 20;

// T?o b?n d? mìn
void create_board(int m, int n, int k, bool board[][MAX_SIZE]) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, m * n - 1);

    for (int i = 0; i < k; i++) {
        int idx = dis(gen);
        int row = idx / n;
        int col = idx % n;
        board[row][col] = true;
    }
}

// In b?n d?
void print_board(int m, int n, char board[][MAX_SIZE]) {
    system("cls");
    for (int i = -1; i < n ; i++) {
        if (i == -1) {
            cout << setw(6) << right << setfill(' ');
            continue;
        }
        cout << i << setw(3) << right << setfill(' ');
    }
    cout << endl;
    for (int i = 0; i < m; i++) {
            cout << i << setw(3) << right << setfill(' ');
            for (int j = 0; j < n; j++) {
                cout << board[i][j] << setw(3) << right << setfill(' ');
            }
        cout << endl;
    }
}

// Tìm s? lu?ng mìn xung quanh ô
int count_mines(int m, int n, bool board[][MAX_SIZE], int row, int col) {
    int count = 0;
    for (int i = row - 1; i <= row + 1; i++) {
        for (int j = col - 1; j <= col + 1; j++) {
            if (i >= 0 && i < m && j >= 0 && j < n && board[i][j]) {
                count++;
            }
        }
    }
    return count;
}

// Duy?t theo chi?u r?ng d? tìm các ô không có mìn xung quanh ô dã du?c m?
void bfs(int m, int n, bool board[][MAX_SIZE], bool visited[][MAX_SIZE], int row, int col) {
    queue<pair<int, int>> q;
    q.push(make_pair(row, col));
    visited[row][col] = true;
    while (!q.empty()) {
        int r = q.front().first;
        int c = q.front().second;
        q.pop();
        for (int i = r - 1; i <= r + 1; i++) {
            for (int j = c - 1; j <= c + 1; j++) {
                if (i >= 0 && i < m && j >= 0 && j < n && !board[i][j] && !visited[i][j]) {
                    visited[i][j] = true;
                    if (count_mines(m, n, board, i, j) == 0) {
                        q.push(make_pair(i, j));
                    }
                }
            }
        }
    }
}
// d?t c?
void flag(const int& m, const int& n, char board[][MAX_SIZE])
{
    int row1, col1;
    cout << "nhap vi tri dat co :";
    cin >> row1 >> col1;
    board[row1][col1] = 'x';
}
// xóa c?
void cleanflag(const int& m, const int& n, char board[][MAX_SIZE])
{
    int row2, col2;
    cout << "nhap vi tri xoa co :";
    cin >> row2 >> col2;
    if (board[row2][col2] == 'x')
    {
        board[row2][col2] = '.';
    }
    else
    {
        cout << "vi tri nay khong co flag!";
    }
}
int main() {
    int m, n, k;
    cout << "Enter board size (m x n): ";
    cin >> m >> n;
    cout << "Enter number of mines: ";
    cin >> k;

    bool board[MAX_SIZE][MAX_SIZE] = { false };
    create_board(m, n, k, board);

    char display_board[MAX_SIZE][MAX_SIZE] = { ' ' };
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            display_board[i][j] = '.';
        }
    }

    bool game_over = false;
    int num_opened = 0;
    while (!game_over) {
        // In b?n d? hi?n t?i
        print_board(m, n, display_board);

        // d?t c? hay m? ô
        int tmp = 1;
        while (tmp)
        {
            cout << "buoc ban muon thuc hien :" << endl;
            cout << "0: mo o." << endl;
            cout << "1: dat co." << endl;
            cout << "2: xoa co" << endl;
            cin >> tmp;
            if (tmp == 1)
            {
                flag(m, n, display_board);
                print_board(m, n, display_board);
            }
            else if (tmp == 2)
            {
                cleanflag(m, n, display_board);
                print_board(m, n, display_board);
            }
            else if (tmp >= 3 || tmp < 0)
            {
                cout << "khong co lenh nay. Moi nhap lai ! ";
                break;
            }
            else break;
        }
        // Nh?p t?a d? ô c?n m?
        int row, col;
        cout << "Enter row and column to open: ";
        cin >> row >> col;

        // Ki?m tra ô dã du?c m? tru?c dó chua
        if (display_board[row][col] != '.' && display_board[row][col] != 'x') {
            cout << "This cell has already been opened! Try again." << endl;
            continue;
        }

        // Ki?m tra có mìn hay không
        if (board[row][col]) {
            cout << "YOU'RE DEAD!" << endl;
            game_over = true;
        }
        else {
            // Ð?m s? lu?ng mìn xung quanh ô dã m?
            int count = count_mines(m, n, board, row, col);
            display_board[row][col] = '0' + count;
            num_opened++;

            // N?u không có mìn xung quanh, duy?t tìm các ô không có mìn xung quanh
            if (count == 0) {
                bool visited[MAX_SIZE][MAX_SIZE] = { false };
                bfs(m, n, board, visited, row, col);
                for (int i = 0; i < m; i++) {
                    for (int j = 0; j < n; j++) {
                        if (visited[i][j]) {
                            int count = count_mines(m, n, board, i, j);
                            display_board[i][j] = '0' + count;
                            num_opened++;
                        }
                    }
                }
            }

            // Ki?m tra dã m? h?t các ô không có mìn chua
            if (num_opened == m * n - k) {
                cout << "YOU WIN!" << endl;
                game_over = true;
            }
        }
        
    }
    system("cls");
    // In b?n d? v?i t?t c? các qu? mìn ra màn hình
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            if (board[i][j]) {
                display_board[i][j] = '*';
            }
        }
    }
    print_board(m, n, display_board);

    return 0;
}

