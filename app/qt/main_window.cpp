#include <filesystem>

#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QPixmap>
#include <QPushButton>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QWidget>
#include <QString>
#include <QSizePolicy>
#include <QMessageBox>

#include "main_window.hpp"
#include "main_logic.hpp"

MainWindow::MainWindow(QWidget* parent)   
   : QMainWindow(parent),
   latitude_input_(nullptr),
   longitude_input_(nullptr),
   find_me_button_(nullptr),
   calc_weather_places_button_(nullptr),
   weather_type_label_(nullptr),
   temperature_label_(nullptr),
   wind_speed_label_(nullptr),
   precipitation_label_(nullptr),
   precipitation_probability_label_(nullptr),
   places_list_(nullptr),
   weather_image_(nullptr)
{
   BuildUi();
}

void MainWindow::BuildUi() {
   auto* central = new QWidget(this);

   auto* main_layout = new QVBoxLayout(central);
   auto* top_layout = new QHBoxLayout();
   main_layout->addLayout(top_layout, 0);

   top_layout->addWidget(CreateLocationBox());
   top_layout->addWidget(CreateWeatherBox());

   main_layout->addWidget(CreatePlacesBox(), 1);
   main_layout->addWidget(CreateImageBox(), 1);

   setWindowTitle("Places recommender");
   resize(600, 800);

   connect(find_me_button_, &QPushButton::clicked, 
                  this, &MainWindow::OnFindMeClicked);

   connect(calc_weather_places_button_, &QPushButton::clicked,
                  this, &MainWindow::OnCalcWeatherPlacesClicked);

   setCentralWidget(central);
}

QGroupBox* MainWindow::CreateLocationBox() {
   auto* box = new QGroupBox("локация", this);
   auto* layout = new QGridLayout(box);

   auto* latitude_label_ = new QLabel("latitude", box);
   auto* longitude_label_ = new QLabel("longitude", box);

   latitude_input_ = new QDoubleSpinBox(box);
   longitude_input_ = new QDoubleSpinBox(box);

   latitude_input_->setRange(-90.0, 90.0);
   longitude_input_->setRange(-180.0, 180.0);

   latitude_input_->setDecimals(8);
   longitude_input_->setDecimals(8);

   latitude_input_->setValue(0.0);
   longitude_input_->setValue(0.0);

   latitude_input_->setSingleStep(0.00000001);
   longitude_input_->setSingleStep(0.00000001);

   find_me_button_ = new QPushButton("найти меня", box);
   
   layout->addWidget(longitude_label_, 0, 0);
   layout->addWidget(latitude_label_, 0, 1);
   layout->addWidget(longitude_input_, 1, 0);
   layout->addWidget(latitude_input_, 1, 1);
   layout->addWidget(find_me_button_, 2, 0, 1, 2);

   return box;
}

QGroupBox* MainWindow::CreateWeatherBox() {
   auto* box = new QGroupBox("погода", this);
   auto layout = new QGridLayout(box);

   calc_weather_places_button_ = new QPushButton("вычислить погоду и места", box);
   
   weather_type_label_ = new QLabel("type: ...", box);
   temperature_label_ = new QLabel("temperature: ...", box);
   wind_speed_label_ = new QLabel("wind speed: ...", box);
   precipitation_label_ = new QLabel("precipitation: ...", box);
   precipitation_probability_label_ = new QLabel("precipitation probability: ...", box);

   layout->addWidget(weather_type_label_, 0, 0, 1, 2);
   layout->addWidget(temperature_label_, 1, 0, 1, 2);
   layout->addWidget(wind_speed_label_, 2, 0, 1, 2);
   layout->addWidget(precipitation_label_, 3, 0, 1, 2);
   layout->addWidget(precipitation_probability_label_, 4, 0, 1, 2);
   layout->addWidget(calc_weather_places_button_, 5, 0, 1, 2);
   

   return box;
}

QGroupBox* MainWindow::CreatePlacesBox() {
   auto* box = new QGroupBox("места", this);
   auto* layout = new QVBoxLayout(box);

   places_list_ = new QListWidget(box);

   QFont places_font = places_list_->font();
   places_font.setPointSize(14);
   places_list_->setFont(places_font);

   layout->addWidget(places_list_);

   return box;
}

QGroupBox* MainWindow::CreateImageBox() {
   auto* box = new QGroupBox(this);
   auto* layout = new QVBoxLayout(box);

   weather_image_ = new QLabel(box);
   weather_image_->setAlignment(Qt::AlignCenter);

   weather_image_->setText("тут будет картинка погоды");

   layout->addWidget(weather_image_);

   return box;
}

