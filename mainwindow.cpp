#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QInputDialog>
#include <QMessageBox>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QResizeEvent>
#include <QNetworkInterface>
#include <QClipboard>

static QString myLocalIp() {
    for (const QHostAddress& addr : QNetworkInterface::allAddresses()) {
        if (addr.protocol() == QAbstractSocket::IPv4Protocol &&
            !addr.isLoopback()) {
            return addr.toString();
        }
    }
    return "не найден";
}


MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_previousPage(0),
    m_wordHidden(false),
    m_joining(false)
{
    ui->setupUi(this);
    this->setWindowTitle("Игра крокодил");

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &MainWindow::onTimerTick);

    m_net = new NetworkManager(this);
    connect(m_net, &NetworkManager::peerConnected,
            this, &MainWindow::onPeerConnected);
    connect(m_net, &NetworkManager::peerDisconnected,
            this, &MainWindow::onPeerDisconnected);
    connect(m_net, &NetworkManager::messageReceived,
            this, &MainWindow::onNetMessage);
    connect(m_net, &NetworkManager::errorOccurred,
            this, &MainWindow::onNetError);

    connect(ui->btnCreate, &QPushButton::clicked, this, &MainWindow::onCreate);
    connect(ui->btnJoin,   &QPushButton::clicked, this, &MainWindow::onJoin);
    connect(ui->btnExit,   &QPushButton::clicked, this, &MainWindow::onExit);

    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::onNickContinue);
    connect(ui->lineEdit, &QLineEdit::textChanged, this, [this](const QString& text) {
        ui->lblCharCount->setText(QString("%1/16").arg(text.length()));
    });

    connect(ui->pushButton_2, &QPushButton::clicked, this, &MainWindow::onJoinConnect);

    connect(ui->btnRemovePlayer, &QPushButton::clicked, this, &MainWindow::onRemovePlayer);
    connect(ui->btnStartGame,    &QPushButton::clicked, this, &MainWindow::onStartGame);
    connect(ui->btnCopyIp, &QPushButton::clicked, this, [this]() {
        QApplication::clipboard()->setText(myLocalIp());
        ui->btnCopyIp->setText("✓");
        QTimer::singleShot(1000, this, [this]() {
            ui->btnCopyIp->setText("📋");
        });
    });

    connect(ui->btnCopyPort, &QPushButton::clicked, this, [this]() {
        QApplication::clipboard()->setText("55555");
        ui->btnCopyPort->setText("✓");
        QTimer::singleShot(1000, this, [this]() {
            ui->btnCopyPort->setText("📋");
        });
    });

    connect(ui->btnConfirmWord,  &QPushButton::clicked, this, &MainWindow::onConfirmWord);

    connect(ui->btnFinish,   &QPushButton::clicked, this, &MainWindow::onFinish);
    connect(ui->btnClear,    &QPushButton::clicked, this, &MainWindow::onClearCanvas);
    connect(ui->btnHideWord, &QPushButton::clicked, this, &MainWindow::onHideWord);
    connect(ui->spinWidth, QOverload<int>::of(&QSpinBox::valueChanged),
            this, [this](int v){ ui->canvas->setPenWidth(v); });
    connect(ui->canvas, &DrawingCanvas::lineDrawn, this,
            [this](int x1, int y1, int x2, int y2, const QColor& color, int width) {
                m_net->sendMessage(QString("DRAW:%1;%2;%3;%4;%5;%6")
                .arg(x1).arg(y1).arg(x2).arg(y2).arg(color.name()).arg(width));
            });
    connect(ui->btnContinue, &QPushButton::clicked, this, &MainWindow::onContinue);

    connect(ui->btnNewGame,  &QPushButton::clicked, this, &MainWindow::onNewGame);

    connect(ui->btnMenuReset, &QPushButton::clicked, this, &MainWindow::onMenuReset);
    connect(ui->btnMenuExit,  &QPushButton::clicked, this, &MainWindow::onMenuExit);
    connect(ui->btnMenuOk,    &QPushButton::clicked, this, &MainWindow::onMenuOk);

    buildPalette();

    m_btnMenu = new QPushButton("≡", this);
    m_btnMenu->setFixedSize(26, 26);
    m_btnMenu->move(10, 11);
    m_btnMenu->setStyleSheet(
        "QPushButton { background-color: #557A48; color: white;"
        " font-size: 16pt; border-radius: 7px; border: none; padding: 0; }"
        "QPushButton:hover { background-color: #7FA86E; }");
    connect(m_btnMenu, &QPushButton::clicked, this, &MainWindow::onOpenMenu);

    m_btnHelp = new QPushButton("?", this);
    m_btnHelp->setFixedSize(26, 26);
    m_btnHelp->setStyleSheet(
        "QPushButton { background-color: #C8A96E; color: white;"
        " font-size: 14pt; font-weight: bold; border-radius: 13px; border: none; }"
        "QPushButton:hover { background-color: #E0C080; }");
    connect(m_btnHelp, &QPushButton::clicked, this, &MainWindow::onShowRules);


    goToPage(ui->page_1);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::goToPage(QWidget* page) {
    ui->stack->setCurrentIndex(ui->stack->indexOf(page));
}

