#ifndef CANVAS2D_H
#define CANVAS2D_H

#include <QLabel>
#include <QMouseEvent>
#include <array>
#include "rgba.h"

class Canvas2D : public QLabel {
    Q_OBJECT
public:
    bool m_isDown = false;

    int m_width = 0;
    int m_height = 0;

    int mask_width = 0;
    int mask_height = 0;


    void init();
    void clearCanvas();
    bool loadImageFromFile(const QString &file);
    bool saveImageToFile(const QString &file);
    void displayImage();
    void resize(int w, int h);

    // This will be called when the settings have changed
    void settingsChanged();

    // Filter TODO: implement
    void filterImage();

private:
    std::vector<RGBA> m_data;
    std::vector<float> mask;
    std::vector<RGBA> smudge_pickup;
    std::vector<SOBELIMG> temp_sobel_image;
    std::vector<SOBELIMG> sobel_result;
    bool sobeled = false;
    int newWidth = 500;
    int newHeight = 500;

    void mouseDown(int x, int y);
    void mouseDragged(int x, int y);
    void mouseUp(int x, int y);

    // These are functions overriden from QWidget that we've provided
    // to prevent you from having to interact with Qt's mouse events.
    // These will pass the mouse coordinates to the above mouse functions
    // that you will have to fill in.
    virtual void mousePressEvent(QMouseEvent* event) override {
        auto [x, y] = std::array{ event->position().x(), event->position().y() };
        mouseDown(static_cast<int>(x), static_cast<int>(y));
    }
    virtual void mouseMoveEvent(QMouseEvent* event) override {
        auto [x, y] = std::array{ event->position().x(), event->position().y() };
        mouseDragged(static_cast<int>(x), static_cast<int>(y));
    }
    virtual void mouseReleaseEvent(QMouseEvent* event) override {
        auto [x, y] = std::array{ event->position().x(), event->position().y() };
        mouseUp(static_cast<int>(x), static_cast<int>(y));
    }

    // TODO: add any member variables or functions you need
    int posToIndex(int x, int y);
    int maskToCanvas(int x, int y, int canvas_x, int canvas_y, int radius);
    void initConstantMask(int radius);
    void initLinearMask(int radius);
    void initQuadraticMask(int radius);
    void initSmudgeMask(int x, int y, int radius);
    bool checkClearSmudge(int x, int y);

    void brush(int x, int y);
    void brushSmudge(int x, int y);

    void convolve(std::vector<RGBA> &data, int width, int height, const std::vector<float> &kernel, int k_width, int k_height, bool sobel);
    std::vector<float> filterBlur(int radius);
    std::vector<RGBA> filterSobel(float sensitivity);
    std::vector<RGBA> filterScaling(std::vector<RGBA> &data, int width, int height, float x, float y, bool horizontal);
    RGBA h_prime(std::vector<RGBA> &data, int width, int height, int k, double a, int fixed, bool horizontal);
    double g(double x, double a);
    void filterGray();
    std::uint8_t rgbaToGray(const RGBA &pixel);
    int getPixelRepeated(int width, int height, int x, int y);
    int getPixelReflected(int width, int height, int x, int y);
    int getPixelWrapped(int width, int height, int x, int y);
};

#endif // CANVAS2D_H
