#include <wx/wx.h>
#include <wx/timer.h>
#include <vector>
#include <random>
#include <iostream>
#include <chrono>

#define PARTICLE_SIZE 10
#define PARTICLE_SPEED 1.0

class ParticleSimPanel : public wxPanel
{
public:
    ParticleSimPanel(wxWindow *parent) : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize),
                                         frameCount(0), fps(0.0), lastTime(std::chrono::steady_clock::now())
    {
        timer = new wxTimer(this);
        timer->Start(12); // ~60fps

        // Bind events
        Bind(wxEVT_PAINT, &ParticleSimPanel::OnPaint, this);
        Bind(wxEVT_TIMER, &ParticleSimPanel::OnTimer, this);
        Bind(wxEVT_SIZE, &ParticleSimPanel::OnResize, this);
    }

    ~ParticleSimPanel()
    {
        timer->Stop();
        delete timer;
    }

    void OnResize(wxSizeEvent &event)
    {
        if (rectangles.empty()) // Initialize rectangles only once
        {
            wxSize panelSize = GetSize();
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<int> distXY(-500, 500);
            std::uniform_real_distribution<float> distDIR(-PARTICLE_SPEED, PARTICLE_SPEED);

            for (int i = 0; i < 10; ++i)
            {
                float directionX = distDIR(gen);
                float directionY = distDIR(gen);
                if (directionX == 0)
                    directionX = (gen() % 2 == 0) ? 1 : -1;
                if (directionY == 0)
                    directionY = (gen() % 2 == 0) ? 1 : -1;

                rectangles.push_back({panelSize.GetWidth() / 2 + distXY(gen),
                                      panelSize.GetHeight() / 2 + distXY(gen) / 3,
                                      directionX, directionY});
            }
        }

        event.Skip(); // Call the default handler
    }

    void addParticle(int number)
    {

        if (number < 0)
        {
            for (int i = 0; i < -number; i++)
            {
                if (rectangles.size() > 0)
                {
                    rectangles.pop_back();
                }
            }
            return;
        }

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

            rectangles.push_back({panelSize.GetWidth() / 2 + distXY(gen),
                                  panelSize.GetHeight() / 2 + distXY(gen) / 3,
                                  directionX, directionY});
        }
    }

    size_t getParticleCount() const // New method to get particle count
    {
        return rectangles.size();
    }
    void Stop()
    {
        timer->Stop();
    }
    void Start()
    {
        timer->Start(12);
    }

private:
    struct Rectangle
    {
        int positionX;
        int positionY;
        float directionX;
        float directionY;
    };

    std::vector<Rectangle> rectangles;
    wxTimer *timer;
    int frameCount;
    double fps;
    std::chrono::steady_clock::time_point lastTime;

    void OnPaint(wxPaintEvent &event)
    {
        wxPaintDC dc(this);

        dc.SetBackground(*wxWHITE_BRUSH);
        dc.Clear();

        dc.SetBrush(*wxBLUE_BRUSH);
        dc.SetPen(*wxBLACK_PEN);

        int colorConstant = 255 / std::sqrt(2 * PARTICLE_SPEED * PARTICLE_SPEED); // Max speed to scale color

        for (const auto &rect : rectangles)
        {

            float speed = std::sqrt(rect.directionX * rect.directionX + rect.directionY * rect.directionY);
            int colorIntensity = std::min(255, static_cast<int>(colorConstant * speed)); // Scale speed to [0, 255]
            wxColour color(colorIntensity, 0, 255 - colorIntensity);                     // Gradient from red to blue
            dc.SetBrush(wxBrush(color));

            dc.DrawRectangle(rect.positionX, rect.positionY, PARTICLE_SIZE, PARTICLE_SIZE);
        }

        dc.SetFont(wxFont(14, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
        dc.SetBackgroundMode(wxBRUSHSTYLE_SOLID);
        dc.SetTextBackground(*wxLIGHT_GREY);
        dc.SetTextForeground(*wxRED);
        dc.DrawText(wxString::Format("FPS: %.2f", fps), 10, 30);
        dc.DrawText(wxString::Format("Particles: %zu", getParticleCount()), 10, 60);
    }

    void OnTimer(wxTimerEvent &event)
    {
        wxSize panelSize = GetSize();

        for (auto &rect : rectangles)
        {
            rect.positionX += rect.directionX * 5;
            rect.positionY += rect.directionY * 5;

            if (rect.positionX <= 0 || rect.positionX + PARTICLE_SIZE >= panelSize.GetWidth())
                rect.directionX *= -1;

            if (rect.positionY <= 0 || rect.positionY + PARTICLE_SIZE >= panelSize.GetHeight())
                rect.directionY *= -1;
        }

        CalculateFPS();
        Refresh();
    }

    void CalculateFPS()
    {
        frameCount++;

        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTime).count();

        if (elapsed >= 1000)
        {
            fps = frameCount * 1000.0 / elapsed;
            frameCount = 0;
            lastTime = now;
        }
    }
};
