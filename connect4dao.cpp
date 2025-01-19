#include "Connect4DAO.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QBuffer>
#include <QDebug>

Connect4DAO& Connect4DAO::getInstance() {
    static Connect4DAO instance;
    return instance;
}

bool Connect4DAO::initializeDatabase(const QString& dbName) {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbName);

    if (!db.open()) {
        qWarning() << "Error al abrir la base de datos:" << db.lastError().text();
        return false;
    }

    QSqlQuery query;
    if (!query.exec("CREATE TABLE IF NOT EXISTS players ("
                    "nickName TEXT PRIMARY KEY, "
                    "email TEXT, "
                    "password TEXT, "
                    "avatar BLOB, "
                    "birthdate TEXT, "
                    "points INTEGER) WITHOUT ROWID")) {
        qWarning() << "Error al crear la tabla 'players':" << query.lastError().text();
        return false;
    }

    if (!query.exec("CREATE TABLE IF NOT EXISTS rounds ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "timestamp TEXT, "
                    "winner TEXT, "
                    "loser TEXT, "
                    "FOREIGN KEY(winner) REFERENCES players(nickName), "
                    "FOREIGN KEY(loser) REFERENCES players(nickName))")) {
        qWarning() << "Error al crear la tabla 'rounds':" << query.lastError().text();
        return false;
    }

    return true;
}

bool Connect4DAO::addPlayer(const Player& player) {
    QSqlQuery query;
    query.prepare("INSERT OR REPLACE INTO players (nickName, email, password, avatar, birthdate, points) "
                  "VALUES (:nickName, :email, :password, :avatar, :birthdate, :points)");
    query.bindValue(":nickName", player.getNickName());
    query.bindValue(":email", player.getEmail());
    query.bindValue(":password", player.getPassword());

    QByteArray avatarData;
    QBuffer buffer(&avatarData);
    player.getAvatar().save(&buffer, "PNG");
    query.bindValue(":avatar", avatarData);

    query.bindValue(":birthdate", player.getBirthdate().toString(Qt::ISODate));
    query.bindValue(":points", player.getPoints());

    if (!query.exec()) {
        qWarning() << "Error al agregar o actualizar jugador:" << query.lastError().text();
        return false;
    }
    return true;
}

Player* Connect4DAO::getPlayer(const QString& nickName) {
    QSqlQuery query;
    query.prepare("SELECT * FROM players WHERE nickName = :nickName");
    query.bindValue(":nickName", nickName);

    if (!query.exec() || !query.next()) {
        qWarning() << "Error al recuperar jugador:" << query.lastError().text();
        return nullptr;
    }

    QString email = query.value("email").toString();
    QString password = query.value("password").toString();
    QDate birthdate = QDate::fromString(query.value("birthdate").toString(), Qt::ISODate);
    int points = query.value("points").toInt();

    QByteArray avatarData = query.value("avatar").toByteArray();
    QImage avatar;
    avatar.loadFromData(avatarData, "PNG");

    return new Player(nickName, email, password, birthdate, points, avatar);
}

bool Connect4DAO::addRound(const Round& round) {
    QSqlQuery query;
    query.prepare("INSERT INTO rounds (timestamp, winner, loser) "
                  "VALUES (:timestamp, :winner, :loser)");
    query.bindValue(":timestamp", round.getTimestamp().toString(Qt::ISODate));
    query.bindValue(":winner", round.getWinner()->getNickName());
    query.bindValue(":loser", round.getLoser()->getNickName());

    if (!query.exec()) {
        qWarning() << "Error al agregar partida:" << query.lastError().text();
        return false;
    }
    return true;
}

QList<Round*> Connect4DAO::getRoundsForPlayer(const QString& nickName) {
    QList<Round*> rounds;

    QSqlQuery query;
    query.prepare("SELECT timestamp, winner, loser FROM rounds WHERE winner = :nickName OR loser = :nickName");
    query.bindValue(":nickName", nickName);

    if (!query.exec()) {
        qWarning() << "Error al recuperar partidas:" << query.lastError().text();
        return rounds;
    }

    while (query.next()) {
        QDateTime timestamp = QDateTime::fromString(query.value("timestamp").toString(), Qt::ISODate);
        QString winnerName = query.value("winner").toString();
        QString loserName = query.value("loser").toString();

        Player* winner = getPlayer(winnerName); // Asume que los jugadores existen
        Player* loser = getPlayer(loserName);

        if (winner && loser) {
            rounds.append(new Round(timestamp, winner, loser));
        }
    }

    return rounds;
}

QList<Player*> Connect4DAO::getRanking() {
    QList<Player*> ranking;

    QSqlQuery query;
    if (!query.exec("SELECT * FROM players ORDER BY points DESC")) {
        qWarning() << "Error al recuperar ranking:" << query.lastError().text();
        return ranking;
    }

    while (query.next()) {
        QString nickName = query.value("nickName").toString();
        QString email = query.value("email").toString();
        QString password = query.value("password").toString();
        QDate birthdate = QDate::fromString(query.value("birthdate").toString(), Qt::ISODate);
        int points = query.value("points").toInt();

        QByteArray avatarData = query.value("avatar").toByteArray();
        QImage avatar;
        avatar.loadFromData(avatarData, "PNG");

        ranking.append(new Player(nickName, email, password, birthdate, points, avatar));
    }

    return ranking;
}

void Connect4DAO::clearAllData() {
    QSqlQuery query;
    if (!query.exec("DELETE FROM players") || !query.exec("DELETE FROM rounds")) {
        qWarning() << "Error al limpiar los datos de la base de datos:" << query.lastError().text();
    }
}
Connect4DAO::~Connect4DAO() {
    if (db.isOpen()) {
        db.close(); // Asegúrate de cerrar la base de datos al destruir la instancia
    }
}
