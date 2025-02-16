#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#define API_ENDPOINT "latest"
#define API_KEY "49c6227841c7d0e9c7841faabe2c24ad"

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_convertButton_clicked();
    void handleApiResponse(QNetworkReply* reply);

private:
    Ui::MainWindow *ui;
    QNetworkAccessManager *networkManager;
};

#endif // MAINWINDOW_H
