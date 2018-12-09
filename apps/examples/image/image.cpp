/* Copyright (C) 2018, Gepard Graphics
 * Copyright (C) 2018, Kristof Kosztyo <kkristof@inf.u-szeged.hu>
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
#include <iostream>
#include <sstream>

#include <unistd.h>

class ChessGame {
public:
    ChessGame(const uint size = 50)
        : _size(size)
        , _surface(new gepard::Surface(12 * size, 8 * size))
        , _ctx(new gepard::Gepard(_surface))
    {
        PiecesType ps[14] = { Dark_Queen, Dark_King, Dark_Rook, Dark_KnightLeft, Dark_Bishop, Dark_Pawn, Dark_KnightRight,
                              Light_Queen, Light_King, Light_Rook, Light_KnightLeft, Light_Bishop, Light_Pawn, Light_KnightRight };

        for (int i = 0; i < 14; ++i) {
            putPiece(ps[i], i % 7, i / 7);
        }

        _chessResizedPieces = _ctx->getImageData(2 * _size, 0, 7 * _size, 2 * _size);
        gepard::utils::savePng(_chessResizedPieces, "c.png");

        generateCheckerBoard();
    }

    ~ChessGame()
    {
        if (_ctx) {
            delete _ctx;
        }
        if (_surface) {
            delete _surface;
        }
    }

    void move(const std::string& mv)
    {
        std::stringstream ss; ss << mv;
        std::string l;
        int i = 0;
        while (std::getline(ss, l, ' ')) {
            i++;
            std::string file("build/step");
            if (std::getline(ss, l, ' ')) {
                move(l, true);
            }
            if (std::getline(ss, l, ' ')) {
                move(l, false);
            }
            savePosition(file + std::to_string(i) + std::string(".png"));
        }
    }

    void savePosition(const std::string& fileName)
    {
        gepard::utils::savePng(_ctx->getImageData(0, 0, _surface->width(), _surface->height()), fileName);
    }

private:
    enum PiecesType
    {
        O = 0,
        Light_King = 20,
        Light_Queen = 10,
        Light_Rook = 30,
        Light_KnightLeft = 40,
        Light_KnightRight = 41,
        Light_Bishop = 50,
        Light_Pawn = 60,
        Dark_King = 26,
        Dark_Queen = 16,
        Dark_Rook = 36,
        Dark_KnightLeft = 46,
        Dark_KnightRight = 47,
        Dark_Bishop = 56,
        Dark_Pawn = 66,
    };

 //"1. a2xa8 h7xh1 2. a8xb8 h1xg1 3. b8xc8 g1xf1 4. c8xd8 f1xe1 5. d8xe8 e1xd1 6. e8xf8 d1xc1 7. f8xg8 c1xb1 8. g8xh8 b1xa1 2. h8xa7 a1xh2 3. a7xb7 h2xg2 3. b7xc7 g2xf2 4. c7xd7 f2xe2 5. d7xe7 e2xd2 6. e7xf7 d2xc2 7. f7xg7 c2xb2"

    void updateTable()
    {
        for (int i = 0; i < 64; ++i) {
            putPiece(_table[i], i % 8, i / 8);
        }
        for (size_t i = 0; i < _lightPieces.size(); ++i) {
            putPiece(_lightPieces[i], -(int(i) + 8) / 8, int(i) % 8, false);
        }
        for (size_t i = 0; i < _darkPieces.size(); ++i) {
            putPiece(_darkPieces[i], int(i) / 8 + 8, 7 - (int(i) % 8), false);
        }
    }

    void move(const std::string& mv, const bool isLight)
    {
        int f = 1;
        PiecesType piecesType = O;
        if (mv[0] == 'K') {
            piecesType = isLight ? Light_King : Dark_King;
        } else if (mv[0] == 'Q') {
            piecesType = isLight ? Light_Queen : Dark_Queen;
        } else if (mv[0] == 'R') {
            piecesType = isLight ? Light_Rook : Dark_Rook;
        } else if (mv[0] == 'B') {
            piecesType = isLight ? Light_Bishop : Dark_Bishop;
        } else if (mv[0] == 'K') {
            piecesType = isLight ? Light_KnightLeft : Dark_KnightLeft ;
        } else if (mv[0] == 'L') {
            piecesType = isLight ? Light_KnightRight : Dark_KnightRight;
        } else {
            piecesType = isLight ? Light_Pawn : Dark_Pawn;
            f = 0;
        }

        if (mv[f + 2] == 'x') {
            PiecesType oldPiece = _table[7 - (mv[f + 3] - 'a') + 8 * (mv[f + 4] - '1')];
            if (isLight) {
                _darkPieces.insert(_darkPieces.begin(), oldPiece);
            } else {
                _lightPieces.insert(_lightPieces.begin(), oldPiece);
            }
            putBoardTile(7 - (mv[f + 3] - 'a'), (mv[f + 4] - '1'));
        }
        _table[7 - (mv[f] - 'a') + 8 * (mv[f + 1] - '1')] = O;
        _table[7 - (mv[f + 3] - 'a') + 8 * (mv[f + 4] - '1')] = piecesType;
        updateTable();
    }

    void generateCheckerBoard()
    {
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                putBoardTile(i, j);
            }
        }
    }

    void putBoardTile(const uint row, const uint column)
    {
        gepard::Image* tileImage = &_chessLightTile;
        if ((row + column) % 2) {
            tileImage = &_chessDarkTile;
        }
        _ctx->drawImage(*tileImage, 0, 0, tileImage->width(), tileImage->height(), (row + 2) * _size, column * _size, _size, _size);
    }

    void putPiece(const PiecesType pieceType, const uint row, const uint column, const bool cmdDraw = true)
    {
        if (pieceType == O) {
            putBoardTile(row, column);
            return;
        }
        int r = pieceType / 10 - 1;
        int c = (pieceType % 10) > 5 ? 0 : 1;
        _ctx->save();
        if (pieceType % 2) {
            _ctx->translate(2 * (row + 2) * _size + _size, 0);
            _ctx->scale(-1, 1);
        }
        _ctx->save();
        if (c) {
            _ctx->translate(2 * (row + 2) * _size + _size, 0);
            _ctx->scale(-1, 1);
            _ctx->translate(2 * (row + 2) * _size + _size, 2 * (column) * _size + _size);
            _ctx->rotate(3.1415926);
        }
        if (cmdDraw) {
            _ctx->drawImage(_chessPieces, r * _pieceWidth, c * _pieceHeight, _pieceWidth, _pieceHeight, (row + 2) * _size, column * _size, _size, _size);
        } else {
            if (pieceType % 2) {
                r = 6;
            }
            _ctx->putImageData(_chessResizedPieces, (row + 2) * _size, column * _size, r * _size, c * _size, _size, _size);
        }
        _ctx->restore();
        _ctx->restore();
    }

    uint _size;

    gepard::Surface* _surface;
    gepard::Gepard* _ctx;

    gepard::Image _chessResizedPieces;
    gepard::Image _chessPieces = gepard::utils::loadPng("apps/examples/image/ChessPiecesArray.png");
    gepard::Image _chessDarkTile = gepard::utils::loadPng("apps/examples/image/Chess_d44.png");
    gepard::Image _chessLightTile = gepard::utils::loadPng("apps/examples/image/Chess_l44.png");
    uint32_t _pieceWidth = _chessPieces.width() / 6;
    uint32_t _pieceHeight = _chessPieces.height() / 2;
    PiecesType _table[64] = {
        Light_Rook, Light_KnightRight, Light_Bishop, Light_King, Light_Queen, Light_Bishop, Light_KnightLeft, Light_Rook,
        Light_Pawn, Light_Pawn, Light_Pawn, Light_Pawn, Light_Pawn, Light_Pawn, Light_Pawn, Light_Pawn,
        O, O, O, O, O, O, O, O,
        O, O, O, O, O, O, O, O,
        O, O, O, O, O, O, O, O,
        O, O, O, O, O, O, O, O,
        Dark_Pawn, Dark_Pawn, Dark_Pawn, Dark_Pawn, Dark_Pawn, Dark_Pawn, Dark_Pawn, Dark_Pawn,
        Dark_Rook, Dark_KnightRight, Dark_Bishop, Dark_King, Dark_Queen, Dark_Bishop, Dark_KnightLeft, Dark_Rook
    };
    std::vector<PiecesType> _lightPieces, _darkPieces;
};

int main(int argc, char* argv[])
{
    const std::string theLaskerTrap("1. d2-d4 d7-d5 2. c2-c4 e7-e5 3. d4xe5 d5-d4 4. e2-e3 Bf8-b4+ 5. Bc1-d2 d4xe3 6. Bd2xb4");

    const uint scale = (argc > 1) ? std::atoi(argv[1]) : 50;
    const std::string moves = (argc > 2) ? std::string(argv[2]) : theLaskerTrap;

    ChessGame chg(scale);

    chg.move(moves);

    chg.savePosition("build/chess.png");

    return 0;
}
