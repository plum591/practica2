#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QPushButton>
#include <QString>
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
    void onCreate();
    void onJoin();
    void onExit();

    void onNickContinue();

    void onRefreshLobbies();
    void onJoinConnect();

    void onRemovePlayer();
    void onStartGame();

    void onConfirmWord();

    void onTimerTick();
    void onFinish();
    void onClearCanvas();
    void onHideWord();

    void onContinue();

    void onNewGame();

    void onShowRules();
    void onOpenMenu();

    void onMenuReset();
    void onMenuExit();
    void onMenuOk();

private:
    void showWordScreen();
    void showDrawScreen();
    void showScoreScreen();
    void updateTimerLabel();
    void buildPalette();

    void goToPage(QWidget* page);

    Ui::MainWindow* ui;
    GameEngine  m_game;
    QTimer*     m_timer;

    QPushButton* m_btnMenu;
    QPushButton* m_btnHelp;

    int  m_previousPage;
    bool m_wordHidden;

    bool    m_joining;
    QString m_nickname;
};

#endif