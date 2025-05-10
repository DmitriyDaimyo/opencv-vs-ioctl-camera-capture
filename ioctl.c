#include <sys/ioctl.h>
#include <fcntl.h>       
#include <unistd.h>         
#include <linux/videodev2.h> 
#include <cstring>         
#include <cstdio>
#include <sys/mman.h>
#include <opencv2/opencv.hpp>


int main(void){

    // open device (requires 70 strings)
    int fd1 = open("/dev/video0", O_RDWR);
        if (fd1 < 0) {
            perror("Failed to open device\n");
            return 1;
        }

    // set format
    v4l2_format fmt = {};
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = 640;
    fmt.fmt.pix.height = 512;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_Y16; //set the FOURCC code to Y16 (16-bit grayscale)
    fmt.fmt.pix.field = V4L2_FIELD_NONE;

    if (ioctl(fd1, VIDIOC_S_FMT, &fmt) < 0) {
        perror("VIDIOC_S_FMT");
        return 1;
    }

    // request buffer
    v4l2_requestbuffers req = {};
    req.count = 1;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if (ioctl(fd1, VIDIOC_REQBUFS, &req) < 0) {
        perror("VIDIOC_REQBUFS");
        return 1;
    }

    // query buffer
    v4l2_buffer buf = {};
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = 0;

    if (ioctl(fd1, VIDIOC_QUERYBUF, &buf) < 0) {
        perror("VIDIOC_QUERYBUF");
        return 1;
    }

    // map memory
    void* buffer = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd1, buf.m.offset);
    if (buffer == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    // start streaming 
    v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(fd1, VIDIOC_STREAMON, &type) < 0) {
        perror("VIDIOC_STREAMON");
        return 1;
    }

    // queue buffer
    if (ioctl(fd1, VIDIOC_QBUF, &buf) < 0) {
        perror("VIDIOC_QBUF");
        return 1;
    }

    // dequeue buffer 
    if (ioctl(fd1, VIDIOC_DQBUF, &buf) < 0) {
        perror("VIDIOC_DQBUF");
        return  1;
    }

    // copy to opencv
    cv::Mat y16(512, 640, CV_16UC1, buffer);

    printf("Size: %d x %d\n", y16.cols, y16.rows);
    printf("Channels: %d\n", y16.channels());
    printf("Depth: %d\n", y16.depth());
    printf("Type: %d\n", y16.type());

    return 0;

}