void MainWindow::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);

    if (m_btnMenu) {
        m_btnMenu->move(10, 11);
        m_btnMenu->raise();
    }

    if (m_btnHelp) {
        int newX = this->width() - m_btnHelp->width() - 10;
        m_btnHelp->move(newX, 11);
        m_btnHelp->raise();
    }
}



void MainWindow::onCreate() {
    m_joining = false;
    ui->lineEdit->clear();
    goToPage(ui->page_2);
}

void MainWindow::onJoin() {
    m_joining = true;
    ui->lineEdit->clear();
    goToPage(ui->page_2);
}

void MainWindow::onExit() {
    close();
}


void MainWindow::onNickContinue() {
    QString nick = ui->lineEdit->text().trimmed();
    if (nick.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите никнейм!");
        return;
    }
    m_nickname = nick;
    if (m_joining) {
        onRefreshLobbies();
        goToPage(ui->page_join);
    } else {
        if (m_net->startHost(55555)) {
            ui->lblHostIp->setText("IP: " + myLocalIp());
            ui->lblHostPort->setText("порт: 55555");
        }
        m_game.resetGame();
        m_players.clear();
        m_players << m_nickname;
        ui->listPlayers->clear();
        ui->listPlayers->addItem(m_nickname + "  (хост)");
        for (QRadioButton* r : {ui->radioEasy, ui->radioMedium, ui->radioHard}) {
            r->setAutoExclusive(false);
            r->setChecked(false);
            r->setAutoExclusive(true);
        }
        goToPage(ui->page_lobby);
    }
}


void MainWindow::onRefreshLobbies() {
}

void MainWindow::onJoinConnect() {
    QString ip = ui->editIp->text().trimmed();
    QString portText = ui->editPort->text().trimmed();

    if (ip.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите IP-адрес хоста!");
        return;
    }

    quint16 port = 55555;
    if (!portText.isEmpty()) {
        port = portText.toUShort();
    }


    m_net->connectToHost(ip, port);
}

void MainWindow::onRemovePlayer() {
    if (!m_net->isHost()) {
        return;
    }

    int row = ui->listPlayers->currentRow();
    if (row <= 0) {
        QMessageBox::information(this, "Игроки", "Нельзя удалить хоста.");
        return;
    }

    if (row >= m_players.size()) {
        return;
    }

    QString nick = m_players[row];
    m_net->kickByNick(nick);
}

void MainWindow::onStartGame() {
    if (ui->listPlayers->count() < 2) {
        QMessageBox::warning(this, "Ошибка", "Нужно хотя бы 2 игрока!");
        return;
    }

    if (!m_net->isHost()) {
        QMessageBox::information(this, "Игра",
                                 "Начать игру может только хост.");
        return;
    }

    if (!ui->radioEasy->isChecked() &&
        !ui->radioMedium->isChecked() &&
        !ui->radioHard->isChecked()) {
        QMessageBox::warning(this, "Ошибка", "Выберите режим сложности!");
        return;
    }

    Difficulty diff = Difficulty::Easy;
    if (ui->radioMedium->isChecked()) diff = Difficulty::Medium;
    if (ui->radioHard->isChecked())   diff = Difficulty::Hard;

    m_difficulty = diff;

    m_game.resetGame();
    m_game.startSession(diff, 100);

    for (const QString& name : m_players) {
        m_game.players().addPlayer(name.toStdString());
    }

    int d = (diff == Difficulty::Easy) ? 0 : (diff == Difficulty::Medium ? 1 : 2);
    m_net->sendMessage("START:" + QString::number(d));

    hostStartTurn();
}


