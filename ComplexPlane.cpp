#include "ComplexPlane.h"
#include <sstream>
#include <cmath>
#include <complex>

using namespace sf;

ComplexPlane::ComplexPlane(int pixelWidth, int pixelHeight)
{
    m_pixelWidth = pixelWidth;
    m_pixelHeight = pixelHeight;

    m_aspectRatio = static_cast<float>(pixelHeight) / static_cast<float>(pixelWidth);

    m_plane_center = { 0.f, 0.f };
    m_plane_size = { BASE_WIDTH, BASE_HEIGHT * m_aspectRatio };

    m_zoomCount = 0;
    m_state = State::CALCULATING;

    // Initialize vertex array
    m_vArray.setPrimitiveType(Points);
    m_vArray.resize(pixelWidth * pixelHeight);
}

void ComplexPlane::draw(RenderTarget& target, RenderStates states) const
{
    target.draw(m_vArray, states);
}

void ComplexPlane::zoomIn()
{
    m_zoomCount++;
    m_plane_size.x = BASE_WIDTH * pow(BASE_ZOOM, m_zoomCount);
    m_plane_size.y = BASE_HEIGHT * m_aspectRatio * pow(BASE_ZOOM, m_zoomCount);
    m_state = State::CALCULATING;
}

void ComplexPlane::zoomOut()
{
    m_zoomCount--;
    m_plane_size.x = BASE_WIDTH * pow(BASE_ZOOM, m_zoomCount);
    m_plane_size.y = BASE_HEIGHT * m_aspectRatio * pow(BASE_ZOOM, m_zoomCount);
    m_state = State::CALCULATING;
}

void ComplexPlane::setCenter(Vector2i mousePixel)
{
    m_plane_center = mapPixelToCoords(mousePixel);
    m_state = State::CALCULATING;
}

void ComplexPlane::setMouseLocation(Vector2i mousePixel)
{
    m_mouseLocation = mapPixelToCoords(mousePixel);
}

void ComplexPlane::loadText(Text& text)
{
    std::stringstream ss;
    ss << "Mandelbrot Set" << "\n";
    ss << "Center: (" << m_plane_center.x << ", " << m_plane_center.y << ")\n";
    ss << "Cursor: (" << m_mouseLocation.x << ", " << m_mouseLocation.y << ")\n";
    ss << "Left-Click to Zoom In" << "\n";
    ss << "Right-Click to Zoom Out" << "\n";
    text.setString(ss.str());
}

void ComplexPlane::updateRender()
{
    if (m_state != State::CALCULATING) return;

    for (int i = 0; i < m_pixelHeight; i++)
    {
        for (int j = 0; j < m_pixelWidth; j++)
        {
            int index = j + i * m_pixelWidth;

            m_vArray[index].position = Vector2f(j, i);
            Vector2f coord = mapPixelToCoords({ j,i });
            size_t iter = countIterations(coord);

            Uint8 r, g, b;
            iterationsToRGB(iter, r, g, b);

            m_vArray[index].color = Color(r, g, b);
        }
    }
    m_state = State::DISPLAYING;
}

size_t ComplexPlane::countIterations(Vector2f coord)
{
    std::complex<double> c(coord.x, coord.y);
    std::complex<double> z(0.0, 0.0);

    size_t iter = 0;

    while (abs(z) <= 2.0 && iter < MAX_ITER)
    {
        z = z * z + c;
        iter++;
    }

    return iter;
}

void ComplexPlane::iterationsToRGB(size_t count, Uint8& r, Uint8& g, Uint8& b)
{
    //Color    RGB
    //Orange   (255, 128, 0)
    //Yellow   (255, 255, 0)
    //Red      (255, 0, 0)
    //White    (255, 255, 255)
    //Pink     (255, 105, 180)

    if (count >= MAX_ITER)
    {
        r = g = b = 0;
        return;
    }

    float t = float(count) / float(MAX_ITER);

    Color c1, c2;

    if (t < 0.20f) {
        c1 = Color(255, 128, 0);  // orange
        c2 = Color(255, 255, 0);  // yellow
        t = t / 0.20f;
    }
    else if (t < 0.40f) {
        c1 = Color(255, 255, 0);  // yellow
        c2 = Color(255, 0, 0);    // red
        t = (t - 0.20f) / 0.20f;
    }
    else if (t < 0.60f) {
        c1 = Color(255, 0, 0);    // red
        c2 = Color(255, 255, 255);// white
        t = (t - 0.40f) / 0.20f;
    }
    else if (t < 0.90f) {
        c1 = Color(255, 255, 255);// white
        c2 = Color(255, 105, 180);// pink
        t = (t - 0.60f) / 0.30f;
    }
    else {
        r = 255;
        g = 105;
        b = 180;
        return;
    }

    r = Uint8(c1.r + t * (c2.r - c1.r));
    g = Uint8(c1.g + t * (c2.g - c1.g));
    b = Uint8(c1.b + t * (c2.b - c1.b));
}

Vector2f ComplexPlane::mapPixelToCoords(Vector2i mousePixel)
{
    float left = m_plane_center.x - m_plane_size.x / 2.0f;
    float right = m_plane_center.x + m_plane_size.x / 2.0f;
    float bottom = m_plane_center.y - m_plane_size.y / 2.0f;
    float top = m_plane_center.y + m_plane_size.y / 2.0f;

    float real = ((mousePixel.x) / float(m_pixelWidth)) * (right - left) + left;
    float imag = ((m_pixelHeight - mousePixel.y) / float(m_pixelHeight)) * (top - bottom) + bottom;
    return { real, imag };
}
