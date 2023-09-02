#include <opencv4/opencv2/opencv.hpp>

int main()
{
    // Create a video capture object for the screen
    cv::VideoCapture capture(0);

    // Set the capture properties
    capture.set(cv::CAP_PROP_FRAME_WIDTH, 1920);
    capture.set(cv::CAP_PROP_FRAME_HEIGHT, 1080);

    // Create a video writer object to encode the captured frames
    cv::VideoWriter writer("output.mp4", cv::VideoWriter::fourcc('H','2','6','4'), 30, cv::Size(1920, 1080));

    // Capture and encode frames for 10 seconds
    for (int i = 0; i < 300; i++)
    {
        // Capture a frame from the screen
        cv::Mat frame;
        capture.read(frame);

        // Write the frame to the output video
        writer.write(frame);
    }

    // Release the capture and writer objects
    capture.release();
    writer.release();

    return 0;
}
