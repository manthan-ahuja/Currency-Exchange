#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QMessageBox>
#include <QDebug>

#define API_KEY "49c6227841c7d0e9c7841faabe2c24ad"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    networkManager = new QNetworkAccessManager(this);

    connect(ui->convertButton, &QPushButton::clicked, this, &MainWindow::on_convertButton_clicked);
    connect(networkManager, &QNetworkAccessManager::finished, this, &MainWindow::handleApiResponse);

    // Populate currency dropdowns
    QStringList currencies = {"EUR", "USD", "GBP", "INR", "JPY", "AUD", "CAD", "CHF"};
    ui->fromCurrency->addItems(currencies);
    ui->toCurrency->addItems(currencies);
}

MainWindow::~MainWindow() {
    delete ui;
}

// Fetch exchange rate when button is clicked
void MainWindow::on_convertButton_clicked() {
    QString fromCurrency = ui->fromCurrency->currentText();
    QString toCurrency = ui->toCurrency->currentText();
    double amount = ui->amountInput->text().toDouble();

    if (amount <= 0) {
        QMessageBox::warning(this, "Input Error", "Please enter a valid amount.");
        return;
    }

    // API requires "EUR" as the base currency in the free plan
    if (fromCurrency != "EUR") {
        QMessageBox::warning(this, "API Limitation", "Only EUR is supported as the base currency.");
        return;
    }

    // Construct the correct API request URL
    QString apiUrl = QString("http://api.exchangeratesapi.io/v1/latest"
                             "?access_key=%1&symbols=%2")
                         .arg(API_KEY)
                         .arg(toCurrency);

    qDebug() << "API URL: " << apiUrl;  // Debugging output

    QNetworkRequest request;
    request.setUrl(QUrl(apiUrl));
    networkManager->get(request);
}

// Handle API response
void MainWindow::handleApiResponse(QNetworkReply* reply) {
    if (reply->error() != QNetworkReply::NoError) {
        QMessageBox::critical(this, "Error", "Failed to fetch exchange rate: " + reply->errorString());
        reply->deleteLater();
        return;
    }

    // Parse JSON response
    QByteArray responseData = reply->readAll();
    qDebug() << "Raw API Response:" << responseData;

    QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
    QJsonObject jsonObj = jsonDoc.object();

    if (jsonObj.contains("rates") && jsonObj["rates"].isObject()) {
        QString toCurrency = ui->toCurrency->currentText();
        double rate = jsonObj["rates"].toObject().value(toCurrency).toDouble();

        if (rate > 0) {
            double amount = ui->amountInput->text().toDouble();
            double convertedAmount = amount * rate;

            // Display result
            ui->resultLabel->setText(QString::number(convertedAmount, 'f', 2));
        } else {
            QMessageBox::warning(this, "API Error", "Invalid currency conversion rate.");
        }
    } else {
        QMessageBox::warning(this, "API Error", "Invalid response from API.");
    }

    reply->deleteLater();
}
