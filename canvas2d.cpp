#include "canvas2d.h"
#include <QPainter>
#include <QMessageBox>
#include <QFileDialog>
#include <iostream>
#include "settings.h"

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
