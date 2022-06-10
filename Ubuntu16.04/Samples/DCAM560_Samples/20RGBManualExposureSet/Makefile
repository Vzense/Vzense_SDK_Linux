TARGET := RGBManualExposureSet

SOURCE := RGBManualExposureSet.cpp

FLAGS := -I../../../Include -Wl,-rpath,./:../Lib/:../../../Lib/:-Wall -Wconversion -O3 -L../../../Lib -lvzense_api 

$(TARGET):$(SOURCE)
	g++ -std=c++11 -o $(TARGET) $(SOURCE) $(FLAGS) 

clean:
	rm -rf *.o $(TARGET)
