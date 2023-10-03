#include "canvas2d.h"
#include <QPainter>
#include <QMessageBox>
#include <QFileDialog>
#include <iostream>
#include "settings.h"
#include <cmath>

/**
 * @brief Initializes new 500x500 canvas
 */
void Canvas2D::init() {
    setMouseTracking(true);
    m_width = 500;
    m_height = 500;
    clearCanvas();
}

/**
 * @brief Canvas2D::clearCanvas sets all canvas pixels to blank white
 */
void Canvas2D::clearCanvas() {
    m_data.assign(m_width * m_height, RGBA{255, 255, 255, 255});
    settings.imagePath = "";
    displayImage();
}

/**
 * @brief Stores the image specified from the input file in this class's
 * `std::vector<RGBA> m_image`.
 * Also saves the image width and height to canvas width and height respectively.
 * @param file: file path to an image
 * @return True if successfully loads image, False otherwise.
 */
bool Canvas2D::loadImageFromFile(const QString &file) {
    QImage myImage;
    if (!myImage.load(file)) {
        std::cout<<"Failed to load in image"<<std::endl;
        return false;
    }
    myImage = myImage.convertToFormat(QImage::Format_RGBX8888);
    m_width = myImage.width();
    m_height = myImage.height();
    QByteArray arr = QByteArray::fromRawData((const char*) myImage.bits(), myImage.sizeInBytes());

    m_data.clear();
    m_data.reserve(m_width * m_height);
    for (int i = 0; i < arr.size() / 4.f; i++){
        m_data.push_back(RGBA{(std::uint8_t) arr[4*i], (std::uint8_t) arr[4*i+1], (std::uint8_t) arr[4*i+2], (std::uint8_t) arr[4*i+3]});
    }
    displayImage();
    return true;
}

/**
 * @brief Saves the current canvas image to the specified file path.
 * @param file: file path to save image to
 * @return True if successfully saves image, False otherwise.
 */
bool Canvas2D::saveImageToFile(const QString &file) {
    QImage myImage = QImage(m_width, m_height, QImage::Format_RGBX8888);
    for (int i = 0; i < m_data.size(); i++){
        myImage.setPixelColor(i % m_width, i / m_width, QColor(m_data[i].r, m_data[i].g, m_data[i].b, m_data[i].a));
    }
    if (!myImage.save(file)) {
        std::cout<<"Failed to save image"<<std::endl;
        return false;
    }
    return true;
}


/**
 * @brief Get Canvas2D's image data and display this to the GUI
 */
void Canvas2D::displayImage() {
    QByteArray* img = new QByteArray(reinterpret_cast<const char*>(m_data.data()), 4*m_data.size());
    QImage now = QImage((const uchar*)img->data(), m_width, m_height, QImage::Format_RGBX8888);
    setPixmap(QPixmap::fromImage(now));
    setFixedSize(m_width, m_height);
    update();
}

/**
 * @brief Canvas2D::resize resizes canvas to new width and height
 * @param w
 * @param h
 */
void Canvas2D::resize(int w, int h) {
    m_width = w;
    m_height = h;
    m_data.resize(w * h);
    displayImage();
}

/**
 * @brief Called when the filter button is pressed in the UI
 */
void Canvas2D::filterImage() {
    // Filter TODO: apply the currently selected filter to the loaded image
    temp_sobel_image.assign(m_data.size(), SOBELIMG{0.0, 0.0, 0.0, 255});
    if (settings.filterType == FILTER_BLUR){
        int radius = settings.blurRadius;
        std::vector<float> filter = filterBlur(radius);
        int filter_length = 2 * radius + 1;
        convolve(m_data, m_width, m_height, filter, filter_length, 1, false);
        convolve(m_data, m_width, m_height, filter, 1, filter_length, false);
    }else if (settings.filterType == FILTER_EDGE_DETECT){
        float sensitivity = settings.edgeDetectSensitivity;
        std::vector<RGBA> result = filterSobel(sensitivity);
        m_data = result;
    }else if (settings.filterType == FILTER_SCALE){
        float x = settings.scaleX;
        float y = settings.scaleY;
        std::vector<RGBA> intermediate;
        intermediate = filterScaling(m_data, m_width, m_height, x, 1.0f, true);
        std::vector<RGBA> result = filterScaling(intermediate, newWidth, newHeight, 1.0f, y, false);
        m_width = newWidth;
        m_height = newHeight;
        m_data = result;
    }
    displayImage();
}

