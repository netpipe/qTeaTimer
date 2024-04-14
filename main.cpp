#include <QApplication>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QSound>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QTimer>
#include <QCloseEvent>

bool finished;
class TeaTimer : public QWidget {
    Q_OBJECT
public:
    TeaTimer(QWidget *parent = nullptr) : QWidget(parent), trayIcon(nullptr), timer(nullptr), timeSpinBox(nullptr), timeLeftLabel(nullptr) {
        createTrayIcon();
        setupUI();
    }

private slots:
    void startTimer(int minutes) {
        finished=0;
        if (timer && timer->isActive()) {
            timer->stop();
            delete timer;
        }
        timeLeft = minutes * 60; // Convert minutes to seconds
        updateTimeLeftLabel();
        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &TeaTimer::updateTimeLeft);
        connect(timer, &QTimer::timeout, this, &TeaTimer::timerExpired);
        timer->start(1000); // Update every second
        qApp->setQuitOnLastWindowClosed(false);
        QMessageBox::information(this, "Tea Timer", QString("Timer set for %1 minutes.").arg(minutes), QMessageBox::Ok);
    qApp->setQuitOnLastWindowClosed(true);
    }

    void updateTimeLeft() {
        if (timeLeft > 0) {
            --timeLeft;
            updateTimeLeftLabel();
        }
    }

    void timerExpired() {
        if (timeLeft == 0 && finished == 0) {
            showMessageBox();
            QSound::play(":/sounds/alert.wav");
        }
    }

    void showMessageBox() {
         qApp->setQuitOnLastWindowClosed(false);
        QMessageBox::information(this, "Tea Timer", "Your tea is ready!", QMessageBox::Ok);
        finished=1;
        qApp->setQuitOnLastWindowClosed(true);
    }

    void updateTimeLeftLabel() {
        int minutes = timeLeft / 60;
        int seconds = timeLeft % 60;
        timeLeftLabel->setText(QString("Time Left: %1:%2").arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0')));
    }

    void createTrayIcon() {
        trayIconMenu = new QMenu();

        QAction *action5Min = new QAction("5 minutes", this);
        connect(action5Min, &QAction::triggered, this, [this]() { startTimer(5); });
        trayIconMenu->addAction(action5Min);

        QAction *action10Min = new QAction("10 minutes", this);
        connect(action10Min, &QAction::triggered, this, [this]() { startTimer(10); });
        trayIconMenu->addAction(action10Min);

        QAction *action15Min = new QAction("15 minutes", this);
        connect(action15Min, &QAction::triggered, this, [this]() { startTimer(15); });
        trayIconMenu->addAction(action15Min);

        QAction *showAction = new QAction("Show", this);
        connect(showAction, &QAction::triggered, this, &TeaTimer::showNormal);
        trayIconMenu->addAction(showAction);

        QAction *quitAction = new QAction("Quit", this);
        connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
        trayIconMenu->addAction(quitAction);

        trayIcon = new QSystemTrayIcon();
        trayIcon->setIcon(QIcon(":/images/icon.png"));
        trayIcon->setToolTip("Tea Timer");
        trayIcon->setContextMenu(trayIconMenu);
        trayIcon->show();
    }

    void setupUI() {
        QHBoxLayout *timeLayout = new QHBoxLayout;
        QLabel *timeLabel = new QLabel("Time (minutes):", this);
        timeSpinBox = new QSpinBox(this);
        timeSpinBox->setMinimum(1);
        timeSpinBox->setMaximum(60); // Limit to 60 minutes
        timeSpinBox->setValue(5); // Default value
        timeLayout->addWidget(timeLabel);
        timeLayout->addWidget(timeSpinBox);

        QPushButton *startButton = new QPushButton("Start", this);
        connect(startButton, &QPushButton::clicked, this, [this]() { startTimer(timeSpinBox->value()); });

        QVBoxLayout *mainLayout = new QVBoxLayout(this);
        mainLayout->addLayout(timeLayout);
        mainLayout->addWidget(startButton);

        timeLeftLabel = new QLabel("Time Left: 00:00", this);
        mainLayout->addWidget(timeLeftLabel);
    }

protected:
    void closeEvent(QCloseEvent *event) override {
        hide();
        event->ignore();
    }

private:
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
    QTimer *timer;
    QSpinBox *timeSpinBox;
    QLabel *timeLeftLabel;
    int timeLeft;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    TeaTimer teaTimer;
    teaTimer.setWindowTitle("Tea Timer");
    teaTimer.show();
    return app.exec();
}

#include "main.moc"
