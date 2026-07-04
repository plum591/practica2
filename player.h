#ifndef PLAYER_H
#define PLAYER_H

#include <string>

class Player {
public:
    // Создаём игрока с именем. Счёт в начале всегда 0.
    Player(const std::string& name);

    // Имя
    std::string name() const;                 // получить имя
    void setName(const std::string& name);    // переименовать

    // Счёт
    int score() const;                 // текущий счёт
    void addPoints(int points);        // прибавить (или отнять) очки
    void resetScore();                 // обнулить счёт (для новой игры)

private:
    std::string m_name;   // имя игрока
    int m_score;          // счёт игрока
};

#endif