/**
 * @brief Called when any of the parameters in the UI are modified.
 */
void Canvas2D::settingsChanged() {
    // this saves your UI settings locally to load next time you run the program
    settings.saveSettings();
    int radius = settings.brushRadius;

    if (settings.brushType == BRUSH_CONSTANT){
        initConstantMask(radius);
    }else if (settings.brushType == BRUSH_LINEAR){
        initLinearMask(radius);
    }else if (settings.brushType == BRUSH_QUADRATIC){
        initQuadraticMask(radius);
    }else if (settings.brushType == BRUSH_SMUDGE){
        initLinearMask(radius);
    }

    // TODO: fill in what you need to do when brush or filter parameters change
}
void Canvas2D::initConstantMask(int radius){
    mask_width = 2 * radius + 1;
    mask_height = 2 * radius + 1;
    mask.assign(mask_width * mask_height, 0.0);
    for (int i = 0; i < mask_width; i++){
        for (int j = 0; j < mask_height; j++){
            float distance = sqrt(pow(i - radius, 2) + pow(j - radius, 2));
            if (distance <= radius){
                mask.at(j *mask_width + i) = 1.0;
            }
        }
    }


}
void Canvas2D::initLinearMask(int radius){
    mask_width = 2 * radius + 1;
    mask_height = 2 * radius + 1;
    mask.assign(mask_width * mask_height, 0.0);
    for (int i = 0; i < mask_width; i++){
        for (int j = 0; j < mask_height; j++){
            float distance = sqrt(pow(i - radius, 2) + pow(j - radius, 2));
            if (distance <= radius){
                float opacity = 1 - distance / radius;
                mask.at(j *mask_width + i) = opacity;
            }
        }
    }
}

void Canvas2D::initQuadraticMask(int radius){
    mask_width = 2 * radius + 1;
    mask_height = 2 * radius + 1;
    mask.assign(mask_width * mask_height, 0.0);
    for (int i = 0; i < mask_width; i++){
        for (int j = 0; j < mask_height; j++){
            float distance = sqrt(pow(i - radius, 2) + pow(j - radius, 2));
            if (distance <= radius){
                float A = 1.0 / pow(radius, 2);
                float B = 2.0 / radius;
                float opacity = A * pow(distance, 2) - B * distance + 1;
                mask.at(j *mask_width + i) = opacity;
            }
        }
    }
}

void Canvas2D::initSmudgeMask(int x, int y, int radius){
    mask_width = 2 * radius + 1;
    mask_height = 2 * radius + 1;
    smudge_pickup.assign(mask_width * mask_height, RGBA{0, 0, 0, 0});

    for (int i = 0; i < mask_width; i++){
        for (int j = 0; j < mask_height; j++){
            int canvas_x = x + i - radius;
            int canvas_y = y + j - radius;
            if (canvas_x >= m_width || canvas_y >= m_height || canvas_x < 0 || canvas_y < 0){
                smudge_pickup[j *mask_width + i].b = 0.0;
                smudge_pickup[j *mask_width + i].g = 0.0;
                smudge_pickup[j *mask_width + i].r = 0.0;
                smudge_pickup[j *mask_width + i].a = 0.0;
                continue;
            }
            smudge_pickup[j *mask_width + i].b = m_data[maskToCanvas(i, j, x, y, radius)].b;
            smudge_pickup[j *mask_width + i].g = m_data[maskToCanvas(i, j, x, y, radius)].g;
            smudge_pickup[j *mask_width + i].r = m_data[maskToCanvas(i, j, x, y, radius)].r;
            smudge_pickup[j *mask_width + i].a = m_data[maskToCanvas(i, j, x, y, radius)].a;
        }
    }
    if (checkClearSmudge(x, y) == true){

        smudge_pickup.assign(mask_width * mask_height, RGBA{0, 0, 0, 0});
    }
}

