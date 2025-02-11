#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QLineEdit>
#include <QDialog>
#include <QMessageBox>
#include <QDebug>

class ResolutionDialog : public QDialog {
    Q_OBJECT

public:
    ResolutionDialog(QWidget *parent = nullptr) : QDialog(parent) {
        setWindowTitle("Выбор разрешения");
        setModal(true);

        // Установка флагов окна для управления видом
        setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);

        QVBoxLayout *layout = new QVBoxLayout(this);

        // Кнопка для 1024x768
        QPushButton *res1Button = new QPushButton("1024x768", this);
        connect(res1Button, &QPushButton::clicked, this, [this]() {
            emit resolutionChosen(1024, 768);
            accept(); // Закрывает диалог после выбора
        });

        // Кнопка для 800x600
        QPushButton *res2Button = new QPushButton("800x600", this);
        connect(res2Button, &QPushButton::clicked, this, [this]() {
            emit resolutionChosen(800, 600);
            accept(); // Закрывает диалог после выбора
        });

        layout->addWidget(res1Button);
        layout->addWidget(res2Button);

        setLayout(layout);
    }

signals:
    void resolutionChosen(int width, int height);
};


class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    SettingsDialog(QWidget *parent = nullptr) : QDialog(parent) {
        setWindowTitle("Настройки");
        setModal(false);
        QVBoxLayout *layout = new QVBoxLayout(this);

        temperatureInput = new QLineEdit(this);
        humidityInput = new QLineEdit(this);
        pressureInput = new QLineEdit(this);

        layout->addWidget(new QLabel("Установить Температуру:"));
        layout->addWidget(temperatureInput);
        layout->addWidget(new QLabel("Установить Влажность:"));
        layout->addWidget(humidityInput);
        layout->addWidget(new QLabel("Установить Давление:"));
        layout->addWidget(pressureInput);

        // Кнопка для обновления значений
        QPushButton *updateButton = new QPushButton("Обновить значения", this);
        connect(updateButton, &QPushButton::clicked, this, &SettingsDialog::updateValues);
        layout->addWidget(updateButton);

        setLayout(layout);
    }

    void updateValues() {
        bool tempOk, humidityOk, pressureOk;

        float tempValue = temperatureInput->text().toFloat(&tempOk);
        int humidityValue = humidityInput->text().toInt(&humidityOk);
        float pressureValue = pressureInput->text().toFloat(&pressureOk);

        // Проверка валидности введенных данных
        if (!tempOk || tempValue < -50 || tempValue > 70) {
            QMessageBox::warning(this, "Неверный ввод", "Пожалуйста, введите допустимую температуру.");
            return;
        }
        if (!humidityOk || humidityValue < 0 || humidityValue > 100) {
            QMessageBox::warning(this, "Неверный ввод", "Пожалуйста, введите допустимый процент влажности.");
            return;
        }
        if (!pressureOk || pressureValue < 0) {
            QMessageBox::warning(this, "Неверный ввод", "Пожалуйста, введите допустимое давление.");
            return;
        }

        // Уведомляем об обновлении значений в настройках
        emit valuesUpdated(tempValue, humidityValue, pressureValue);
    }

    float getTemperature() const {
        return temperatureInput->text().toFloat();
    }

    int getHumidity() const {
        return humidityInput->text().toInt();
    }

    float getPressure() const {
        return pressureInput->text().toFloat();
    }

signals:
    void valuesUpdated(float temp, int humidity, float pressure);

private:
    QLineEdit *temperatureInput;
    QLineEdit *humidityInput;
    QLineEdit *pressureInput;
};

class HVACControl : public QMainWindow {
    Q_OBJECT

public:
    HVACControl(int width, int height, QWidget *parent = nullptr);

private slots:
    void toggleAC();
    void changeScaleTemperature(const QString &scale);
    void changeScalePressure(const QString &scale);
    void updateFromSettings(float temp, int humidity, float pressure);

private:
    QString unittemperature = "°C";
    double temp = 0;
    QLabel *temperatureLabel;
    int humidity = 0;
    QLabel *humidityLabel;
    QString unitpressure = "Pa";
    double pressure = 0;
    QLabel *pressureLabel;
    QComboBox *tempScaleCombo;
    QComboBox *pressureScaleCombo;
    QPushButton *toggleButton;
    bool acStatus;
    QGraphicsView *graphicsView;
    QGraphicsScene *scene;
    SettingsDialog *settingsDialog;

    void updateLabels(double temp, int humidity, float pressure, const QString &scaleT, const QString &scaleP);
    void convertTemperature(const QString &toScale);
    void convertPressure(const QString &toScale);
};

