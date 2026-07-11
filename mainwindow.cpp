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



    connect(ui->btnCreate, &QPushButton::clicked, this, &MainWindow::onCreate);
    connect(ui->btnJoin,   &QPushButton::clicked, this, &MainWindow::onJoin);
    connect(ui->btnExit,   &QPushButton::clicked, this, &MainWindow::onExit);

    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::onNickContinue);
    connect(ui->lineEdit, &QLineEdit::textChanged, this, [this](const QString& text) {
        ui->lblCharCount->setText(QString("%1/16").arg(text.length()));
    });

    connect(ui->pushButton_3, &QPushButton::clicked, this, &MainWindow::onRefreshLobbies);
    connect(ui->pushButton_2, &QPushButton::clicked, this, &MainWindow::onJoinConnect);

    connect(ui->btnRemovePlayer, &QPushButton::clicked, this, &MainWindow::onRemovePlayer);
    connect(ui->btnStartGame,    &QPushButton::clicked, this, &MainWindow::onStartGame);

    connect(ui->btnConfirmWord,  &QPushButton::clicked, this, &MainWindow::onConfirmWord);

    connect(ui->btnFinish,   &QPushButton::clicked, this, &MainWindow::onFinish);
    connect(ui->btnClear,    &QPushButton::clicked, this, &MainWindow::onClearCanvas);
    connect(ui->btnHideWord, &QPushButton::clicked, this, &MainWindow::onHideWord);
    connect(ui->spinWidth, QOverload<int>::of(&QSpinBox::valueChanged),
            this, [this](int v){ ui->canvas->setPenWidth(v); });

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
        m_game.resetGame();
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
    ui->listWidget->clear();
    ui->listWidget->addItem("Комната 2");
    ui->listWidget->addItem("Комната 1");
}

void MainWindow::onJoinConnect() {
    if (ui->listWidget->currentRow() < 0) {
        QMessageBox::warning(this, "Ошибка", "Выберите комнату из списка!");
        return;
    }

    ui->listPlayers->clear();
    ui->listPlayers->addItem("Хост");
    ui->listPlayers->addItem(m_nickname + "  (вы)");

    for (QRadioButton* r : {ui->radioEasy, ui->radioMedium, ui->radioHard}) {
        r->setAutoExclusive(false);
        r->setChecked(false);
        r->setAutoExclusive(true);
    }
    goToPage(ui->page_lobby);
}


void MainWindow::onRemovePlayer() {
    int row = ui->listPlayers->currentRow();
    if (row >= 0) {
        delete ui->listPlayers->takeItem(row);
    }
}

void MainWindow::onStartGame() {
    // if (ui->listPlayers->count() < 3) {
    //     QMessageBox::warning(this, "Ошибка", "Нужно хотя бы 3 игрока!");
    //     return;
    // }


    if (!ui->radioEasy->isChecked() &&
        !ui->radioMedium->isChecked() &&
        !ui->radioHard->isChecked()) {
        QMessageBox::warning(this, "Ошибка", "Выберите режим сложности!");
        return;
    }

    Difficulty diff = Difficulty::Easy;
    if (ui->radioMedium->isChecked()) diff = Difficulty::Medium;
    if (ui->radioHard->isChecked())   diff = Difficulty::Hard;

    m_game.resetGame();
    m_game.startSession(diff, 20);

    for (int i = 0; i < ui->listPlayers->count(); i++) {
        QString name = ui->listPlayers->item(i)->text();
        int cut = name.indexOf("  ");
        if (cut >= 0) name = name.left(cut);
        m_game.players().addPlayer(name.trimmed().toStdString());
    }

    m_game.startTurn();
    showWordScreen();
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
    m_game.chooseWord(idx);
    showDrawScreen();
}


void MainWindow::showDrawScreen() {
    QString header = QString(
                         "<table width='100%'>"
                         "<tr>"
                         "<td align='left'>Ход: %1</td>"
                         "<td align='right'>Раунд: %2</td>"
                         "</tr>"
                         "</table>")
                         .arg(QString::fromStdString(m_game.currentArtistName()))
                         .arg(m_game.currentRound());
    ui->lblWordHeader_2->setText(header);

    m_wordHidden = true;
    ui->btnHideWord->setText("Показать");
    ui->lblWord->setText("• • • • •");

    ui->canvas->clear();
    ui->canvas->setPenColor(Qt::black);
    ui->canvas->setPenWidth(ui->spinWidth->value());

    m_game.startTimer(60);
    updateTimerLabel();
    m_timer->start(1000);

    goToPage(ui->page_6);
}

void MainWindow::onTimerTick() {
    m_game.tick();
    updateTimerLabel();
    if (m_game.isTimeUp()) {
        m_timer->stop();
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
    m_game.endTurnEarly();
    m_timer->stop();
    showScoreScreen();
}

void MainWindow::onClearCanvas() {
    ui->canvas->clear();
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

    for (int i = 0; i < m_game.players().count(); i++) {
        QHBoxLayout* row = new QHBoxLayout();
        row->setSpacing(8);

        QLabel* lblName = new QLabel(
            QString::fromStdString(m_game.players().player(i).name()));
        lblName->setFixedWidth(200);

        QPushButton* btnPlus  = new QPushButton("+3");
        QPushButton* btnMinus = new QPushButton("-3");
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

        QLabel* lblScore = new QLabel(
            QString("%1 / 20").arg(m_game.players().player(i).score()));
        lblScore->setMinimumWidth(90);

        connect(btnPlus, &QPushButton::clicked, this, [this, i, lblScore]() {
            if (m_game.players().player(i).score() >= 21) return;
            m_game.addPoints(i, 3);
            lblScore->setText(
                QString("%1 / 20").arg(m_game.players().player(i).score()));
        });

        connect(btnMinus, &QPushButton::clicked, this, [this, i, lblScore]() {
            m_game.addPoints(i, -3);
            lblScore->setText(
                QString("%1 / 20").arg(m_game.players().player(i).score()));
        });

        row->addWidget(lblName);
        row->addWidget(btnPlus);
        row->addWidget(btnMinus);
        row->addWidget(lblScore);
        row->addStretch();
        box->addLayout(row);
    }

    box->addStretch();
    goToPage(ui->page_7);
}

void MainWindow::onContinue() {
    if (m_game.isGameOver()) {
        ui->lblWinner->setText(
            QString("%1 победил!")
                .arg(QString::fromStdString(m_game.winnerName())));
        goToPage(ui->page_8);
    } else {
        m_game.startTurn();
        showWordScreen();
    }
}


void MainWindow::onNewGame() {
    m_game.resetGame();
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
                             "5. Угадавшему и художнику: +3 очка каждому.\n"
                             "6. Побеждает первый, кто наберёт 20 очков.");
}

void MainWindow::onOpenMenu() {
    if (ui->stack->currentWidget() == ui->page_9) return;
    m_previousPage = ui->stack->currentIndex();
    m_timer->stop();
    goToPage(ui->page_9);
}


void MainWindow::onMenuReset() {
    m_timer->stop();
    m_game.resetGame();
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