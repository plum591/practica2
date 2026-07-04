#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QPushButton>
#include "gameengine.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

protected:
    void resizeEvent(QResizeEvent* event) override;

private slots:
    // Экран 0 — главное меню
    void onStart();
    void onExit();

    // Экран 1 — создание игры
    void onAddPlayer();
    void onRemovePlayer();
    void onStartGame();

    // Экран 2 — выбор слова
    void onConfirmWord();

    // Экран 3 — рисование
    void onTimerTick();
    void onFinish();
    void onClearCanvas();
    void onHideWord();

    // Экран 4 — расчёт очков
    void onContinue();

    // Экран 5 — конец игры
    void onNewGame();

    // Плавающие кнопки (на всех экранах)
    void onShowRules();
    void onOpenMenu();

    // Экран меню паузы (страница 6)
    void onMenuReset();
    void onMenuExit();
    void onMenuOk();

private:
    // Вспомогательные методы
    void showWordScreen();
    void showDrawScreen();
    void showScoreScreen();
    void updateTimerLabel();
    void buildPalette();

    Ui::MainWindow* ui;
    GameEngine  m_game;
    QTimer*     m_timer;

    // Плавающие кнопки поверх всех экранов
    QPushButton* m_btnMenu;
    QPushButton* m_btnHelp;

    int  m_previousPage;   // куда вернуться из меню паузы
    bool m_wordHidden;     // скрыто ли слово на экране рисования
};

#endif
