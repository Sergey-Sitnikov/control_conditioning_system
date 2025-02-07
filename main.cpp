#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QLineEdit>
#include <QSettings>
#include <QDialog>
//#include <QRegularExpression>
#include <QDebug>

class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    SettingsDialog(QWidget *parent = nullptr) : QDialog(parent) {
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

        QPushButton *okButton = new QPushButton("OK", this);
        connect(okButton, &QPushButton::clicked, this, &SettingsDialog::accept);
        layout->addWidget(okButton);

        setLayout(layout);
    }

    float getTemperature() const {
        return temperatureInput->text().toFloat();
    }

    float getHumidity() const {
        return humidityInput->text().toFloat();
    }

    float getPressure() const {
        return pressureInput->text().toFloat();
    }

private:
    QLineEdit *temperatureInput;
    QLineEdit *humidityInput;
    QLineEdit *pressureInput;
};

class HVACControl : public QMainWindow {
    Q_OBJECT

public:
    HVACControl(QWidget *parent = nullptr);

private slots:
    void toggleAC();
    void openSettings();
    void changeScale(const QString &scale);
    void saveSettings();
    void loadSettings();

private:
    QString unittemperature = "°C";
    double temp = 0;
    QLabel *temperatureLabel;
    QLabel *humidityLabel;
    QLabel *pressureLabel;
    QComboBox *tempScaleCombo;
    QComboBox *pressureUnitCombo;
    QPushButton *toggleButton;
    bool acStatus;
    QGraphicsView *graphicsView;
    QGraphicsScene *scene;

    void updateLabels(double temp, float humidity, float pressure, const QString &scale);
    void convertTemperature(const QString &toScale);
};

HVACControl::HVACControl(QWidget *parent) : QMainWindow(parent), acStatus(false) {
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

  //  unittemperature = "°C";
    temperatureLabel = new QLabel("Температура: " + QString::number(temp) + " " + unittemperature, this);
    humidityLabel = new QLabel("Влажность: 0 %", this);
    pressureLabel = new QLabel("Давление: 0 Pa", this);

    layout->addWidget(temperatureLabel);
    layout->addWidget(humidityLabel);
    layout->addWidget(pressureLabel);

    tempScaleCombo = new QComboBox(this);
    tempScaleCombo->addItems({"Celsius", "Fahrenheit", "Kelvin"});
    connect(tempScaleCombo, &QComboBox::currentTextChanged, this, &HVACControl::changeScale);
    layout->addWidget(tempScaleCombo);

    pressureUnitCombo = new QComboBox(this);
    pressureUnitCombo->addItems({"Pascals", "mmHg"});
    layout->addWidget(pressureUnitCombo);

    QPushButton *settingsButton = new QPushButton("Настройки", this);
    connect(settingsButton, &QPushButton::clicked, this, &HVACControl::openSettings);
    layout->addWidget(settingsButton);

    toggleButton = new QPushButton("Turn On AC", this);
    layout->addWidget(toggleButton);
    connect(toggleButton, &QPushButton::clicked, this, &HVACControl::toggleAC);

    graphicsView = new QGraphicsView(this);
    scene = new QGraphicsScene(this);
    graphicsView->setScene(scene);
    layout->addWidget(graphicsView);

    setCentralWidget(centralWidget);
    loadSettings();

    resize(1024, 768);
}

void HVACControl::toggleAC() {
    acStatus = !acStatus;
    toggleButton->setText(acStatus ? "Turn Off AC" : "Turn On AC");
}

void HVACControl::changeScale(const QString &scale) {
    // Сначала конвертируем текущую температуру, затем обновляем метки
    //std::printf(QString::number(temp));
     qDebug() << "Температура1:" << QString::number(temp);

    if (scale == "Celsius") {
        convertTemperature("°C");
    } else if (scale == "Fahrenheit") {
        convertTemperature("°F");
    } else if (scale == "Kelvin") {
        convertTemperature("K");
    }

    updateLabels(temp, humidityLabel->text().split(" ")[1].toFloat(),
                 pressureLabel->text().split(" ")[1].toFloat(), unittemperature);
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
    // Обновляем метки после конверсии
    updateLabels(temp, humidityLabel->text().split(" ")[1].toFloat(),
                 pressureLabel->text().split(" ")[1].toFloat(), unittemperature);
}

void HVACControl::updateLabels(double temp, float humidity, float pressure, const QString &scale) {
    qDebug() << "Updating labels with Temperature:" << temp << "Humidity:" << humidity << "Pressure:" << pressure << "Scale:" << scale;

    temperatureLabel->setText(QString("Температура: %1 %2").arg(temp, 0, 'f', 2).arg(scale));
    humidityLabel->setText(QString("Влажность: %1 %").arg(humidity, 0, 'f', 2));
    pressureLabel->setText(QString("Давление: %1 Pa").arg(pressure, 0, 'f', 2));
}

void HVACControl::openSettings() {
    SettingsDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        temp = dialog.getTemperature();
        float humidity = dialog.getHumidity();
        float pressure = dialog.getPressure();
        updateLabels(temp, humidity, pressure, "°C");
    }
}

void HVACControl::saveSettings() {
    QSettings settings("YourCompany", "HVACControl");
    settings.setValue("TemperatureScale", tempScaleCombo->currentText());
    settings.setValue("PressureUnit", pressureUnitCombo->currentText());
}

void HVACControl::loadSettings() {
    QSettings settings("YourCompany", "HVACControl");
    tempScaleCombo->setCurrentText(settings.value("TemperatureScale", "Celsius").toString());
    pressureUnitCombo->setCurrentText(settings.value("PressureUnit", "Pascals").toString());
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    HVACControl window;
    window.show();
    return app.exec();
}

#include "main.moc"
