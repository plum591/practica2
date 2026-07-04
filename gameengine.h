#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <string>
#include <vector>
#include "difficulty.h"
#include "playermanager.h"
#include "worddictionary.h"

class GameEngine {
public:
    GameEngine();

    // Игровая сессия
    void startSession(Difficulty difficulty, int targetScore = 20);
    bool isSessionActive() const;            // идёт ли сейчас игра

    // Доступ к менеджеру игроков (добавить/удалить/переименовать игроков до старта).
    PlayerManager& players();

    // Ход/раунд
    void startTurn();

    int currentRound() const;                // номер текущего раунда
    int currentArtistIndex() const;          // номер игрока-художника
    std::string currentArtistName() const;   // имя художника

    // Три случайных слова, которые показываем художнику
    std::vector<std::string> wordChoices() const;

    // Художник выбрал слово
    void chooseWord(int wordIndex);
    std::string chosenWord() const;          // загаданное слово

    // Таймер 60 сек
    void startTimer(int seconds = 60);
    void tick();                             // вызывать раз в секунду из QTimer
    int secondsLeft() const;                 // сколько секунд осталось
    bool isTimeUp() const;                   // время вышло?
    void endTurnEarly();                     // досрочно завершить ход

    // Очки
    void addPoints(int playerIndex, int points);

    // Конец
    bool isGameOver() const;                 // кто-то набрал нужные очки?
    int winnerIndex() const;                 // номер победителя (или -1)
    std::string winnerName() const;          // имя победителя

    // Сброс игры
    void resetGame();

private:
    PlayerManager  m_players;     // игроки
    WordDictionary m_dictionary;  // словарь слов
    Difficulty     m_difficulty;  // выбранный режим
    int            m_targetScore; // сколько очков нужно для победы (20)

    bool m_sessionActive;         // идёт ли игра
    int  m_round;                 // номер раунда (1, 2, 3, ...)
    int  m_artistIndex;           // кто сейчас рисует

    std::vector<std::string> m_wordChoices; // три слова на выбор
    std::string m_chosenWord;               // выбранное слово

    int  m_secondsLeft;           // осталось секунд на таймере
};

#endif