int Canvas2D::posToIndex(int x, int y){
    return y * m_width + x;
}
int Canvas2D::maskToCanvas(int x, int y, int canvas_x, int canvas_y, int radius){
    int new_x = canvas_x + x - radius;
    int new_y = canvas_y + y - radius;
    int ret = posToIndex(new_x, new_y);
    return ret;

}

bool Canvas2D::checkClearSmudge(int x, int y){
     int radius = settings.brushRadius;
    if (x > radius + m_width || y > radius + m_height || x <  - radius || y < - radius){
        return true;
    }else{
        return false;
    }
}

void Canvas2D::brush(int x, int y){
    int radius = settings.brushRadius;
    RGBA color = settings.brushColor;
    float alpha = color.a / 255.0;


    for (int i = 0; i < mask_width; i++){
        for (int j = 0; j < mask_height; j++){
            int canvas_x = x + i - radius;
            int canvas_y = y + j - radius;
            if (canvas_x >= m_width || canvas_y >= m_height || canvas_x < 0 || canvas_y < 0){
                continue;
            }else{
                float opacity = mask[j *mask_width + i];
                m_data[posToIndex(canvas_x, canvas_y)].r = (alpha * opacity) * color.r + (1 - alpha * opacity) * m_data[posToIndex(canvas_x, canvas_y)].r;
                m_data[posToIndex(canvas_x, canvas_y)].g = (alpha * opacity) * color.g + (1 - alpha * opacity) * m_data[posToIndex(canvas_x, canvas_y)].g;
                m_data[posToIndex(canvas_x, canvas_y)].b = (alpha * opacity) * color.b + (1 - alpha * opacity) * m_data[posToIndex(canvas_x, canvas_y)].b;
            }
        }
    }

}

void Canvas2D::brushSmudge(int x, int y){
    int radius = settings.brushRadius;
    if (checkClearSmudge(x, y) == true){
        smudge_pickup.assign(mask_width * mask_height, RGBA{0, 0, 0, 0});
    }

    for (int i = 0; i < mask_width; i++){
        for (int j = 0; j < mask_height; j++){
            int canvas_x = x + i - radius;
            int canvas_y = y + j - radius;
            if (canvas_x >= m_width || canvas_y >= m_height || canvas_x < 0 || canvas_y < 0){
                continue;
            }else{
                float opacity = mask[j *mask_width + i];
                float alpha = (float)smudge_pickup[j *mask_width + i].a / 255;

                m_data[posToIndex(canvas_x, canvas_y)].r = 0.5f + alpha * opacity * smudge_pickup[j *mask_width + i].r + (1 - alpha * opacity) * m_data[posToIndex(canvas_x, canvas_y)].r;
                m_data[posToIndex(canvas_x, canvas_y)].g = 0.5f + alpha * opacity * smudge_pickup[j *mask_width + i].g + (1 - alpha * opacity) * m_data[posToIndex(canvas_x, canvas_y)].g;
                m_data[posToIndex(canvas_x, canvas_y)].b = 0.5f + alpha * opacity * smudge_pickup[j *mask_width + i].b + (1 - alpha * opacity) * m_data[posToIndex(canvas_x, canvas_y)].b;

                smudge_pickup[j *mask_width + i].r = 0.5f + opacity * m_data[posToIndex(canvas_x, canvas_y)].r + (1 - opacity) * smudge_pickup[j *mask_width + i].r;
                smudge_pickup[j *mask_width + i].g = 0.5f + opacity * m_data[posToIndex(canvas_x, canvas_y)].g + (1 - opacity) * smudge_pickup[j *mask_width + i].g;
                smudge_pickup[j *mask_width + i].b = 0.5f + opacity * m_data[posToIndex(canvas_x, canvas_y)].b + (1 - opacity) * smudge_pickup[j *mask_width + i].b;
            }
        }
    }

}
// Repeats the pixel on the edge of the image such that A,B,C,D looks like ...A,A,A,B,C,D,D,D...
int Canvas2D::getPixelRepeated(int width, int height, int x, int y) {
    int newX = (x < 0) ? 0 : std::min(x, width  - 1);
    int newY = (y < 0) ? 0 : std::min(y, height - 1);
    return width * newY + newX;
}