Coordinates MainWindow::ReadCoordinates() {
   latitude_input_->interpretText();
   longitude_input_->interpretText();

   Coordinates coords;

   coords.latitude = latitude_input_->value();
   coords.longitude = longitude_input_->value();

   return coords;
}
QString MainWindow::WeatherTypeToQString(WeatherType type) const {
   switch (type) {
      case (WeatherType::kGood):
         return "good";

      case (WeatherType::kNeutral):
         return "neutral";

      case (WeatherType::kRainy):
         return "rainy";

      case (WeatherType::kSnowy):
         return "snowy";

      case (WeatherType::kDangerous):
         return "dangerous";

      default:
         return "unknown";
   }
}

void MainWindow::UpdateLocation(const Coordinates& coords) {
   latitude_input_->setValue(coords.latitude);
   longitude_input_->setValue(coords.longitude);
}

void MainWindow::UpdateWeather(const WeatherInfo& weather) {
   weather_type_label_->setText(
      "type: " + WeatherTypeToQString(weather.type)
   );

   temperature_label_->setText(
      QString("temperature: %1 C").arg(weather.temperature)
   ); 

   wind_speed_label_->setText(
      QString("wind speed: %1 km/h").arg(weather.wind_speed)
   );

   precipitation_label_->setText(
      QString("precipitation: %1 mm").arg(weather.precipitation)
   );

   precipitation_probability_label_->setText(
      QString("precipitation probability: %1%").arg(weather.precipitation_probability)
   );
}


void MainWindow::UpdatePlaces(const std::vector<Place>& places) {
   places_list_->clear();

   if (places.empty()) {
      places_list_->addItem("не найдено подходящих мест");
      return;
   }

   int ind = 1;

   for (const auto& place : places) {
      QString text = QString("%1. %2 (%3), %4 m")
         .arg(ind)
         .arg(QString::fromStdString(place.name))
         .arg(QString::fromStdString(place.type))
         .arg(static_cast<long long>(place.distance_km*1000)
      );

      places_list_->addItem(text);

      ++ind;
   }
}

static QString FindImagePath(const QString& file_name) {
   namespace fs = std::filesystem;

   fs::path path1 = fs::path("../images") / file_name.toStdString();
   fs::path path2 = fs::path("../../images") / file_name.toStdString();

   if (fs::exists(path1)) {
      return QString::fromStdString(path1.string());
   }

   if (fs::exists(path2)) {
      return QString::fromStdString(path2.string());
   }

   return {};
}

void MainWindow::UpdateWeatherImage(WeatherType type) {
   QString file_name;

   switch (type) {
      case (WeatherType::kGood):
         file_name = "good.png";
         break;

      case (WeatherType::kNeutral):
         file_name = "neutral.png";
         break;

      case (WeatherType::kRainy):
         file_name = "rainy.png";
         break;

      case (WeatherType::kSnowy):
         file_name = "snowy.png";
         break;
      
      case (WeatherType::kDangerous):
         file_name = "dangerous.png";
         break;
   }

   QString path = FindImagePath(file_name);

   if (path.isEmpty()) {
      weather_image_->setText("картинка не найдена");
      return;
   }

   QPixmap pixmap(path);

   if (pixmap.isNull()) {
      weather_image_->setText("картинка не найдена");
      return;
   }

   weather_image_->setPixmap(
      pixmap.scaled(
         weather_image_->size(),
         Qt::KeepAspectRatio,
         Qt::SmoothTransformation
      )
   );
}


void MainWindow::OnFindMeClicked() {
   auto location_result = MainLogic::GetLocation();
   
   if (!location_result) {
      QMessageBox::warning(
         this,
         "Ошибка локации",
         QString::fromStdString(location_result.error())
      );

      return;
   }

   UpdateLocation(*location_result);
}

void MainWindow::OnCalcWeatherPlacesClicked() {
   Coordinates coords = ReadCoordinates();

   auto weather_and_places_res = MainLogic::GetWeatherAndPlaces(coords);

   if (!weather_and_places_res) {
      QMessageBox::warning(
         this,
         "Ошибка погоды и мест",
         QString::fromStdString(weather_and_places_res.error())
      );

      return;
   }

   UpdateWeather(weather_and_places_res->weather_info);
   UpdatePlaces(weather_and_places_res->places);
   UpdateWeatherImage(weather_and_places_res->weather_info.type);
}