void MainWindow::showWordScreen() {
    QString header = QString(
                         "<table width='100%'>"
                         "<tr>"
                         "<td align='left'>Ход: %1</td>"
                         "<td align='right'>Раунд: %2</td>"
                         "</tr>"
                         "</table>")
                         .arg(QString::fromStdString(m_game.currentArtistName()))
                         .arg(m_game.currentRound());
    ui->lblWordHeader->setText(header);

    auto words = m_game.wordChoices();
    if (words.size() >= 3) {
        ui->radioWord0->setText(QString::fromStdString(words[0]));
        ui->radioWord1->setText(QString::fromStdString(words[1]));
        ui->radioWord2->setText(QString::fromStdString(words[2]));
    }
    for (QRadioButton* r : {ui->radioWord0, ui->radioWord1, ui->radioWord2}) {
        r->setAutoExclusive(false);
        r->setChecked(false);
        r->setAutoExclusive(true);
    }

    goToPage(ui->page_4);
}

void MainWindow::onConfirmWord() {
    if (!ui->radioWord0->isChecked() &&
        !ui->radioWord1->isChecked() &&
        !ui->radioWord2->isChecked()) {
        QMessageBox::warning(this, "Ошибка", "Выберите слово!");
        return;
    }

    int idx = 0;
    if (ui->radioWord1->isChecked()) idx = 1;
    if (ui->radioWord2->isChecked()) idx = 2;

    QString chosen = (idx == 0) ? ui->radioWord0->text(): (idx == 1) ? ui->radioWord1->text(): ui->radioWord2->text();

    m_net->sendMessage("WORD:" + chosen);

    showDrawScreen();
    ui->canvas->clear();
    ui->canvas->setDrawingEnabled(true);
    ui->lblWord->setText(chosen);
    m_wordHidden = false;
    ui->btnHideWord->setText("Скрыть");

    if (m_net->isHost()) {
        m_game.chooseWord(idx);
        m_game.startTimer(60);
        updateTimerLabel();
        m_timer->start(1000);
    } else {
        m_net->sendMessage("STARTTIMER:");
    }
}

void MainWindow::showDrawScreen() {
    QString header = QString(
                         "<table width='100%'>"
                         "<tr>"
                         "<td align='left'>Ход: %1</td>"
                         "<td align='right'>Раунд: %2</td>"
                         "</tr>"
                         "</table>")
                         .arg((m_artistIndex >= 0 && m_artistIndex < m_players.size()) ? m_players[m_artistIndex] : QString())
                         .arg(m_round);
    ui->lblWordHeader_2->setText(header);

    m_wordHidden = true;
    ui->btnHideWord->setText("Показать");
    ui->lblWord->setText("• • • • •");

    ui->canvas->clear();
    ui->canvas->setPenColor(Qt::black);
    ui->canvas->setPenWidth(ui->spinWidth->value());
    ui->canvas->setDrawingEnabled(amIArtist());
    setDrawingControlsEnabled(amIArtist());

    updateTimerLabel();

    goToPage(ui->page_6);
}

void MainWindow::onTimerTick() {
    m_game.tick();
    updateTimerLabel();

    m_net->sendMessage("TIME:" + QString::number(m_game.secondsLeft()));

    if (m_game.isTimeUp()) {
        m_timer->stop();
        m_net->sendMessage("TIMEUP:");
        hostBroadcastScores();
        showScoreScreen();
    }
}

void MainWindow::updateTimerLabel() {
    int s  = m_game.secondsLeft();
    int mm = s / 60;
    int ss = s % 60;
    ui->lblTimer->setText(
        QString("%1:%2").arg(mm, 2, 10, QChar('0')).arg(ss, 2, 10, QChar('0')));
}

