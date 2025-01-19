#include "Player.h"
#include "Connect4DAO.h"
#include <QDebug>

// Constructor privado

Player::Player(const QString& nickName, const QString& email, const QString& password,
               const QDate& birthdate, int points, const QImage& avatar)
    : nickName(nickName), email(email), password(password), birthdate(birthdate), points(points), avatar(avatar) {}


// Métodos de acceso
QString Player::getNickName() const {
    return nickName;
}

QString Player::getEmail() const {
    return email;
}

void Player::setEmail(const QString& email) {
    this->email = email;
    updateDatabase();
}

QString Player::getPassword() const {
    return password;
}

void Player::setPassword(const QString& password) {
    this->password = password;
    updateDatabase();
}

QImage Player::getAvatar() const {
    return avatar;
}

void Player::setAvatar(const QImage& avatar) {
    this->avatar = avatar;
    updateDatabase();
}

QDate Player::getBirthdate() const {
    return birthdate;
}

int Player::getPoints() const {
    return points;
}

void Player::addPoints(int points) {
    this->points += points;
    updateDatabase();
}

// Persistencia de cambios
void Player::updateDatabase() {
    if (!Connect4DAO::getInstance().addPlayer(*this)) {
        qWarning() << "Error al actualizar el jugador en la base de datos.";
    }
}
