#pragma once

#include <QMainWindow>

#include "structs.hpp"

class QDoubleSpinBox;
class QSpinBox;
class QLabel;
class QPushButton;
class QListWidget;
class QGroupBox;

class MainWindow : public QMainWindow {
   Q_OBJECT

private slots:  
   void OnFindMeClicked();
   void OnCalcWeatherPlacesClicked();

private:
   // виджеты
   QDoubleSpinBox* latitude_input_;
   QDoubleSpinBox* longitude_input_;

   QPushButton* find_me_button_;
   QPushButton* calc_weather_places_button_;

   QLabel* weather_type_label_;
   QLabel* temperature_label_;
   QLabel* wind_speed_label_;
   QLabel* precipitation_label_;
   QLabel* precipitation_probability_label_;

   QListWidget* places_list_;
   QLabel* weather_image_;

private:
   // сборка окна
   void BuildUi();

   QGroupBox* CreateLocationBox();
   QGroupBox* CreateWeatherBox();
   QGroupBox* CreatePlacesBox();
   QGroupBox* CreateImageBox();

private:
   // хелперы
   void UpdateLocation(const Coordinates& coords);
   void UpdateWeather(const WeatherInfo& weather);
   void UpdatePlaces(const std::vector<Place>& places);
   void UpdateWeatherImage(WeatherType type);

   Coordinates ReadCoordinates();
   QString WeatherTypeToQString(WeatherType type) const;

public:
   explicit MainWindow(QWidget* parent = nullptr);
};