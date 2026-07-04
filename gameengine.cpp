#include "gameengine.h"

GameEngine::GameEngine()
    : m_difficulty(Difficulty::Easy),
      m_targetScore(20),
      m_sessionActive(false),
      m_round(0),
      m_artistIndex(0),
      m_secondsLeft(0)
{
}

void GameEngine::startSession(Difficulty difficulty, int targetScore) {
    m_difficulty   = difficulty;
    m_targetScore  = targetScore;
    m_sessionActive = true;
    m_round = 0;
    m_artistIndex = 0;
    m_players.resetAllScores();
}

bool GameEngine::isSessionActive() const {
    return m_sessionActive;
}

PlayerManager& GameEngine::players() {
    return m_players;
}

// ХОД / РАУНД

void GameEngine::startTurn() {
    m_round++;   // переходим к следующему раунду

    // Художник определяется порядком регистрации игроков
    // Раунд 1 -> игрок 0, раунд 2 -> игрок 1, и по кругу.
    if (m_players.count() > 0) {
        m_artistIndex = (m_round - 1) % m_players.count();
    } else {
        m_artistIndex = 0;
    }

    // Получаем 3 слова на выбор для текущей сложности.
    m_wordChoices = m_dictionary.getRandomWords(m_difficulty, 3);
    m_chosenWord = "";
}

int GameEngine::currentRound() const {
    return m_round;
}

int GameEngine::currentArtistIndex() const {
    return m_artistIndex;
}

std::string GameEngine::currentArtistName() const {
    if (m_artistIndex >= 0 && m_artistIndex < m_players.count()) {
        return m_players.player(m_artistIndex).name();
    }
    return "";
}

std::vector<std::string> GameEngine::wordChoices() const {
    return m_wordChoices;
}

void GameEngine::chooseWord(int wordIndex) {
    if (wordIndex >= 0 && wordIndex < (int)m_wordChoices.size()) {
        m_chosenWord = m_wordChoices[wordIndex];
    }
}

std::string GameEngine::chosenWord() const {
    return m_chosenWord;
}

// ТАЙМЕР

void GameEngine::startTimer(int seconds) {
    m_secondsLeft = seconds;
}

void GameEngine::tick() {
    if (m_secondsLeft > 0) {
        m_secondsLeft--;
    }
}

int GameEngine::secondsLeft() const {
    return m_secondsLeft;
}

bool GameEngine::isTimeUp() const {
    return m_secondsLeft <= 0;
}

void GameEngine::endTurnEarly() {
    m_secondsLeft = 0;   // как будто время вышло -> ход завершается
}

// ОЧКИ

void GameEngine::addPoints(int playerIndex, int points) {
    m_players.addPoints(playerIndex, points);
}

// КОНЕЦ ИГРЫ

bool GameEngine::isGameOver() const {
    return m_players.findWinner(m_targetScore) != -1;
}

int GameEngine::winnerIndex() const {
    return m_players.findWinner(m_targetScore);
}

std::string GameEngine::winnerName() const {
    int index = winnerIndex();
    if (index != -1) {
        return m_players.player(index).name();
    }
    return "";
}

void GameEngine::resetGame() {
    m_sessionActive = false;
    m_round = 0;
    m_artistIndex = 0;
    m_secondsLeft = 0;
    m_chosenWord = "";
    m_wordChoices.clear();
    m_players.resetAllScores();

    while (m_players.count() > 0) {
        m_players.removePlayer(0);
    }
}
