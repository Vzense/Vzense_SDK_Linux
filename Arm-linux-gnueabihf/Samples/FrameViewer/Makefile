TARGET := FrameViewer

SOURCE := FrameViewer.cpp

FLAGS := -I../../Include -I../../Thirdparty/opencv-3.4.1/include -Wl,-rpath,./:../../Lib:../Lib:../../Thirdparty/opencv-3.4.1/lib:../../Thirdparty/ffmpeg/lib:../Thirdparty/opencv-3.4.1/lib:../Thirdparty/ffmpeg/lib -Wall -Wconversion -O3 -L../../Lib -lvzense_api -L../../Thirdparty/opencv-3.4.1/lib -lopencv_flann -lopencv_imgcodecs -lopencv_calib3d -lopencv_highgui -lopencv_imgproc -lopencv_core

$(TARGET):$(SOURCE)
	g++ -std=c++11 -o $(TARGET) $(SOURCE) $(FLAGS) 

clean:
	rm -rf *.o $(TARGET)
