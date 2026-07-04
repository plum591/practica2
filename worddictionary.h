#ifndef WORDDICTIONARY_H
#define WORDDICTIONARY_H

#include <string>
#include <vector>
#include "difficulty.h"

// Словарь слов. Хранит три списка слов (по сложности) и выдаёт случайные слова.
// Соответствует компоненту "Word Dictionary" (Word List + Difficulty Selector).
class WordDictionary {
public:
    // В конструкторе сразу заполняем все списки слов.
    WordDictionary();

    // Выдать count случайных НЕповторяющихся слов нужной сложности.
    std::vector<std::string> getRandomWords(Difficulty difficulty, int count = 3);

private:
    // Вспомогательный метод: выбрать нужный список по сложности.
    const std::vector<std::string>& listFor(Difficulty difficulty) const;

    std::vector<std::string> m_easyWords;     // лёгкие слова
    std::vector<std::string> m_mediumWords;   // средние слова
    std::vector<std::string> m_hardWords;     // сложные слова
};

#endif // WORDDICTIONARY_H