HVACControl::HVACControl(int width, int height, QWidget *parent)
    : QMainWindow(parent), acStatus(false) {
    setFixedSize(width, height); // Установка фиксированного размера окна

    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    temperatureLabel = new QLabel("Температура: " + QString::number(temp) + " " + unittemperature, this);
    humidityLabel = new QLabel("Влажность: " + QString::number(humidity) + " %", this);
    pressureLabel = new QLabel("Давление: " + QString::number(pressure) + " " + unitpressure, this);

    layout->addWidget(temperatureLabel);
    layout->addWidget(humidityLabel);
    layout->addWidget(pressureLabel);

    tempScaleCombo = new QComboBox(this);
    tempScaleCombo->addItems({"Celsius", "Fahrenheit", "Kelvin"});
    connect(tempScaleCombo, &QComboBox::currentTextChanged, this, &HVACControl::changeScaleTemperature);
    layout->addWidget(tempScaleCombo);

    pressureScaleCombo = new QComboBox(this);
    pressureScaleCombo->addItems({"Pascals", "mmHg"});
    connect(pressureScaleCombo, &QComboBox::currentTextChanged, this, &HVACControl::changeScalePressure);
    layout->addWidget(pressureScaleCombo);

    toggleButton = new QPushButton("Turn On AC", this);
    layout->addWidget(toggleButton);
    connect(toggleButton, &QPushButton::clicked, this, &HVACControl::toggleAC);

    graphicsView = new QGraphicsView(this);
    scene = new QGraphicsScene(this);
    graphicsView->setScene(scene);
    layout->addWidget(graphicsView);

    setCentralWidget(centralWidget);

    // Создаем и показываем диалоговое окно настроек
    settingsDialog = new SettingsDialog(this);
    settingsDialog->show();

    // Подключаем сигнал для обновления значений из настроек
    connect(settingsDialog, &SettingsDialog::valuesUpdated, this, &HVACControl::updateFromSettings);
}

void HVACControl::toggleAC() {
    acStatus = !acStatus;
    toggleButton->setText(acStatus ? "Turn Off AC" : "Turn On AC");
}

void HVACControl::changeScaleTemperature(const QString &scale) {
    qDebug() << "Температура1:" << QString::number(temp);

    if (scale == "Celsius") {
        convertTemperature("°C");
    } else if (scale == "Fahrenheit") {
        convertTemperature("°F");
    } else if (scale == "Kelvin") {
        convertTemperature("K");
    }

    qDebug() << "Температура2:" << QString::number(temp);
}

void HVACControl::convertTemperature(const QString &toScale) {
    double originalTemp = temp;
    if (unittemperature == "°C") {
        if (toScale == "°F") {
            temp = (originalTemp * 9.0 / 5.0) + 32;
            unittemperature = "°F";
        } else if (toScale == "K") {
            temp = originalTemp + 273.15;
            unittemperature = "K";
        }
    } else if (unittemperature == "°F") {
        if (toScale == "°C") {
            temp = (originalTemp - 32) * 5.0 / 9.0;
            unittemperature = "°C";
        } else if (toScale == "K") {
            temp = (originalTemp - 32) * 5.0 / 9.0 + 273.15;
            unittemperature = "K";
        }
    } else if (unittemperature == "K") {
        if (toScale == "°C") {
            temp = originalTemp - 273.15;
            unittemperature = "°C";
        } else if (toScale == "°F") {
            temp = (originalTemp - 273.15) * 9.0 / 5.0 + 32;
            unittemperature = "°F";
        }
    }
    updateLabels(temp, humidity, pressure, unittemperature, unitpressure);
}

void HVACControl::changeScalePressure(const QString &scale) {
    if (scale == "Pascals") {
        convertPressure("Pa");
    } else if (scale == "mmHg") {
        convertPressure("mmHg");
    }
}

void HVACControl::convertPressure(const QString &toScale) {
    double originalPressure = pressure;
    if (unitpressure == "Pa") {
        if (toScale == "mmHg") {
            pressure = originalPressure / 133.322;
            unitpressure = "mmHg";
        }
    } else if (unitpressure == "mmHg") {
        if (toScale == "Pa") {
            pressure = originalPressure * 133.322;
            unitpressure = "Pa";
        }
    }
    updateLabels(temp, humidity, pressure, unittemperature, unitpressure);
}

void HVACControl::updateLabels(double temp, int humidity, float pressure, const QString &scaleT, const QString &scaleP) {
    qDebug() << "Updating labels with Temperature:" << temp << "Humidity:" << humidity << "Pressure:" << pressure << "Scale:" << scaleT << "Scale:" << scaleP;

    temperatureLabel->setText(QString("Температура: %1 %2").arg(temp, 0, 'f', 2).arg(scaleT));
    humidityLabel->setText(QString("Влажность: %1 %").arg(humidity));
    pressureLabel->setText(QString("Давление: %1 %2").arg(pressure, 0, 'f', 2).arg(scaleP));
}

void HVACControl::updateFromSettings(float newTemp, int newHumidity, float newPressure) {
    temp = newTemp; // Записываем значение из диалога настроек
    humidity = newHumidity;
    pressure = newPressure;
    updateLabels(temp, humidity, pressure, unittemperature, unitpressure);
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Создаем и показываем диалоговое окно разрешения
    ResolutionDialog resDialog;
    QObject::connect(&resDialog, &ResolutionDialog::resolutionChosen, [&](int width, int height) {
        // Создаем окно HVACControl с выбранным разрешением
        HVACControl *window = new HVACControl(width, height);
        window->show();
    });

    resDialog.exec(); // Ожидаем, пока пользователь выберет разрешение

    return app.exec();
}

#include "main.moc"
