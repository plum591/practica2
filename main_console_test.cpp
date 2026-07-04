// Консольная проверка логики игры (БЕЗ графики).
// Запустите её, чтобы убедиться, что классы работают правильно,
// прежде чем строить интерфейс в Qt Creator.
//
// Компиляция вручную:
//   g++ -std=c++17 main_console_test.cpp player.cpp playermanager.cpp \
//       worddictionary.cpp gameengine.cpp -o test
//   ./test

#include <iostream>
#include "gameengine.h"

int main() {
    GameEngine game;

    // 1) Добавляем игроков (как на экране "Создание игры").
    game.players().addPlayer("Аня");
    game.players().addPlayer("Боря");
    game.players().addPlayer("Вика");

    // 2) Стартуем сессию: средняя сложность, до 20 очков.
    game.startSession(Difficulty::Medium, 20);

    std::cout << "=== Игра 'Крокодил' (тест логики) ===\n";

    // Играем раунды, пока никто не победил.
    while (!game.isGameOver()) {
        game.startTurn();

        std::cout << "\nРаунд " << game.currentRound()
                  << " | Рисует: " << game.currentArtistName() << "\n";

        // Показываем 3 слова на выбор.
        std::vector<std::string> choices = game.wordChoices();
        std::cout << "Слова на выбор: ";
        for (int i = 0; i < (int)choices.size(); i++) {
            std::cout << (i + 1) << ") " << choices[i] << "  ";
        }
        std::cout << "\n";

        // Художник "выбирает" первое слово.
        game.chooseWord(0);
        std::cout << "Загадано слово: " << game.chosenWord() << "\n";

        // Запускаем таймер на 60 секунд и "проматываем" его до нуля.
        game.startTimer(60);
        while (!game.isTimeUp()) {
            game.tick();   // в реальной игре это делает QTimer раз в секунду
        }
        std::cout << "Время вышло. Осталось секунд: " << game.secondsLeft() << "\n";

        // Игроки решают: слово угадали. Художнику и угадавшему по +3 (ТЗ 3.1.10/3.1.11).
        int artist = game.currentArtistIndex();
        int guesser = (artist + 1) % game.players().count(); // просто следующий игрок
        game.addPoints(artist, 3);
        game.addPoints(guesser, 3);

        // Печатаем таблицу очков.
        std::cout << "Счёт: ";
        for (int i = 0; i < game.players().count(); i++) {
            std::cout << game.players().player(i).name() << "="
                      << game.players().player(i).score() << "/20  ";
        }
        std::cout << "\n";
    }

    std::cout << "\n*** Победил игрок: " << game.winnerName() << " ***\n";
    return 0;
}
