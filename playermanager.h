#ifndef PLAYERMANAGER_H
#define PLAYERMANAGER_H

#include <string>
#include <vector>
#include "player.h"

class PlayerManager {
public:
    void addPlayer(const std::string& name);                 // добавить игрока
    void removePlayer(int index);                            // удалить игрока по номеру

    int count() const;                       // сколько игроков
    Player& player(int index);               // получить игрока по номеру (для изменения)
    const Player& player(int index) const;   // получить игрока по номеру (только чтение)

    void addPoints(int index, int points);   // начислить/отнять очки игроку
    void resetAllScores();                   // обнулить очки всем (новая игра)

    // Возвращает номер первого игрока, набравшего targetScore и больше.
    // Если такого нет — возвращает -1.
    int findWinner(int targetScore) const;

private:
    std::vector<Player> m_players;   // список всех игроков
};

#endif