void MainWindow::onFinish() {
    if (m_net->isHost()) {
        m_game.endTurnEarly();
        m_timer->stop();
        m_net->sendMessage("TIMEUP:");
        showScoreScreen();
    } else {
        m_net->sendMessage("FINISH:");
    }
}

void MainWindow::setDrawingControlsEnabled(bool on) {
    ui->btnHideWord->setEnabled(on);
    ui->btnFinish->setEnabled(on);
}

void MainWindow::onClearCanvas() {
    if (!amIArtist()) return;
    ui->canvas->clear();
    m_net->sendMessage("CLEAR:");
}

void MainWindow::onHideWord() {
    m_wordHidden = !m_wordHidden;
    if (m_wordHidden) {
        ui->lblWord->setText("• • • • •");
        ui->btnHideWord->setText("Показать");
    } else {
        ui->lblWord->setText(QString::fromStdString(m_game.chosenWord()));
        ui->btnHideWord->setText("Скрыть");
    }
}


void MainWindow::buildPalette() {
    QGridLayout* grid = new QGridLayout(ui->paletteArea);
    grid->setSpacing(5);
    grid->setContentsMargins(4, 4, 4, 4);

    QStringList colors = {
        "#000000", "#7F7F7F",
        "#D94F4F", "#E08E3C",
        "#E8C84F", "#4FAF5A",
        "#4F7FD9", "#7FD1E8",
        "#9B59B6", "#E08EC8",
        "#8B5A2B", "#FFFFFF"
    };

    int row = 0, col = 0;
    for (const QString& c : colors) {
        QPushButton* btn = new QPushButton();
        btn->setFixedSize(30, 30);
        btn->setStyleSheet(QString(
                               "QPushButton { background-color:%1; border-radius:15px;"
                               " border:2px solid #C8A96E; padding:0; }"
                               "QPushButton:hover { border:2px solid white; }").arg(c));
        connect(btn, &QPushButton::clicked, this, [this, c]() {
            ui->canvas->setPenColor(QColor(c));
            ui->canvas->setEraser(false);
        });
        grid->addWidget(btn, row, col);
        col++;
        if (col >= 2) { col = 0; row++; }
    }

    QPushButton* btnPencil = new QPushButton("✏ Карандаш");
    QPushButton* btnEraser = new QPushButton("◻ Ластик");
    btnPencil->setStyleSheet(
        "QPushButton { background-color:#6B8F5E; color:white;"
        " border-radius:8px; font-size:10pt; padding:4px; border:none; }"
        "QPushButton:hover { background-color:#7FA86E; }");
    btnEraser->setStyleSheet(
        "QPushButton { background-color:#A0785A; color:white;"
        " border-radius:8px; font-size:10pt; padding:4px; border:none; }"
        "QPushButton:hover { background-color:#C09070; }");
    connect(btnPencil, &QPushButton::clicked, this, [this]() {
        ui->canvas->setEraser(false);
    });
    connect(btnEraser, &QPushButton::clicked, this, [this]() {
        ui->canvas->setEraser(true);
    });
    grid->addWidget(btnPencil, row + 1, 0, 1, 2);
    grid->addWidget(btnEraser, row + 2, 0, 1, 2);
}