// Flips the edge of the image such that A,B,C,D looks like ...C,B,A,B,C,D,C,B...
int Canvas2D::getPixelReflected(int width, int height, int x, int y) {
    // Task 9: implement this function
    int newX = (x < 0) ? -x : width - abs(x - width + 1) - 1;
    int newY = (y < 0) ? -y : height - abs(y - height + 1) - 1;
    return width * newY + newX;
}

// Wraps the image such that A,B,C,D looks like ...C,D,A,B,C,D,A,B...
int Canvas2D::getPixelWrapped(int width, int height, int x, int y) {
    int newX = (x < 0) ? x + width  : x % width;
    int newY = (y < 0) ? y + height : y % height;
    return width * newY + newX;
}

std::uint8_t Canvas2D::rgbaToGray(const RGBA &pixel) {
    // Task 3: Calculate and return the gray value of the
    //         pixel using its RGB components.
    // Note: We are returning an std::uint8_t, because it is 1 byte
    //       and can store values from 0-255!
    std::uint8_t R = pixel.r;
    std::uint8_t G = pixel.g;
    std::uint8_t B = pixel.b;
    std::uint8_t Y = 0.299 * R + 0.587 * G + 0.114 * B;

    return Y;
}

void Canvas2D::filterGray() {
    for (int row = 0; row < m_height; ++row) {
        for (int col = 0; col < m_width; ++col) {
            size_t currentIndex = m_width * row + col;
            RGBA &currentPixel = m_data[currentIndex];

            // Task 2: call rgbaToGray()
            std::uint8_t gray_currentPixel = rgbaToGray(currentPixel);

            // Task 4: Update currentPixel's color
            currentPixel.r = gray_currentPixel;
            currentPixel.g = gray_currentPixel;
            currentPixel.b = gray_currentPixel;
        }
    }
}


std::vector<float> Canvas2D::filterBlur(int radius){
    float sigma = radius / 3.f;
    if (sigma < 1.0){
        sigma = 1.0;
    }
    int filter_size = radius * 2 + 1;
    std::vector<float> gaussianfilter;
    gaussianfilter.assign(filter_size, 0.0);
    float sum = 0;
    for (int x = 0; x < filter_size; x++){
        float x1 = sqrt(2 * M_PI * (sigma * sigma));
        float x2 = exp(-(pow((x - radius), 2)/(2*(sigma*sigma))));
        gaussianfilter[x] = (1/x1)*x2;
        sum = sum + (1/x1)*x2;
    }

    for (int i = 0; i < gaussianfilter.size(); i++){
        gaussianfilter[i] = gaussianfilter[i] / sum;
    }

    return gaussianfilter;

}

