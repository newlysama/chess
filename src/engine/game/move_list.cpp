/**
 * @file      move_list.cpp
 * @author    Thibault THOMAS
 * @copyright Copyright 2025 Better Chess Engine
 * @par       This project is released under the MIT License
 *
 * @brief Implements MoveList class.
 */

#include "move_list.h"

#include <assert.h>
#include <cmath>

#include "engine/board/magic_const.h"
#include "engine/board/mask.h"
#include "logging/logging.h"
#include "utils/enums_to_string.h"
#include "utils/utils.h"

namespace engine::game
{
    using namespace engine::core;
    using namespace engine::board;

    MoveList::MoveList() noexcept
        : _m_moves{}
    {
    }

    void MoveList::add(const Move& move) noexcept
    {
        assert(_m_size < 256);
        _m_moves[_m_size++] = move;
    }

    void MoveList::clear() noexcept
    {
        _m_size = 0;
    }

    std::size_t MoveList::size() const noexcept
    {
        return _m_size;
    }

    Move& MoveList::operator[](std::size_t index) noexcept
    {
        return _m_moves[index];
    }

    bool MoveList::contains(const Move& other) const noexcept
    {
        for (std::size_t i = 0; i < _m_size; i++)
        {
            if (other == _m_moves[i])
            {
                return true;
            }
        }

        return false;
    }

    game::Move MoveList::find(const int fromSquare, const int toSquare) const noexcept
    {
        for (std::size_t i = 0; i < _m_size; i++)
        {
            if (_m_moves[i].getFromSquare() == fromSquare && _m_moves[i].getToSquare() == toSquare)
            {
                return _m_moves[i];
            }
        }

        return Move{};
    }

    void MoveList::processTargets(const State& state, Bitboard& targets, const int fromSquare, const MoveType moveType,
                                  const Piece fromPiece) noexcept
    {
        if (fromPiece != Piece::KING)
        {
            // Filter pinned pieces moves
            Bitboard pinnedTargets = state.m_pinnedBB[state.m_sideToMove][fromSquare];
            if (!pinnedTargets.isEmpty())
            {
                targets &= pinnedTargets;
            }

            // If the king is in simple check, non-king pieces can only
            // capture checking pieces, and only move to square that
            // block checking sliding pieces (if any)
            if (state.m_isChecked)
            {
                if (moveType == MoveType::CAPTURE)
                {
                    targets &= (state.m_checkersBB | state.m_blockersBB);
                }
                else
                {
                    targets &= state.m_blockersBB;
                }
            }
        }

        while (targets.m_data)
        {
            int toSquare = targets.lsbIndex();

            // If move is a promotion, create the 4 types of it
            int toRank = State::getRankIndex(toSquare);
            if (fromPiece == Piece::PAWN && (toRank == 7 || toRank == 0)) [[unlikely]]
            {
                Move move{fromSquare, toSquare, moveType, fromPiece};

                move.setPromotionPiece(Piece::KNIGHT);
                this->add(move);

                move.setPromotionPiece(Piece::ROOK);
                this->add(move);

                move.setPromotionPiece(Piece::BISHOP);
                this->add(move);

                move.setPromotionPiece(Piece::QUEEN);
                this->add(move);
            }
            else
            {
                Move move{fromSquare, toSquare, moveType, fromPiece};
                this->add(std::move(move));
            }

            // Move to the next target
            targets.unset(toSquare);
        }
    }

    template <Castling Castling>
    void MoveList::getCastlingMoves(const State& state, int fromSquare) noexcept
    {
        // Can't castle
        if (!state.hasCastlingRight<Castling>())
        {
            return;
        }

        // Check that square between rook and king are free
        if ((state.m_allOccBB & CASTLING_BETWEEN_MASKS[Castling]).m_data != 0ULL)
            return;

        // Check that no square on the king's path is under attack
        if ((state.m_targetsBB & CASTLING_KING_PATH_MASKS[Castling]).m_data != 0ULL)
            return;

        Move castle{fromSquare, CASTLING_TO_SQUARE[Castling], MoveType::CASTLE, Piece::KING, Castling};
        this->add(std::move(castle));
    }

    void MoveList::getEnPassantMoves(const State& state, int fromSquare) noexcept
    {
        // Pawn must be on rank 5 (white) or 4 (black)
        int rankFrom = State::getRankIndex(fromSquare);
        if (!((state.m_sideToMove == Color::WHITE && rankFrom == 4) ||
              (state.m_sideToMove == Color::BLACK && rankFrom == 3)))
        {
            return;
        }

        // Can only En Passant in diagonal
        int fileFrom = State::getFileIndex(fromSquare);
        int fileEnPassant = State::getFileIndex(state.m_epSquare);
        if (std::abs(fileFrom - fileEnPassant) != 1)
        {
            return;
        }

        const int capturedSquare = (state.m_sideToMove == Color::WHITE ? state.m_epSquare - 8 : state.m_epSquare + 8);

        // Quick simulation that checks wether the En Passant is gonna leave the king in check
        auto kingInCheck = [&](const State& s, const int capturedSquare, const int fromSquare) -> bool {
            State tmp = s;
            Color enemyColor = tmp.getEnemyColor();

            tmp.movePiece(Piece::PAWN, fromSquare, tmp.m_epSquare);
            tmp.unsetPiece(enemyColor, Piece::PAWN, capturedSquare);

            tmp.computePinnedPieces();
            tmp.computeEnemyTargetedSquares();

            return tmp.m_isChecked;
        };

        if (kingInCheck(state, capturedSquare, fromSquare))
        {
            return;
        }

        this->add(Move{fromSquare, state.m_epSquare, MoveType::EN_PASSANT, Piece::PAWN});
    }