void MainWindow::showScoreScreen() {
    if (ui->scoreArea->layout()) {
        QLayoutItem* item;
        while ((item = ui->scoreArea->layout()->takeAt(0)) != nullptr) {
            if (item->widget()) item->widget()->deleteLater();
            delete item;
        }
        delete ui->scoreArea->layout();
    }

    for (QWidget* w : ui->scoreArea->findChildren<QWidget*>(
             QString(), Qt::FindDirectChildrenOnly)) {
        w->deleteLater();
    }

    QVBoxLayout* box = new QVBoxLayout(ui->scoreArea);
    box->setSpacing(4);
    box->setContentsMargins(8, 8, 8, 8);

    for (int i = 0; i < m_players.size(); i++) {
        QHBoxLayout* row = new QHBoxLayout();
        row->setSpacing(8);

        QLabel* lblName = new QLabel(m_players[i]);
        lblName->setFixedWidth(200);

        QPushButton* btnPlus  = new QPushButton("+10");
        QPushButton* btnMinus = new QPushButton("-10");
        btnPlus->setFixedSize(55, 32);
        btnMinus->setFixedSize(55, 32);

        btnPlus->setStyleSheet(
            "QPushButton { background-color:#4FAF5A; color:white;"
            " border-radius:8px; font-size:11pt; font-weight:bold; border:none; }"
            "QPushButton:hover { background-color:#6FC86A; }");
        btnMinus->setStyleSheet(
            "QPushButton { background-color:#D94F4F; color:white;"
            " border-radius:8px; font-size:11pt; font-weight:bold; border:none; }"
            "QPushButton:hover { background-color:#E87070; }");

        int score = (i < m_scores.size()) ? m_scores[i] : 0;
        QLabel* lblScore = new QLabel(QString("%1 / 100").arg(score));
        lblScore->setMinimumWidth(90);

        connect(btnPlus, &QPushButton::clicked, this, [this, i]() {
            if (m_net->isHost()) {
                m_game.addPoints(i, 10);
                hostBroadcastScores();
            } else {
                m_net->sendMessage(QString("ADDPOINT:%1;10").arg(i));
            }
        });
        connect(btnMinus, &QPushButton::clicked, this, [this, i]() {
            if (m_net->isHost()) {
                m_game.addPoints(i, -10);
                hostBroadcastScores();
            } else {
                m_net->sendMessage(QString("ADDPOINT:%1;-10").arg(i));
            }
        });

        row->addWidget(lblName);
        row->addWidget(btnPlus);
        row->addWidget(btnMinus);
        row->addWidget(lblScore);
        row->addStretch();
        box->addLayout(row);
    }

    box->addStretch();

    if (m_net->isHost() && m_players.size() < 2) {
        ui->btnContinue->setEnabled(false);
    } else {
        ui->btnContinue->setEnabled(true);
    }

    goToPage(ui->page_7);
}

void MainWindow::hostBroadcastScores() {
    QStringList parts;
    m_scores.clear();
    for (int i = 0; i < m_game.players().count(); i++) {
        int s = m_game.players().player(i).score();
        m_scores << s;
        parts << QString::number(s);
    }
    m_net->sendMessage("SCORES:" + parts.join(";"));

    showScoreScreen();
}

void MainWindow::applyScores(const QString& data) {
    QStringList parts = data.split(";");
    m_scores.clear();
    for (const QString& p : parts) {
        m_scores << p.toInt();
    }
    if (ui->stack->currentWidget() == ui->page_7) {
        showScoreScreen();
    }
}

void MainWindow::onContinue() {
    if (!m_net->isHost()) {
        return;
    }

    if (m_game.isGameOver()) {
        QString winner = QString::fromStdString(m_game.winnerName());
        m_net->sendMessage("OVER:" + winner);
        ui->lblWinner->setText(QString("%1 победил!").arg(winner));
        goToPage(ui->page_8);
    } else {
        hostStartTurn();
    }
}

void MainWindow::onNewGame() {
    m_net->disconnectAll();
    m_game.resetGame();
    m_players.clear();
    ui->listPlayers->clear();
    m_timer->stop();
    goToPage(ui->page_1);
}


void MainWindow::onShowRules() {
    QMessageBox::information(this, "Правила игры «Крокодил»",
                             "1. Создайте игру или присоединитесь к чужой.\n"
                             "2. Введите свой никнейм.\n"
                             "3. Художник выбирает одно из трёх слов.\n"
                             "4. Художник рисует слово за 60 секунд,\n"
                             "   остальные угадывают.\n"
                             "5. Угадавшему и художнику: +10 очка каждому.\n"
                             "6. Побеждает первый, кто наберёт 100 очков.");
}

void MainWindow::onOpenMenu() {
    if (ui->stack->currentWidget() == ui->page_9) return;
    m_previousPage = ui->stack->currentIndex();
    m_timer->stop();
    goToPage(ui->page_9);
}


void MainWindow::onMenuReset() {
    m_net->disconnectAll();
    m_timer->stop();
    m_game.resetGame();
    m_players.clear();
    ui->listPlayers->clear();
    goToPage(ui->page_1);
}

void MainWindow::onMenuExit() {
    close();
}