std::vector<RGBA> Canvas2D::filterSobel(float sensitivity){
    filterGray();
    sobel_result.assign(m_data.size(), SOBELIMG{0.0, 0.0, 0.0, 255});
    std::vector<RGBA> d = m_data;
    std::vector<float> G_x1 = {1.0, 2.0, 1.0};
    std::vector<float> G_x2 = {-1.0, 0.0, 1.0};
    std::vector<float> G_y1 = {-1.0, 0.0, 1.0};
    std::vector<float> G_y2 = {1.0, 2.0, 1.0};

    std::vector<RGBA> image1;
    image1 = d;
    std::vector<RGBA> image2;
    image2 = d;
    convolve(image1, m_width, m_height, G_x1, 1, 3, false);
    convolve(image1, m_width, m_height, G_x2, 3, 1, true);
    std::vector<SOBELIMG> fin_image1;
    fin_image1 = sobel_result;

    convolve(image2, m_width, m_height, G_y1, 1, 3, false);
    convolve(image2, m_width, m_height, G_x2, 3, 1, true);
    std::vector<SOBELIMG> fin_image2;
    fin_image2 = sobel_result;

    std::vector<RGBA> G;
    G.assign(m_data.size(), RGBA{0, 0, 0, 255});
    for (int i = 0; i < m_data.size(); i++){
        float gradientMagnitude = sqrt(pow(fin_image1[i].r, 2) + pow(fin_image2[i].r, 2) ) * sensitivity;
        uint8_t gradientValue = static_cast<uint8_t>(std::clamp(gradientMagnitude, 0.0f, 255.0f));
        G[i].r = gradientValue;
        G[i].g = gradientValue;
        G[i].b = gradientValue;
    }

    return G;

}

std::vector<RGBA> Canvas2D::filterScaling(std::vector<RGBA> &data, int width, int height, float x, float y, bool horizontal){
    newWidth = round(width * x);
    newHeight = round(height * y);
    std::vector<RGBA> result;
    result.assign(newWidth * newHeight, RGBA{0, 0, 0, 255});
    for (int i = 0; i < newWidth; i++){
        for (int j = 0; j < newHeight; j++){
            if (horizontal){
                result[j * newWidth + i] = h_prime(data, width, height, i, x, j, true);
            }else{
                result[j * newWidth + i] = h_prime(data, width, height, j, y, i, false);
            }
        }
    }
    return result;

}

RGBA Canvas2D::h_prime(std::vector<RGBA> &data, int width, int height, int k, double a, int fixed, bool horizontal){
    double sumR = 0, sumG = 0, sumB = 0, weights_sum = 0;
    int left, right;

    double center = k/a + (1-a)/(2*a);
    double radius = (a > 1) ? 1 : 1/a;

    left = std::ceil(center - radius);
    right = std::floor(center + radius);

    for (int i = left; i <= right; i++){
        int index = horizontal ? (fixed * width + i)  : (i * width + fixed) ;
//        if (index >= 0 && index < data.size()){
//            sumR +=  g(i - center, a) * data[index].r;
//            sumG +=  g(i - center, a) * data[index].g;
//            sumB +=  g(i - center, a) * data[index].b;
//        }else{
            if (horizontal){
                sumR +=  g(i - center, a) * data[getPixelRepeated(width, height, i, fixed)].r;
                sumG +=  g(i - center, a) * data[getPixelRepeated(width, height, i, fixed)].g;
                sumB +=  g(i - center, a) * data[getPixelRepeated(width, height, i, fixed)].b;

            }else{
                sumR +=  g(i - center, a) * data[getPixelRepeated(width, height, fixed, i)].r;
                sumG +=  g(i - center, a) * data[getPixelRepeated(width, height, fixed, i)].g;
                sumB +=  g(i - center, a) * data[getPixelRepeated(width, height, fixed, i)].b;

            }
//        }
        weights_sum += g(i - center, a);
    }

    RGBA result;
    result.r = static_cast<std::uint8_t> (sumR / weights_sum);
    result.g = static_cast<std::uint8_t> (sumG / weights_sum);
    result.b = static_cast<std::uint8_t> (sumB / weights_sum);
    return result;
}

double Canvas2D::g(double x, double a){
    double radius;
    radius = a < 1 ? 1.0/a : 1.0;
    if ((x < -radius) || (x > radius)) {
        return 0;
    } else {
        return (1 - fabs(x)/radius) / radius;
    }
}

