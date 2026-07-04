#include "player.h"

Player::Player(const std::string& name)
    : m_name(name), m_score(0)   // счёт стартует с нуля
{
}

std::string Player::name() const {
    return m_name;
}

void Player::setName(const std::string& name) {
    m_name = name;
}

int Player::score() const {
    return m_score;
}

void Player::addPoints(int points) {
    m_score += points;
    if (m_score < 0) {       // счёт не может быть отрицательным
        m_score = 0;
    }
}

void Player::resetScore() {
    m_score = 0;
}
