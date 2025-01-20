#include "simpleParticle.h"
#include <wx/timer.h>
#include <wx/dcbuffer.h>
#include <random>
#include <cmath>
#include <iostream>

// Unique ID for the timer event
enum
{
    ID_PANEL_TIMER = wxID_HIGHEST + 1
};

wxBEGIN_EVENT_TABLE(ParticleSimPanel, wxPanel)
    // If you prefer static event table over Bind, uncomment:
    //   EVT_PAINT        (ParticleSimPanel::OnPaint)
    //   EVT_TIMER        (ID_PANEL_TIMER, ParticleSimPanel::OnTimer)
    //   EVT_SIZE         (ParticleSimPanel::OnResize)
    wxEND_EVENT_TABLE()

    // --------------- Grid struct methods ---------------
    ParticleSimPanel::Grid::Grid(int cs, int width, int height)
    : cellSize(cs), currentWidth(width), currentHeight(height),
      gridWidth((width + cellSize - 1) / cellSize),
      gridHeight((height + cellSize - 1) / cellSize)
{
}

int ParticleSimPanel::Grid::getCellID(int x, int y) const
{
    return y * gridWidth + x;
}

std::pair<int, int> ParticleSimPanel::Grid::getCellCoordinates(int x, int y) const
{
    return {x / cellSize, y / cellSize};
}

void ParticleSimPanel::Grid::clear()
{
    cells.clear();
}

void ParticleSimPanel::Grid::resize(int width, int height)
{
    currentWidth = width;
    currentHeight = height;
    gridWidth = (width + cellSize - 1) / cellSize;
    gridHeight = (height + cellSize - 1) / cellSize; // <-- Corrected
    cells.clear();
}

void ParticleSimPanel::Grid::addParticle(const Rectangle &rect, int x, int y)
{
    int left = x;
    int right = x + PARTICLE_SIZE;
    int top = y;
    int bottom = y + PARTICLE_SIZE;

    int cellXStart = left / cellSize;
    int cellXEnd = (right - 1) / cellSize;
    int cellYStart = top / cellSize;
    int cellYEnd = (bottom - 1) / cellSize;

    for (int cx = cellXStart; cx <= cellXEnd; ++cx)
    {
        for (int cy = cellYStart; cy <= cellYEnd; ++cy)
        {
            if (cx < 0 || cx >= gridWidth || cy < 0 || cy >= gridHeight)
                continue;
            int cellID = getCellID(cx, cy);
            cells[cellID].particles.push_back(rect);
        }
    }
}

ParticleSimPanel::ParticleSimPanel(wxWindow *parent)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize),
      timer(new wxTimer(this, ID_PANEL_TIMER)),
      frameCount(0),
      fps(0.0),
      lastTime(std::chrono::steady_clock::now()),
      grid(50, 800, 600) // example cell size=50, panel=800x600
{
    // Start the timer ~60 FPS
    timer->Start(11);

    // If you prefer dynamic event binding:
    Bind(wxEVT_PAINT, &ParticleSimPanel::OnPaint, this);
    Bind(wxEVT_TIMER, &ParticleSimPanel::OnTimer, this);
    Bind(wxEVT_SIZE, &ParticleSimPanel::OnResize, this);

    // ADD a few rectangles by default so we see something
    addParticle(10);

    // Optionally enable double-buffer background style:
    SetBackgroundStyle(wxBG_STYLE_PAINT);
}

ParticleSimPanel::~ParticleSimPanel()
{
    timer->Stop();
    delete timer;
}

void ParticleSimPanel::Start()
{
    if (!timer->IsRunning())
        timer->Start(11);
}

void ParticleSimPanel::Stop()
{
    timer->Stop();
}

void ParticleSimPanel::addParticle(int number)
{
    // Negative => remove some
    if (number < 0)
    {
        for (int i = 0; i < -number; ++i)
        {
            if (!rectangles.empty())
                rectangles.pop_back();
        }
        return;
    }

    // Positive => add random squares
    wxSize panelSize = GetSize();

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distXY(-500, 500);
    std::uniform_real_distribution<float> distDIR(-PARTICLE_SPEED, PARTICLE_SPEED);

    for (int i = 0; i < number; i++)
    {
        float directionX = distDIR(gen);
        float directionY = distDIR(gen);
        if (directionX == 0)
            directionX = (gen() % 2 == 0) ? 1 : -1;
        if (directionY == 0)
            directionY = (gen() % 2 == 0) ? 1 : -1;

        rectangles.push_back({// Start near the center plus some random offset
                              panelSize.GetWidth() / 2 + distXY(gen),
                              panelSize.GetHeight() / 2 + distXY(gen) / 3,
                              directionX,
                              directionY});
    }
}

size_t ParticleSimPanel::getParticleCount() const
{
    return rectangles.size();
}

// Called when the panel is resized
void ParticleSimPanel::OnResize(wxSizeEvent &event)
{
    wxSize newSize = GetSize();
    grid.resize(newSize.GetWidth(), newSize.GetHeight());
    event.Skip();
}

