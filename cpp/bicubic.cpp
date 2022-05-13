#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui_c.h>

#include <iostream>
#include <cmath>
#include <fstream>

std::vector<double> getWeight(double c, double a = 0.5)
{
    //c����u��v����������������������㷽ʽһ��
    std::vector<double> temp(4);
    temp[0] = 1 + c; temp[1] = c;
    temp[2] = 1 - c; temp[3] = 2 - c;

    //y(x) = (a+2)|x|*|x|*|x| - (a+3)|x|*|x| + 1   |x|<=1
    //y(x) = a|x|*|x|*|x| - 5a|x|*|x| + 8a|x| - 4a  1<|x|<2
    std::vector<double> weight(4);
    weight[0] = (a * pow(abs(temp[0]), 3) - 5 * a * pow(abs(temp[0]), 2) + 8 * a * abs(temp[0]) - 4 * a);
    weight[1] = (a + 2) * pow(abs(temp[1]), 3) - (a + 3) * pow(abs(temp[1]), 2) + 1;
    weight[2] = (a + 2) * pow(abs(temp[2]), 3) - (a + 3) * pow(abs(temp[2]), 2) + 1;
    weight[3] = (a * pow(abs(temp[3]), 3) - 5 * a * pow(abs(temp[3]), 2) + 8 * a * abs(temp[3]) - 4 * a);

    return weight;
}

