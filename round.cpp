#include "Round.h"

// Constructor privado
Round::Round(const QDateTime& timestamp, Player* winner, Player* loser)
    : timestamp(timestamp), winner(winner), loser(loser) {}

QDateTime Round::getTimestamp() const {
    return timestamp;
}

Player* Round::getWinner() const {
    return winner;
}

Player* Round::getLoser() const {
    return loser;
}