void MainWindow::onMenuOk() {
    ui->stack->setCurrentIndex(m_previousPage);
    if (ui->stack->currentWidget() == ui->page_6 && !m_game.isTimeUp()) {
        m_timer->start(1000);
    }
}

// Сеть
bool MainWindow::amIArtist() const {
    return m_players.indexOf(m_nickname) == m_artistIndex;
}

void MainWindow::hostStartTurn() {
    m_game.startTurn();
    m_artistIndex = m_game.currentArtistIndex();

    m_net->sendMessage(QString("TURN:%1;%2")
                           .arg(m_artistIndex).arg(m_game.currentRound()));

    auto words = m_game.wordChoices();
    if (words.size() >= 3) {
        m_lastWords.clear();
        m_lastWords << QString::fromStdString(words[0])
                    << QString::fromStdString(words[1])
                    << QString::fromStdString(words[2]);

        m_net->sendMessage(QString("WORDS:%1;%2;%3")
                               .arg(m_lastWords[0])
                               .arg(m_lastWords[1])
                               .arg(m_lastWords[2]));
    }

    applyTurn(m_artistIndex, m_game.currentRound());
}

void MainWindow::applyTurn(int artistIndex, int round) {
    m_artistIndex = artistIndex;
    m_round = round;

    QString artistName = (artistIndex >= 0 && artistIndex < m_players.size())
                             ? m_players[artistIndex] : QString();

    QString header = QString(
                         "<table width='100%'><tr>"
                         "<td align='left'>Ход: %1</td>"
                         "<td align='right'>Раунд: %2</td>"
                         "</tr></table>").arg(artistName).arg(round);
    ui->lblWordHeader->setText(header);

    if (amIArtist()) {
        if (m_lastWords.size() >= 3) {
            ui->radioWord0->setText(m_lastWords[0]);
            ui->radioWord1->setText(m_lastWords[1]);
            ui->radioWord2->setText(m_lastWords[2]);
        }
        for (QRadioButton* r : {ui->radioWord0, ui->radioWord1, ui->radioWord2}) {
            r->setAutoExclusive(false);
            r->setChecked(false);
            r->setAutoExclusive(true);
        }
        goToPage(ui->page_4);

    } else {
        showDrawScreen();
        ui->lblWord->setText("• • • • •");
        m_wordHidden = true;
        ui->btnHideWord->setText("Показать");
    }
}

void MainWindow::onPeerConnected() {
    if (m_net->isHost()) {

    } else {
        m_net->sendMessage("NICK:" + m_nickname);
        goToPage(ui->page_lobby);

        ui->radioEasy->setEnabled(false);
        ui->radioMedium->setEnabled(false);
        ui->radioHard->setEnabled(false);
        ui->btnStartGame->setEnabled(false);
        ui->btnRemovePlayer->setEnabled(false);
    }
}

void MainWindow::onPeerDisconnected(const QString& nick) {
    if (m_net->isHost()) {
        if (!nick.isEmpty()) {
            int idx = m_players.indexOf(nick);
            if (idx >= 0) {
                m_players.removeAt(idx);
            }
        }
        ui->listPlayers->clear();
        for (int i = 0; i < m_players.size(); i++) {
            QString label = m_players[i];
            if (i == 0) label += "  (хост)";
            else if (m_players[i] == m_nickname) label += "  (вы)";
            ui->listPlayers->addItem(label);
        }
        m_net->sendMessage("PLAYERS:" + m_players.join(","));
    } else {
        if (ui->stack->currentWidget() == ui->page_1) return;
        QMessageBox::information(this, "Сеть", "Соединение с хостом потеряно.");
        m_net->disconnectAll();
        m_game.resetGame();
        m_players.clear();
        ui->listPlayers->clear();
        m_timer->stop();
        goToPage(ui->page_1);
    }
}