void bicubic(cv::Mat& src, cv::Mat& dst, int dst_rows, int dst_cols)
{
    dst.create(dst_rows, dst_cols, src.type());
    double sy = static_cast<double>(dst_rows) / static_cast<double>(src.rows);
    double sx = static_cast<double>(dst_cols) / static_cast<double>(src.cols);
    cv::Mat border;
    cv::copyMakeBorder(src, border, 1, 1, 1, 1, cv::BORDER_REPLICATE);

    for (int i = 1; i < dst_rows + 1; ++i)
    {
        int src_y = (i + 0.5) / sy - 0.5;
        if (src_y < 0) src_y = 0;
        if (src_y > src.rows - 1) src_y = src.rows - 1;
        src_y += 1;
        int i1 = std::floor(src_y);
        int i2 = std::ceil(src_y);
        int i0 = i1 - 1;
        int i3 = i2 + 1;
        double u = src_y - static_cast<int64>(i1);
        std::vector<double> weight_y = getWeight(u);

        for (int j = 1; j < dst_cols + 1; ++j)
        {
            int src_x = (j + 0.5) / sx - 0.5;
            if (src_x < 0) src_x = 0;
            if (src_x > src.cols - 1) src_x = src.cols - 1;
            src_x += 1;
            int j1 = std::floor(src_x);
            int j2 = std::ceil(src_x);
            int j0 = j1 - 1;
            int j3 = j2 + 1;
            double v = src_x - static_cast<int64>(j1);
            std::vector<double> weight_x = getWeight(v);

            cv::Vec3b pix;

            pix[0] = weight_x[0] * weight_y[0] * border.at<cv::Vec3b>(i0, j0)[0] + weight_x[1] * weight_y[0] * border.at<cv::Vec3b>(i0, j1)[0]
                + weight_x[2] * weight_y[0] * border.at<cv::Vec3b>(i0, j2)[0] + weight_x[3] * weight_y[0] * border.at<cv::Vec3b>(i0, j3)[0]
                + weight_x[0] * weight_y[1] * border.at<cv::Vec3b>(i1, j0)[0] + weight_x[1] * weight_y[1] * border.at<cv::Vec3b>(i1, j1)[0]
                + weight_x[2] * weight_y[1] * border.at<cv::Vec3b>(i1, j2)[0] + weight_x[3] * weight_y[1] * border.at<cv::Vec3b>(i1, j3)[0]
                + weight_x[0] * weight_y[2] * border.at<cv::Vec3b>(i2, j0)[0] + weight_x[1] * weight_y[2] * border.at<cv::Vec3b>(i2, j1)[0]
                + weight_x[2] * weight_y[2] * border.at<cv::Vec3b>(i2, j2)[0] + weight_x[3] * weight_y[2] * border.at<cv::Vec3b>(i2, j3)[0]
                + weight_x[0] * weight_y[3] * border.at<cv::Vec3b>(i3, j0)[0] + weight_x[1] * weight_y[3] * border.at<cv::Vec3b>(i3, j1)[0]
                + weight_x[2] * weight_y[3] * border.at<cv::Vec3b>(i3, j2)[0] + weight_x[3] * weight_y[3] * border.at<cv::Vec3b>(i3, j3)[0];
            pix[1] = weight_x[0] * weight_y[0] * border.at<cv::Vec3b>(i0, j0)[1] + weight_x[1] * weight_y[0] * border.at<cv::Vec3b>(i0, j1)[1]
                + weight_x[2] * weight_y[0] * border.at<cv::Vec3b>(i0, j2)[1] + weight_x[3] * weight_y[0] * border.at<cv::Vec3b>(i0, j3)[1]
                + weight_x[0] * weight_y[1] * border.at<cv::Vec3b>(i1, j0)[1] + weight_x[1] * weight_y[1] * border.at<cv::Vec3b>(i1, j1)[1]
                + weight_x[2] * weight_y[1] * border.at<cv::Vec3b>(i1, j2)[1] + weight_x[3] * weight_y[1] * border.at<cv::Vec3b>(i1, j3)[1]
                + weight_x[0] * weight_y[2] * border.at<cv::Vec3b>(i2, j0)[1] + weight_x[1] * weight_y[2] * border.at<cv::Vec3b>(i2, j1)[1]
                + weight_x[2] * weight_y[2] * border.at<cv::Vec3b>(i2, j2)[1] + weight_x[3] * weight_y[2] * border.at<cv::Vec3b>(i2, j3)[1]
                + weight_x[0] * weight_y[3] * border.at<cv::Vec3b>(i3, j0)[1] + weight_x[1] * weight_y[3] * border.at<cv::Vec3b>(i3, j1)[1]
                + weight_x[2] * weight_y[3] * border.at<cv::Vec3b>(i3, j2)[1] + weight_x[3] * weight_y[3] * border.at<cv::Vec3b>(i3, j3)[1];
            pix[2] = weight_x[0] * weight_y[0] * border.at<cv::Vec3b>(i0, j0)[2] + weight_x[1] * weight_y[0] * border.at<cv::Vec3b>(i0, j1)[2]
                + weight_x[2] * weight_y[0] * border.at<cv::Vec3b>(i0, j2)[2] + weight_x[3] * weight_y[0] * border.at<cv::Vec3b>(i0, j3)[2]
                + weight_x[0] * weight_y[1] * border.at<cv::Vec3b>(i1, j0)[2] + weight_x[1] * weight_y[1] * border.at<cv::Vec3b>(i1, j1)[2]
                + weight_x[2] * weight_y[1] * border.at<cv::Vec3b>(i1, j2)[2] + weight_x[3] * weight_y[1] * border.at<cv::Vec3b>(i1, j3)[2]
                + weight_x[0] * weight_y[2] * border.at<cv::Vec3b>(i2, j0)[2] + weight_x[1] * weight_y[2] * border.at<cv::Vec3b>(i2, j1)[2]
                + weight_x[2] * weight_y[2] * border.at<cv::Vec3b>(i2, j2)[2] + weight_x[3] * weight_y[2] * border.at<cv::Vec3b>(i2, j3)[2]
                + weight_x[0] * weight_y[3] * border.at<cv::Vec3b>(i3, j0)[2] + weight_x[1] * weight_y[3] * border.at<cv::Vec3b>(i3, j1)[2]
                + weight_x[2] * weight_y[3] * border.at<cv::Vec3b>(i3, j2)[2] + weight_x[3] * weight_y[3] * border.at<cv::Vec3b>(i3, j3)[2];

            for (int i = 0; i < src.channels(); ++i)
            {
                if (pix[i] < 0) pix = 0;
                if (pix[i] > 255)pix = 255;
            }


            dst.at<cv::Vec3b>(i - 1, j - 1) = static_cast<cv::Vec3b>(pix);
        }
    }
}

int main()
{
    cv::Mat src = cv::imread("images/0.bmp");
    cv::Mat dst;
    bicubic(src, dst, 2160, 3840);
    cv::imwrite("0.bmp", dst);

}