/* Copyright (C) 2018, Gepard Graphics
 * Copyright (C) 2018, Kristof Kosztyo <kkristof@inf.u-szeged.hu>
 * Copyright (C) 2018, Szilard Ledan <szledan@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "gepard.h"
#include <ctype.h>
#include <sstream>

class ChessBoard {
public:
    ChessBoard(const uint size)
        : _size(size)
        , _tableOffsetX(2)
        , _surface(new gepard::Surface(12 * size, 8 * size))
        , _ctx(new gepard::Gepard(_surface))
    {
        std::string pieces = "qkrnbplQKRNBPL";
        for (int i = 0; i < 14; ++i) {
            drawPiece(pieces[i], i % 7, i / 7);
        }
        _chessResizedPieces = _ctx->getImageData(2 * _size, 0, 7 * _size, 2 * _size);

        generateCheckerBoard();
    }

    ~ChessBoard()
    {
        if (_ctx) {
            delete _ctx;
        }
        if (_surface) {
            delete _surface;
        }
    }

    void doSteps(const std::string& moves)
    {
        std::stringstream ss;
        ss << moves;
        std::string line;
        while (ss >> line) {
            if (ss >> line) {
                step(line, Piece::White);
            }
            if (ss >> line) {
                step(line, Piece::Black);
            }
        }
    }

    void savePosition(const std::string& fileName)
    {
        gepard::utils::savePng(_ctx->getImageData(0, 0, _surface->width(), _surface->height()), fileName);
    }

private:
    struct Piece {
        enum Color {
            White = 1,
            Black = 0,
        } color : 1;
        enum Name {
            Undefined = 0,
            Queen = 1,
            King = 2,
            Rook = 3,
            KnightLeft = 4,
            Bishop = 5,
            Pawn = 6,
            KnightRight = 7,
        } name : 3;

        Piece(const Name pn = Undefined, const Color pc = White)
            : color(pc)
            , name(pn)
        {
        }
    };

    const Piece charToPiece(char piece)
    {
        Piece::Color pc = (piece < 'a') ? Piece::White : Piece::Black;
        piece = toupper(piece);

        if (piece == 'P') { return Piece(Piece::Pawn, pc); }
        else if (piece == 'K') { return Piece(Piece::King, pc); }
        else if (piece == 'Q') { return Piece(Piece::Queen, pc); }
        else if (piece == 'R') { return Piece(Piece::Rook, pc); }
        else if (piece == 'B') { return Piece(Piece::Bishop, pc); }
        else if (piece == 'N') { return Piece(Piece::KnightLeft, pc); }
        else if (piece == 'L') { return Piece(Piece::KnightRight, pc); }
        return Piece(Piece::Undefined, pc);
    }
    const char pieceToChar(const Piece& piece) const
    {
        if (piece.name == Piece::Pawn) { return (piece.color) ? 'P' : 'p'; }
        else if (piece.name == Piece::King) { return (piece.color) ? 'K' : 'k'; }
        else if (piece.name == Piece::Queen) { return (piece.color) ? 'Q' : 'q'; }
        else if (piece.name == Piece::Rook) { return (piece.color) ? 'R' : 'r'; }
        else if (piece.name == Piece::Bishop) { return (piece.color) ? 'B' : 'b'; }
        else if (piece.name == Piece::KnightLeft) { return (piece.color) ? 'N' : 'n'; }
        else if (piece.name == Piece::KnightRight) { return (piece.color) ? 'L' : 'l'; }
        return ' ';
    }

    void updateTable()
    {
        for (int i = 0; i < 64; ++i) {
            drawPiece(_table[i], i % 8, i / 8);
        }
        for (size_t i = 0; i < _whitePieces.size(); ++i) {
            drawPiece(_whitePieces[i], -(int(i) + 8) / 8, int(i) % 8, false);
        }
        for (size_t i = 0; i < _blackPieces.size(); ++i) {
            drawPiece(_blackPieces[i], int(i) / 8 + 8, 7 - (int(i) % 8), false);
        }
    }

    void step(const std::string& move, const Piece::Color stepColor)
    {
        if (move.length() < 4)
            return;

        int pawnOffset = (move[3] == '-' || move[3] == 'x');

        Piece piece = Piece(pawnOffset ? charToPiece(move[0]).name : Piece::Pawn, stepColor);

        if (move[pawnOffset + 2] == 'x') {
            char oldPiece = _table[7 - (move[pawnOffset + 3] - 'a') + 8 * (move[pawnOffset + 4] - '1')];
            if (stepColor) {
                _blackPieces = oldPiece + _blackPieces;
            } else {
                _whitePieces = oldPiece + _whitePieces;
            }
            drawBoardTile(7 - (move[pawnOffset + 3] - 'a'), (move[pawnOffset + 4] - '1'));
        }
        _table[7 - (move[pawnOffset] - 'a') + 8 * (move[pawnOffset + 1] - '1')] = ' ';
        _table[7 - (move[pawnOffset + 3] - 'a') + 8 * (move[pawnOffset + 4] - '1')] = pieceToChar(piece);
        updateTable();
    }

    void generateCheckerBoard()
    {
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                drawBoardTile(i, j);
            }
        }
    }

    void drawBoardTile(const uint row, const uint column)
    {
        gepard::Image* tileImage = &_chessLightTile;
        if ((row + column) % 2) {
            tileImage = &_chessDarkTile;
        }
        _ctx->drawImage(*tileImage, 0, 0, tileImage->width(), tileImage->height(), (row + _tableOffsetX) * _size, column * _size, _size, _size);
    }

    void drawPiece(const char pieceCh, const int tileX, const int tileY, const bool cmdDraw = true)
    {
        if (pieceCh == ' ') {
            drawBoardTile(tileX, tileY);
            return;
        }

        Piece piece = charToPiece(pieceCh);
        int srcRow = piece.name - 1;
        int srcCol = piece.color;

        _ctx->save();
        if (srcCol) {
            // Transform WHITE pieces.
            _ctx->translate(2 * (tileX + _tableOffsetX) * _size + _size, 0);
            _ctx->scale(-1, 1);
            _ctx->translate(2 * (tileX + _tableOffsetX) * _size + _size, 2 * (tileY) * _size + _size);
            _ctx->rotate(3.14159265359);
        }
        if (cmdDraw) {
            // Flip left KNIGHT.
            if (piece.name == Piece::KnightRight) {
                _ctx->translate(2 * (tileX + _tableOffsetX) * _size + _size, 0);
                _ctx->scale(-1, 1);
                srcRow = Piece::KnightLeft - 1;
            }
            _ctx->drawImage(_chessPieces, srcRow * _pieceWidth, srcCol * _pieceHeight, _pieceWidth, _pieceHeight, (tileX + _tableOffsetX) * _size, tileY * _size, _size, _size);
        } else {
            _ctx->putImageData(_chessResizedPieces, (tileX + _tableOffsetX) * _size, tileY * _size, srcRow * _size, srcCol * _size, _size, _size);
        }
        _ctx->restore();
    }

    uint _size;
    int _tableOffsetX;

    gepard::Surface* _surface;
    gepard::Gepard* _ctx;

    gepard::Image _chessResizedPieces;
    gepard::Image _chessPieces = gepard::utils::loadPng("apps/examples/image/ChessPiecesArray.png");
    uint32_t _pieceWidth = _chessPieces.width() / 6;
    uint32_t _pieceHeight = _chessPieces.height() / 2;
    gepard::Image _chessDarkTile = gepard::utils::loadPng("apps/examples/image/Chess_d44.png");
    gepard::Image _chessLightTile = gepard::utils::loadPng("apps/examples/image/Chess_l44.png");
    std::string _table = "RLBKQBNRPPPPPPPP                                pppppppprlbkqbnr";
    std::string _whitePieces, _blackPieces;
};

int main(int argc, char* argv[])
{
    const std::string theLaskerTrap("1. d2-d4 d7-d5 2. c2-c4 e7-e5 3. d4xe5 d5-d4 4. e2-e3 Bf8-b4+ 5. Bc1-d2 d4xe3 6. Bd2xb4");
    const std::string crazyPeasants("1. a2xa8 h7xh1 2. a8xb8 h1xg1 3. b8xc8 g1xf1 4. c8xd8 f1xe1 5. d8xe8 e1xd1 6. e8xf8 d1xc1 7. f8xg8 c1xb1 8. g8xh8 b1xa1 9. h8xa7 a1xh2 10. a7xb7 h2xg2 11. b7xc7 g2xf2 12. c7xd7 f2xe2 13. d7xe7 e2xd2 14. e7xf7 d2xc2 15. f7xg7 c2xb2");

    const std::string moves = (argc > 1) ? std::string(argv[1]) : theLaskerTrap;

    ChessBoard chg(50);

    chg.doSteps(moves);

    chg.savePosition("build/chess.png");

    return 0;
}