void MainWindow::onNetMessage(const QString& text) {
    if (text.startsWith("NICK:")) {
        if (m_net->isHost()) {
            m_players << text.mid(5);
            ui->listPlayers->addItem(text.mid(5));
            m_net->sendMessage("PLAYERS:" + m_players.join(","));
            m_net->sendMessage("HOSTIP:" + myLocalIp());
        }
    }
    else if (text.startsWith("PLAYERS:")) {
        m_players = text.mid(8).split(",");
        ui->listPlayers->clear();
        for (int i = 0; i < m_players.size(); i++) {
            QString label = m_players[i];
            if (i == 0) {
                label += "  (хост)";
            } else if (m_players[i] == m_nickname) {
                label += "  (вы)";
            }
            ui->listPlayers->addItem(label);
        }
    }
    else if (text.startsWith("HOSTIP:")) {
        ui->lblHostIp->setText("IP: " + text.mid(7));
        ui->lblHostPort->setText("порт: 55555");
    }
    else if (text.startsWith("START:")) {
        int d = text.mid(6).toInt();
        m_difficulty = (d == 0) ? Difficulty::Easy
                       : (d == 1) ? Difficulty::Medium : Difficulty::Hard;
        if (!m_net->isHost()) {
            m_game.resetGame();
            m_game.startSession(m_difficulty, 100);
            for (const QString& name : m_players) {
                m_game.players().addPlayer(name.toStdString());
            }
        }
    }
    else if (text.startsWith("TURN:")) {
        QStringList p = text.mid(5).split(";");
        if (p.size() == 2) {
            applyTurn(p[0].toInt(), p[1].toInt());
        }
    }
    else if (text.startsWith("WORDS:")) {
        m_lastWords = text.mid(6).split(";");
        if (amIArtist() && m_lastWords.size() >= 3) {
            ui->radioWord0->setText(m_lastWords[0]);
            ui->radioWord1->setText(m_lastWords[1]);
            ui->radioWord2->setText(m_lastWords[2]);
            for (QRadioButton* r : {ui->radioWord0, ui->radioWord1, ui->radioWord2}) {
                r->setAutoExclusive(false);
                r->setChecked(false);
                r->setAutoExclusive(true);
            }
            goToPage(ui->page_4);
        }
    }
    else if (text.startsWith("WORD:")) {
        if (!amIArtist()) {
            showDrawScreen();
            ui->canvas->clear();
            ui->canvas->setDrawingEnabled(false);
            ui->lblWord->setText("• • • • •");
            m_wordHidden = true;
            ui->btnHideWord->setText("Показать");
        }
    }
    else if (text.startsWith("STARTTIMER:")) {
        if (m_net->isHost()) {
            m_game.startTimer(60);
            updateTimerLabel();
            m_timer->start(1000);
        }
    }
    else if (text.startsWith("DRAW:")) {
        QStringList d = text.mid(5).split(";");
        if (d.size() == 6) {
            ui->canvas->drawRemoteLine(d[0].toInt(), d[1].toInt(),
                                       d[2].toInt(), d[3].toInt(),
                                       QColor(d[4]), d[5].toInt());
        }
    }
    else if (text.startsWith("CLEAR:")) {
        ui->canvas->clear();
    }
    else if (text.startsWith("TIME:")) {
        int s = text.mid(5).toInt();
        int mm = s / 60, ss = s % 60;
        ui->lblTimer->setText(
            QString("%1:%2").arg(mm, 2, 10, QChar('0')).arg(ss, 2, 10, QChar('0')));
    }
    else if (text.startsWith("TIMEUP:")) {
        if (!m_net->isHost()) {
            showScoreScreen();
        }
    }
    else if (text.startsWith("FINISH:")) {
        if (m_net->isHost()) {
            m_game.endTurnEarly();
            m_timer->stop();
            m_net->sendMessage("TIMEUP:");
            hostBroadcastScores();
            showScoreScreen();
        }
    }
    else if (text.startsWith("ADDPOINT:")) {
        if (m_net->isHost()) {
            QStringList p = text.mid(9).split(";");
            if (p.size() == 2) {
                m_game.addPoints(p[0].toInt(), p[1].toInt());
                hostBroadcastScores();
            }
        }
    }
    else if (text.startsWith("SCORES:")) {
        applyScores(text.mid(7));
    }
    else if (text.startsWith("OVER:")) {
        ui->lblWinner->setText(QString("%1 победил!").arg(text.mid(5)));
        goToPage(ui->page_8);
    }
}

// Ошибка сети.
void MainWindow::onNetError(const QString& text) {
    QMessageBox::warning(this, "Сеть", text);
}