// Called whenever the panel is repainted
void ParticleSimPanel::OnPaint(wxPaintEvent &event)
{
    wxAutoBufferedPaintDC dc(this); // double-buffer
    dc.SetBackground(*wxWHITE_BRUSH);
    dc.Clear();

    int colorConstant = 255 / std::sqrt(2 * PARTICLE_SPEED * PARTICLE_SPEED);

    // Draw each rectangle
    for (auto &rect : rectangles)
    {
        float speed = std::sqrt(rect.directionX * rect.directionX + rect.directionY * rect.directionY);
        int colorIntensity = std::min(255, static_cast<int>(colorConstant * speed));
        wxColour color(colorIntensity, 0, 255 - colorIntensity);
        dc.SetBrush(wxBrush(color));
        dc.DrawRectangle(rect.positionX, rect.positionY, PARTICLE_SIZE, PARTICLE_SIZE);
    }

    // Display some text (FPS & # of squares)
    dc.SetFont(wxFontInfo(14).Family(wxFONTFAMILY_SWISS).Bold());
    dc.SetTextBackground(*wxLIGHT_GREY);
    dc.SetTextForeground(*wxRED);
    dc.DrawText(wxString::Format("FPS: %.2f", fps), 10, 30);
    dc.DrawText(wxString::Format("Particles: %zu", getParticleCount()), 10, 60);
}

// Called ~60 times/second by the timer
void ParticleSimPanel::OnTimer(wxTimerEvent &event)
{
    wxSize panelSize = GetSize();
    grid.clear();

    // Move each rectangle
    for (auto &rect : rectangles)
    {
        rect.positionX += (int)(rect.directionX * 5);
        rect.positionY += (int)(rect.directionY * 5);

        // Bounce if outside panel
        if (rect.positionX <= 0 || rect.positionX + PARTICLE_SIZE >= panelSize.GetWidth())
            rect.directionX *= -1;
        if (rect.positionY <= 0 || rect.positionY + PARTICLE_SIZE >= panelSize.GetHeight())
            rect.directionY *= -1;

        // Add to the grid for collision checks
        grid.addParticle(rect, rect.positionX, rect.positionY);
    }

    // Collision checks
    detectCollisions();
    CalculateFPS();

    // Request a redraw
    Refresh();
}

void ParticleSimPanel::detectCollisions()
{
    for (auto &[cellID, cell] : grid.cells)
    {
        auto &particles = cell.particles;
        for (size_t i = 0; i < particles.size(); i++)
        {
            for (size_t j = i + 1; j < particles.size(); j++)
            {
                if (isColliding(particles[i], particles[j]))
                {
                    handleCollision(particles[i], particles[j]);
                    std::cout << "Collision detected" << std::endl;
                }
            }
        }
    }
}

bool ParticleSimPanel::isColliding(const Rectangle &a, const Rectangle &b)
{
    // AABB overlap test
    return !(
        a.positionX + PARTICLE_SIZE < b.positionX ||
        a.positionX > b.positionX + PARTICLE_SIZE ||
        a.positionY + PARTICLE_SIZE < b.positionY ||
        a.positionY > b.positionY + PARTICLE_SIZE);
}

void ParticleSimPanel::handleCollision(Rectangle &a, Rectangle &b)
{
    // Calculate overlaps
    float aLeft = a.positionX, aRight = a.positionX + PARTICLE_SIZE;
    float bLeft = b.positionX, bRight = b.positionX + PARTICLE_SIZE;
    float overlapX = std::min(aRight, bRight) - std::max(aLeft, bLeft);

    float aTop = a.positionY, aBottom = a.positionY + PARTICLE_SIZE;
    float bTop = b.positionY, bBottom = b.positionY + PARTICLE_SIZE;
    float overlapY = std::min(aBottom, bBottom) - std::max(aTop, bTop);

    if (overlapX <= 0 || overlapY <= 0)
        return;

    // Determine collision axis (smallest overlap)
    if (overlapX < overlapY)
    {
        // X-axis collision
        float aCenterX = a.positionX + PARTICLE_SIZE / 2.0f;
        float bCenterX = b.positionX + PARTICLE_SIZE / 2.0f;
        float direction = (aCenterX < bCenterX) ? -1.0f : 1.0f;

        // Separate particles
        a.positionX -= (overlapX / 2.0f) * direction;
        b.positionX += (overlapX / 2.0f) * direction;

        // Swap X velocities
        std::swap(a.directionX, b.directionX);
    }
    else
    {
        // Y-axis collision
        float aCenterY = a.positionY + PARTICLE_SIZE / 2.0f;
        float bCenterY = b.positionY + PARTICLE_SIZE / 2.0f;
        float direction = (aCenterY < bCenterY) ? -1.0f : 1.0f;

        // Separate particles
        a.positionY -= (overlapY / 2.0f) * direction;
        b.positionY += (overlapY / 2.0f) * direction;

        // Swap Y velocities
        std::swap(a.directionY, b.directionY);
    }
}

void ParticleSimPanel::CalculateFPS()
{
    static int frameCount = 0;
    static auto startTime = std::chrono::steady_clock::now();

    frameCount++;
    auto now = std::chrono::steady_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count();

    if (ms >= 1000)
    {
        fps = frameCount * 1000.0 / ms;
        frameCount = 0;
        startTime = now;
    }
}