void Canvas2D::convolve(std::vector<RGBA> &data, int width, int height, const std::vector<float> &kernel, int k_width, int k_height, bool sobel) {
    std::vector<RGBA> result;
    result.assign(data.size(), RGBA{0, 0, 0, 255});

    for (int r = 0; r < height; r++) {
        for (int c = 0; c < width; c++) {
            size_t centerIndex = r * width + c;

            float redAcc = 0.0;
            float greenAcc = 0.0;
            float blueAcc = 0.0;


            for (int i = 0; i < k_height; i++){
                for (int j = 0; j < k_width; j++){
                    int central_kernel_index = (k_width - 1) / 2;

                    int flipped_i = k_height - 1 - i;
                    int flipped_j = k_width - 1 - j;

                    int curr_data_i = (i - central_kernel_index) + r;
                    int curr_data_j = (j - central_kernel_index) + c;

                    if (sobel){
                            redAcc +=  (float) temp_sobel_image[getPixelReflected(width, height, curr_data_j, curr_data_i)].r * kernel[flipped_i * k_width + flipped_j];
                            greenAcc += (float) temp_sobel_image[getPixelReflected(width, height, curr_data_j, curr_data_i)].g * kernel[flipped_i * k_width + flipped_j];
                            blueAcc += (float) temp_sobel_image[getPixelReflected(width, height, curr_data_j, curr_data_i)].b * kernel[flipped_i * k_width + flipped_j];
                    }else{
                            redAcc +=  (float) data[getPixelReflected(width, height, curr_data_j, curr_data_i)].r * kernel[flipped_i * k_width + flipped_j];
                            greenAcc += (float) data[getPixelReflected(width, height, curr_data_j, curr_data_i)].g * kernel[flipped_i * k_width + flipped_j];
                            blueAcc += (float) data[getPixelReflected(width, height, curr_data_j, curr_data_i)].b * kernel[flipped_i * k_width + flipped_j];
                    }
                }
            }
            if (sobel){
                sobel_result[centerIndex].r = redAcc;
                sobel_result[centerIndex].g = greenAcc;
                sobel_result[centerIndex].b = blueAcc;
            }else{
                temp_sobel_image[centerIndex].r = redAcc;
                temp_sobel_image[centerIndex].g = greenAcc;
                temp_sobel_image[centerIndex].b = blueAcc;
                result[centerIndex].r = std::max(0.0f, std::min(255.0f, redAcc));
                result[centerIndex].g = std::max(0.0f, std::min(255.0f, greenAcc));
                result[centerIndex].b = std::max(0.0f, std::min(255.0f, blueAcc));
                result[centerIndex].a = 255;

            }

        }
    }
    for (int index = 0; index < data.size(); index++){
        data[index] = result[index];
    }

}


/**
 * @brief These functions are called when the mouse is clicked and dragged on the canvas
 */
void Canvas2D::mouseDown(int x, int y) {
    // Brush TODO
    m_isDown = true;
    int radius = settings.brushRadius;

    if (settings.brushType == BRUSH_CONSTANT){
        initConstantMask(radius);
    }else if (settings.brushType == BRUSH_LINEAR){
        initLinearMask(radius);
    }else if (settings.brushType == BRUSH_QUADRATIC){
        initQuadraticMask(radius);
    }else if (settings.brushType == BRUSH_SMUDGE){
        initLinearMask(radius);
        initSmudgeMask(x, y, settings.brushRadius);
    }
    mouseDragged(x, y);
}

void Canvas2D::mouseDragged(int x, int y) {
    // Brush TODO
    if (m_isDown == true){

        if (settings.brushType == BRUSH_SMUDGE){
            brushSmudge(x, y);
            initSmudgeMask(x, y, settings.brushRadius);
        }else{
            brush(x, y);
        }
    }
    displayImage();
}

void Canvas2D::mouseUp(int x, int y) {
    // Brush TODO
    m_isDown = false;
}
