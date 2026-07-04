#include "worddictionary.h"
#include <algorithm>
#include <random>

WordDictionary::WordDictionary() {
    // Лёгкие слова
    m_easyWords = {
        "дом", "кот", "солнце", "рыба", "ключ", "ворон", "гриб",
        "мяч", "цветок", "звезда", "яблоко", "лист", "дерево", "рука", "глаз"
    };

    // Средние словаы
    m_mediumWords = {
        "велосипед", "телефон", "гитара", "замок", "ракета", "чемодан",
        "фонарь", "парус", "мельница", "маяк", "скрипка", "телескоп",
        "аквариум", "компас", "парашют"
    };

    // Сложные слова
    m_hardWords = {
        "эволюция", "гравитация", "философия", "демократия", "бесконечность",
        "ностальгия", "симметрия", "абстракция", "парадокс", "гипотеза",
        "перспектива", "инфляция", "дипломатия", "энтропия", "свобода"
    };
}

const std::vector<std::string>& WordDictionary::listFor(Difficulty difficulty) const {
    if (difficulty == Difficulty::Easy) {
        return m_easyWords;
    } else if (difficulty == Difficulty::Medium) {
        return m_mediumWords;
    } else {
        return m_hardWords;
    }
}

std::vector<std::string> WordDictionary::getRandomWords(Difficulty difficulty, int count) {
    std::vector<std::string> pool = listFor(difficulty);

    // Перемешиваем список в случайном порядке.
    static std::mt19937 generator(std::random_device{}());
    std::shuffle(pool.begin(), pool.end(), generator);

    // Берём первые 3 слова
    std::vector<std::string> result;
    for (int i = 0; i < count && i < (int)pool.size(); i++) {
        result.push_back(pool[i]);
    }
    return result;
}
