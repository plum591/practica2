#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QPushButton>
#include <QString>
#include "gameengine.h"
#include "networkmanager.h"
#include <QStringList>
#include <QList>

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

    void onPeerConnected();
    void onPeerDisconnected(const QString& nick);
    void onNetMessage(const QString& text);
    void onNetError(const QString& text);

private:
    void showWordScreen();
    void showDrawScreen();
    void setDrawingControlsEnabled(bool on);
    void showScoreScreen();
    void updateTimerLabel();
    void buildPalette();

    void goToPage(QWidget* page);

    Ui::MainWindow* ui;
    GameEngine  m_game;
    NetworkManager* m_net;
    QTimer*     m_timer;

    QPushButton* m_btnMenu;
    QPushButton* m_btnHelp;

    int  m_previousPage;
    bool m_wordHidden;

    bool    m_joining;
    QString m_nickname;
    QStringList m_players;
    Difficulty  m_difficulty;
    int         m_artistIndex;
    int         m_round = 0;

    bool amIArtist() const;
    void hostStartTurn();
    void applyTurn(int artistIndex, int round);
    QStringList m_lastWords;
    QList<int> m_scores;
    void applyScores(const QString& data);
    void hostBroadcastScores();
};

#endif