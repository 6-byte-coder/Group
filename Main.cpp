#include <algorithm>
#include <array>
#include <iostream>
#include <random>
#include <string>
#include <utility>
#include <vector>

using Cell = std::pair<int, int>;

struct Piece {
    std::vector<Cell> cells;
    char symbol;
};

class BlockBlast {
private:
    static constexpr int SIZE = 8;

    std::array<std::array<char, SIZE>, SIZE> board{};
    std::vector<Piece> pieces;
    std::array<bool, 3> used{};

    int score = 0;
    std::mt19937 rng{std::random_device{}()};

    std::vector<Piece> allPieces{
        {{{0, 0}}, 'A'},

        {{{0, 0}, {0, 1}}, 'B'},
        {{{0, 0}, {1, 0}}, 'C'},

        {{{0, 0}, {0, 1}, {0, 2}}, 'D'},
        {{{0, 0}, {1, 0}, {2, 0}}, 'E'},

        {{{0, 0}, {0, 1}, {1, 0}, {1, 1}}, 'F'},

        {{{0, 0}, {0, 1}, {0, 2}, {0, 3}}, 'G'},
        {{{0, 0}, {1, 0}, {2, 0}, {3, 0}}, 'H'},

        {{{0, 0}, {1, 0}, {1, 1}}, 'I'},
        {{{0, 1}, {1, 0}, {1, 1}}, 'J'},
        {{{0, 0}, {0, 1}, {1, 1}}, 'K'},
        {{{0, 0}, {0, 1}, {1, 0}}, 'L'},

        {{{0, 0}, {0, 1}, {0, 2}, {1, 1}}, 'M'},
        {{{0, 1}, {1, 0}, {1, 1}, {2, 1}}, 'N'},

        {{{0, 0}, {0, 1}, {1, 0}, {2, 0}}, 'O'},
        {{{0, 0}, {0, 1}, {1, 1}, {2, 1}}, 'P'},

        {{{0, 0}, {1, 0}, {1, 1}, {2, 1}}, 'Q'},
        {{{0, 1}, {1, 1}, {1, 0}, {2, 0}}, 'R'},

        {{{0, 0}, {0, 1}, {0, 2}, {1, 0}, {1, 1}, {1, 2}}, 'S'},
        {{{0, 0}, {0, 1}, {1, 0}, {1, 1}, {2, 0}, {2, 1}}, 'T'}
    };

public:
    BlockBlast() {
        clearBoard();
        refillPieces();
    }

    void run() {
        std::cout << "=============================\n";
        std::cout << "      BLOCK BLAST C++\n";
        std::cout << "=============================\n";
        std::cout << "Place pieces to complete rows\n";
        std::cout << "or columns and clear them.\n\n";

        while (true) {
            draw();

            if (!hasAnyMove()) {
                std::cout << "\nGAME OVER!\n";
                std::cout << "Final score: " << score << "\n";
                break;
            }

            std::cout << "\nChoose a piece and position.\n";
            std::cout << "Format: piece row column\n";
            std::cout << "Example: 1 3 4\n";
            std::cout << "Rows and columns are numbered 0-7.\n";
            std::cout << "Enter q to quit.\n> ";

            std::string input;
            std::cin >> input;

            if (!std::cin) {
                break;
            }

            if (input == "q" || input == "Q") {
                std::cout << "Thanks for playing!\n";
                break;
            }

            try {
                int pieceIndex = std::stoi(input);
                int row;
                int col;
                std::cin >> row >> col;

                pieceIndex--;

                if (pieceIndex < 0 || pieceIndex >= 3) {
                    std::cout << "Invalid piece number.\n";
                    continue;
                }

                if (used[pieceIndex]) {
                    std::cout << "That piece has already been used.\n";
                    continue;
                }

                if (!canPlace(pieces[pieceIndex], row, col)) {
                    std::cout << "That piece cannot be placed there.\n";
                    continue;
                }

                placePiece(pieces[pieceIndex], row, col);
                used[pieceIndex] = true;

                int cleared = clearCompletedLines();

                score += static_cast<int>(pieces[pieceIndex].cells.size()) * 10;
                score += cleared * 100;

                std::cout << "Piece placed!";
                if (cleared > 0) {
                    std::cout << " Cleared " << cleared << " line";
                    if (cleared != 1) {
                        std::cout << "s";
                    }
                    std::cout << "!";
                }
                std::cout << "\n";

                if (allPiecesUsed()) {
                    refillPieces();
                }
            } catch (...) {
                std::cout << "Invalid input.\n";
            }
        }
    }

private:
    void clearBoard() {
        for (auto& row : board) {
            row.fill('.');
        }
    }