    void MoveList::generatePawnsMoves(const State& state) noexcept
    {
        Color enemyColor = state.getEnemyColor();

        Bitboard pawns = state.m_piecesBB[state.m_sideToMove][Piece::PAWN];

        // While there is bits set to 1
        while (pawns.m_data)
        {
            // Extract bit index to the next '1'
            int fromSquare = pawns.lsbIndex();

            // Move to the next pawn
            pawns.unset(fromSquare);

            // Rechable empty squares
            Bitboard pushTargets = PAWN_PUSHES_MASKS[state.m_sideToMove][fromSquare] & ~state.m_allOccBB;

            // Rechable capture square
            Bitboard captureTargets =
                PAWN_CAPTURES_MASKS[state.m_sideToMove][fromSquare] & state.m_teamsOccBB[enemyColor];

            this->processTargets(state, captureTargets, fromSquare, MoveType::CAPTURE, Piece::PAWN);
            this->processTargets(state, pushTargets, fromSquare, MoveType::QUIET, Piece::PAWN);

            // Generate double pushes if the square between start and dest is empty
            if ((PAWN_PUSHES_MASKS[state.m_sideToMove][fromSquare] & state.m_allOccBB) == 0)
            {
                Bitboard doublePushTargets =
                    PAWN_DOUBLE_PUSHES_MASKS[state.m_sideToMove][fromSquare] & ~state.m_allOccBB;

                this->processTargets(state, doublePushTargets, fromSquare, MoveType::DOUBLE_PUSH, Piece::PAWN);
            }

            // Generate enPassant if enabled
            if (state.m_epSquare != -1) [[unlikely]]
            {
                this->getEnPassantMoves(state, fromSquare);
            }
        }
    }

    void MoveList::generateKnightsMoves(const State& state) noexcept
    {
        Color enemyColor = state.getEnemyColor();

        Bitboard knights = state.m_piecesBB[state.m_sideToMove][Piece::KNIGHT];

        // While there is bits set to 1
        while (knights.m_data)
        {
            // Extract bit index to the next '1'
            int fromSquare = knights.lsbIndex();

            // Move to the next knight
            knights.unset(fromSquare);

            // Get the targets
            Bitboard targets = KNIGHT_ATTACKS_MASKS[fromSquare] & ~state.m_teamsOccBB[state.m_sideToMove];

            // Rechable empty squares
            Bitboard quietTargets = targets & ~state.m_teamsOccBB[enemyColor];

            // Rechable squares occupied by an enemy piece
            Bitboard captureTargets = targets & state.m_teamsOccBB[enemyColor];

            this->processTargets(state, captureTargets, fromSquare, MoveType::CAPTURE, Piece::KNIGHT);
            this->processTargets(state, quietTargets, fromSquare, MoveType::QUIET, Piece::KNIGHT);
        }
    }

    void MoveList::generateRooksMoves(const State& state) noexcept
    {
        Color enemyColor = state.getEnemyColor();

        Bitboard rooks = state.m_piecesBB[state.m_sideToMove][Piece::ROOK];

        // While there is bits set to 1
        while (rooks.m_data)
        {
            // Extract bit index to the next '1'
            int fromSquare = rooks.lsbIndex();

            // Move to the next rook
            rooks.unset(fromSquare);

            Bitboard relevantOcc = state.m_allOccBB & ROOK_RELEVANT_MASKS[fromSquare];

            // Index the attack table using magic bitboards
            size_t magicIndex = (relevantOcc.m_data * rookMagics[fromSquare].m_data) >> rookShifts[fromSquare];

            // Get the targets
            Bitboard targets = ROOK_ATTACKS_TABLE[fromSquare][magicIndex] & ~state.m_teamsOccBB[state.m_sideToMove];

            // Rechable empty squares
            Bitboard quietTargets = targets & ~state.m_teamsOccBB[enemyColor];

            // Rechable squares occupied by an enemy piece
            Bitboard captureTargets = targets & state.m_teamsOccBB[enemyColor];

            this->processTargets(state, captureTargets, fromSquare, MoveType::CAPTURE, Piece::ROOK);
            this->processTargets(state, quietTargets, fromSquare, MoveType::QUIET, Piece::ROOK);
        }
    }

