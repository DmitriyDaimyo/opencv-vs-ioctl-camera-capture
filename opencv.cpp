#include <opencv2/opencv.hpp>
#include <iostream>

int main() {

    // open device (requires one string)
    cv::VideoCapture cap("/dev/video0", cv::CAP_V4L2);

    if (!cap.isOpened()) {
       perror("Failed to open device\n");
        return 1;
    }

    // set the FOURCC code to Y16 (16-bit grayscale)
    cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('Y', '1', '6', ' '));
    cap.set(cv::CAP_PROP_CONVERT_RGB, 0);

    // set resolution
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 512);

    cv::Mat y16;

    if (!cap.read(y16)) {
        perror("ERROR: Could not grab frame\n");

    }

    printf("Size: %d x %d\n", y16.cols, y16.rows);
    printf("Channels: %d\n", y16.channels());
    printf("Depth: %d\n", y16.depth());
    printf("Type: %d\n", y16.type());

    cap.release();

    return 0;
}