    void refillPieces() {
        std::uniform_int_distribution<int> distribution(
            0, static_cast<int>(allPieces.size()) - 1
        );

        pieces.clear();
        used.fill(false);

        for (int i = 0; i < 3; ++i) {
            pieces.push_back(allPieces[distribution(rng)]);
        }
    }

    void draw() const {
        std::cout << "\nScore: " << score << "\n\n";

        std::cout << "    ";
        for (int col = 0; col < SIZE; ++col) {
            std::cout << col << " ";
        }
        std::cout << "\n";

        std::cout << "   +----------------+\n";

        for (int row = 0; row < SIZE; ++row) {
            std::cout << row << "  |";

            for (int col = 0; col < SIZE; ++col) {
                std::cout << board[row][col] << " ";
            }

            std::cout << "|\n";
        }

        std::cout << "   +----------------+\n\n";

        std::cout << "Available pieces:\n";

        for (int i = 0; i < 3; ++i) {
            std::cout << i + 1 << ") ";

            if (used[i]) {
                std::cout << "[used]\n";
                continue;
            }

            char preview[5][5]{};

            for (const auto& cell : pieces[i].cells) {
                int r = cell.first;
                int c = cell.second;

                if (r < 5 && c < 5) {
                    preview[r][c] = pieces[i].symbol;
                }
            }

            std::cout << "\n";

            for (int r = 0; r < 5; ++r) {
                std::cout << "   ";

                for (int c = 0; c < 5; ++c) {
                    std::cout << (preview[r][c] == '\0'
                                     ? ' '
                                     : preview[r][c])
                              << " ";
                }

                std::cout << "\n";
            }
        }
    }

    bool canPlace(const Piece& piece, int row, int col) const {
        for (const auto& cell : piece.cells) {
            int boardRow = row + cell.first;
            int boardCol = col + cell.second;

            if (boardRow < 0 || boardRow >= SIZE ||
                boardCol < 0 || boardCol >= SIZE) {
                return false;
            }

            if (board[boardRow][boardCol] != '.') {
                return false;
            }
        }

        return true;
    }

    void placePiece(const Piece& piece, int row, int col) {
        for (const auto& cell : piece.cells) {
            board[row + cell.first][col + cell.second] = piece.symbol;
        }
    }

    int clearCompletedLines() {
        std::array<bool, SIZE> fullRows{};
        std::array<bool, SIZE> fullCols{};

        int cleared = 0;

        for (int row = 0; row < SIZE; ++row) {
            fullRows[row] = true;

            for (int col = 0; col < SIZE; ++col) {
                if (board[row][col] == '.') {
                    fullRows[row] = false;
                    break;
                }
            }

            if (fullRows[row]) {
                cleared++;
            }
        }

        for (int col = 0; col < SIZE; ++col) {
            fullCols[col] = true;

            for (int row = 0; row < SIZE; ++row) {
                if (board[row][col] == '.') {
                    fullCols[col] = false;
                    break;
                }
            }

            if (fullCols[col]) {
                cleared++;
            }
        }

        for (int row = 0; row < SIZE; ++row) {
            for (int col = 0; col < SIZE; ++col) {
                if (fullRows[row] || fullCols[col]) {
                    board[row][col] = '.';
                }
            }
        }

        return cleared;
    }

    bool allPiecesUsed() const {
        return used[0] && used[1] && used[2];
    }

    bool hasAnyMove() const {
        for (int pieceIndex = 0; pieceIndex < 3; ++pieceIndex) {
            if (used[pieceIndex]) {
                continue;
            }

            for (int row = 0; row < SIZE; ++row) {
                for (int col = 0; col < SIZE; ++col) {
                    if (canPlace(pieces[pieceIndex], row, col)) {
                        return true;
                    }
                }
            }
        }

        return false;
    }
};

int main() {
    BlockBlast game;
    game.run();

    return 0;
}