    void MoveList::generateBishopsMoves(const State& state) noexcept
    {
        Color enemyColor = state.getEnemyColor();

        Bitboard bishops = state.m_piecesBB[state.m_sideToMove][Piece::BISHOP];

        // While there is bits set to 1
        while (bishops.m_data)
        {
            // Extract bit index to the next '1'
            int fromSquare = bishops.lsbIndex();

            // Move to the next bishop
            bishops.unset(fromSquare);

            Bitboard relevantOcc = state.m_allOccBB & BISHOP_RELEVANT_MASKS[fromSquare];

            // Index the attack table using magic bitboards
            size_t magicIndex = (relevantOcc.m_data * bishopMagics[fromSquare].m_data) >> bishopShifts[fromSquare];

            // Get the targets
            Bitboard targets = BISHOP_ATTACKS_TABLE[fromSquare][magicIndex] & ~state.m_teamsOccBB[state.m_sideToMove];

            // Rechable empty squares
            Bitboard quietTargets = targets & ~state.m_teamsOccBB[enemyColor];

            // Rechable squares occupied by an enemy piece
            Bitboard captureTargets = targets & state.m_teamsOccBB[enemyColor];

            this->processTargets(state, captureTargets, fromSquare, MoveType::CAPTURE, Piece::BISHOP);
            this->processTargets(state, quietTargets, fromSquare, MoveType::QUIET, Piece::BISHOP);
        }
    }

    void MoveList::generateQueenMoves(const State& state) noexcept
    {
        Color enemyColor = state.getEnemyColor();

        Bitboard queen = state.m_piecesBB[state.m_sideToMove][Piece::QUEEN];

        // While there is bits set to 1
        // Remember that thank's to promotions, we can have several queens alive
        while (queen.m_data)
        {
            // Extract queen's index
            int fromSquare = queen.lsbIndex();

            // Remove queen's bit
            queen.unset(fromSquare);

            // Magic rook
            Bitboard rookRelevantOcc = state.m_allOccBB & ROOK_RELEVANT_MASKS[fromSquare];
            size_t rookMagicIndex = (rookRelevantOcc.m_data * rookMagics[fromSquare].m_data) >> rookShifts[fromSquare];

            // Magic bishop
            Bitboard bishopRelevantOcc = state.m_allOccBB & BISHOP_RELEVANT_MASKS[fromSquare];
            size_t bishopMagicIndex =
                (bishopRelevantOcc.m_data * bishopMagics[fromSquare].m_data) >> bishopShifts[fromSquare];

            // Combine rooks and bishops attack tables
            Bitboard attacks =
                ROOK_ATTACKS_TABLE[fromSquare][rookMagicIndex] | BISHOP_ATTACKS_TABLE[fromSquare][bishopMagicIndex];

            Bitboard targets = attacks & ~state.m_teamsOccBB[state.m_sideToMove];
            Bitboard quietTargets = targets & ~state.m_teamsOccBB[enemyColor];
            Bitboard captureTargets = targets & state.m_teamsOccBB[enemyColor];

            this->processTargets(state, captureTargets, fromSquare, MoveType::CAPTURE, Piece::QUEEN);
            this->processTargets(state, quietTargets, fromSquare, MoveType::QUIET, Piece::QUEEN);
        }
    }

    void MoveList::generateKingMoves(const State& state) noexcept
    {
        Color enemyColor = state.getEnemyColor();

        Bitboard king = state.m_piecesBB[state.m_sideToMove][Piece::KING];

        // Extract king's index
        int fromSquare = king.lsbIndex();

        // Generate classic targets
        Bitboard targets =
            KING_ATTACKS_MASKS[fromSquare] & ~state.m_teamsOccBB[state.m_sideToMove] & ~state.m_targetsBB;
        Bitboard quietTargets = targets & ~state.m_teamsOccBB[enemyColor];
        Bitboard captureTargets = targets & state.m_teamsOccBB[enemyColor];

        // Generate Castling targets
        if (state.m_sideToMove == Color::WHITE && !state.m_isChecked)
        {
            getCastlingMoves<Castling::WHITE_KING_SIDE>(state, fromSquare);
            getCastlingMoves<Castling::WHITE_QUEEN_SIDE>(state, fromSquare);
        }
        else if (state.m_sideToMove == Color::BLACK && !state.m_isChecked)
        {
            getCastlingMoves<Castling::BLACK_KING_SIDE>(state, fromSquare);
            getCastlingMoves<Castling::BLACK_QUEEN_SIDE>(state, fromSquare);
        }

        this->processTargets(state, captureTargets, fromSquare, MoveType::CAPTURE, Piece::KING);
        this->processTargets(state, quietTargets, fromSquare, MoveType::QUIET, Piece::KING);
    }

    void MoveList::generateAllMoves(State& state) noexcept
    {
        // Clear the actual list
        this->clear();

        // Computes pinned pieces
        state.computePinnedPieces();

        // Computes enemy targeted squares
        state.computeEnemyTargetedSquares();

        if (state.m_isDoubleChecked == false)
        {
            this->generatePawnsMoves(state);
            this->generateKnightsMoves(state);
            this->generateRooksMoves(state);
            this->generateBishopsMoves(state);
            this->generateQueenMoves(state);
        }

        this->generateKingMoves(state);

        // GAME OVER BABY
        state.m_isCheckMate = state.m_isChecked && (_m_size == 0);
    }
} // namespace engine::game
