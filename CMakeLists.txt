project (ThermalBitmap)

add_executable (ThermalBitmap main.cpp)

enable_testing()
add_test(NAME Test1 COMMAND ThermalBitmap thermalframe0.csv thermalframe0.bmp)