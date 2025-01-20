#ifndef SIMPLE_PARTICLE_H
#define SIMPLE_PARTICLE_H

#include <wx/wx.h>
#include <vector>
#include <unordered_map>
#include <chrono>

static constexpr int PARTICLE_SIZE = 10;
static constexpr float PARTICLE_SPEED = 1.0f;

// ParticleSimPanel: panel that handles drawing and collision logic
class ParticleSimPanel : public wxPanel
{
public:
    ParticleSimPanel(wxWindow *parent);
    virtual ~ParticleSimPanel();

    void Start();
    void Stop();

    // Add the specified number of particles (positive=add, negative=remove, etc.)
    void addParticle(int number);

    // Return how many we have
    size_t getParticleCount() const;

private:
    // Timer pointer
    wxTimer *timer;

    // We'll store squares in 'rectangles' for clarity
    struct Rectangle
    {
        int positionX;
        int positionY;
        float directionX;
        float directionY;
    };

    // Simple “grid” struct to group particles in cells
    struct Grid
    {
        struct Cell
        {
            std::vector<Rectangle> particles;
        };

        int cellSize;
        int currentWidth;
        int currentHeight;
        int gridWidth;
        int gridHeight;
        std::unordered_map<int, Cell> cells;

        Grid(int cs, int width, int height);
        void resize(int width, int height);
        int getCellID(int x, int y) const;
        std::pair<int, int> getCellCoordinates(int x, int y) const;
        void clear();
        void addParticle(const Rectangle &rect, int x, int y);
    };

    std::vector<Rectangle> rectangles;
    int frameCount;
    double fps;
    std::chrono::steady_clock::time_point lastTime;
    Grid grid;

    // Event handlers
    void OnPaint(wxPaintEvent &event);
    void OnTimer(wxTimerEvent &event);
    void OnResize(wxSizeEvent &event);

    void detectCollisions();
    bool isColliding(const Rectangle &a, const Rectangle &b);
    void handleCollision(Rectangle &a, Rectangle &b);
    void CalculateFPS();

    wxDECLARE_EVENT_TABLE();
};

#endif // SIMPLE_PARTICLE_H
