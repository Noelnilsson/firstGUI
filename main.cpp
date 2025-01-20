#include <wx/wx.h>
#include <cmath>
#include "simpleParticle.h"

class App : public wxApp
{
private:
    ParticleSimPanel *simPanel = nullptr; // We'll store a pointer to the panel

public:
    bool OnInit() override
    {
        wxFrame *mainWindow = new wxFrame(nullptr, wxID_ANY, "Windows",
                                          wxDefaultPosition, wxSize(1200, 800));
        mainWindow->SetSizer(new wxBoxSizer(wxVERTICAL));
        CreateStartView(mainWindow);
        mainWindow->Show();
        return true;
    }

    void CreateStartView(wxFrame *window)
    {
        wxBoxSizer *sizer = dynamic_cast<wxBoxSizer *>(window->GetSizer());
        sizer->Clear(true); // Clear any existing content

        wxStaticText *text = new wxStaticText(window, wxID_ANY,
                                              "Welcome User!\nThis project was created by NN",
                                              wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
        text->SetFont(wxFontInfo(30));
        sizer->Add(text, 1, wxALIGN_CENTER | wxALL, 150);

        wxButton *button = new wxButton(window, wxID_ANY, "Continue",
                                        wxDefaultPosition, wxSize(300, 100));
        button->SetFont(wxFontInfo(20));
        sizer->Add(button, 0, wxALIGN_CENTER | wxALL, 120);

        // Bind the button to transition to the new content
        button->Bind(wxEVT_BUTTON, &App::OnGoToModeView, this);

        window->Layout();
    }

    void CreateModeView(wxFrame *window)
    {
        wxBoxSizer *sizer = dynamic_cast<wxBoxSizer *>(window->GetSizer());
        sizer->Clear(true);

        wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
        wxGridSizer *gridSizer = new wxGridSizer(3, 3, 100, 10);

        wxStaticText *text = new wxStaticText(window, wxID_ANY, "Select a mode",
                                              wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
        text->SetFont(wxFontInfo(30));
        mainSizer->Add(text, 1, wxALIGN_CENTER | wxALL, 120);

        const char *mode[3] = {"Particle Sim", "Simple Game", "TBD"};

        for (int i = 1; i <= 9; ++i)
        {
            if (i == 7) // The "Particle Sim" button
            {
                wxButton *buttonParticle = new wxButton(window, wxID_ANY, mode[i - 7]);
                buttonParticle->SetMinSize(wxSize(220, 90));
                buttonParticle->SetFont(wxFontInfo(20));
                gridSizer->Add(buttonParticle, 0, wxALIGN_CENTER | wxALL, 10);
                buttonParticle->Bind(wxEVT_BUTTON, &App::OnGoToParticleSim, this);
            }
            else if (i == 8)
            {
                wxButton *buttonGame = new wxButton(window, wxID_ANY, mode[i - 7]);
                buttonGame->SetMinSize(wxSize(220, 90));
                buttonGame->SetFont(wxFontInfo(20));
                gridSizer->Add(buttonGame, 0, wxALIGN_CENTER | wxALL, 10);
                buttonGame->Bind(wxEVT_BUTTON, &App::OnGoToGame, this);
            }
            else if (i == 9)
            {
                // Placeholder for other modes
                wxButton *button = new wxButton(window, wxID_ANY, mode[i - 7]);
                button->SetMinSize(wxSize(220, 90));
                button->SetFont(wxFontInfo(20));
                gridSizer->Add(button, 0, wxALIGN_CENTER | wxALL, 10);
            }
            else
            {
                wxStaticText *blankText = new wxStaticText(window, wxID_ANY, "");
                gridSizer->Add(blankText, 0, wxALIGN_CENTER | wxALL, 10);
            }
        }

        mainSizer->Add(gridSizer, 1, wxALIGN_CENTER | wxALL, 200);

        wxButton *backButton = new wxButton(window, wxID_ANY, "Back");
        backButton->SetMinSize(wxSize(120, 60));
        backButton->SetFont(wxFontInfo(10));
        mainSizer->Add(backButton, 0, wxALIGN_CENTER | wxBOTTOM, 20);
        backButton->Bind(wxEVT_BUTTON, &App::OnGoToStartView, this);

        sizer->Add(mainSizer, 1, wxEXPAND | wxALL, 0);
        window->Layout();
    }

    void CreateParticleSim(wxFrame *window)
    {
        wxBoxSizer *sizer = dynamic_cast<wxBoxSizer *>(window->GetSizer());
        sizer->Clear(true);

        wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);

        // Top label
        wxStaticText *text = new wxStaticText(window, wxID_ANY, "Particle Simulation",
                                              wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
        text->SetFont(wxFontInfo(20));
        mainSizer->Add(text, 0, wxALIGN_CENTER | wxALL, 20);

        // Middle: create the ParticleSimPanel
        simPanel = new ParticleSimPanel(window);
        mainSizer->Add(simPanel, 1, wxEXPAND | wxALL, 30);

        // Bottom row
        wxGridSizer *gridSizer = new wxGridSizer(1, 3, 0, 250);

        // -- left portion: +/- particle buttons
        wxGridSizer *gridSizer2 = new wxGridSizer(2, 4, 10, 5);

        for (int i = 0; i < 8; i++)
        {
            wxString label;
            int particleValue;

            if (i < 4)
            {
                // +1, +10, +100, +1000
                particleValue = static_cast<int>(std::pow(10, i));
                label = wxString::Format("+%d", particleValue);
            }
            else
            {
                // -1, -10, -100, -1000
                particleValue = -static_cast<int>(std::pow(10, i - 4));
                label = wxString::Format("-%d", std::abs(particleValue));
            }

            wxButton *addButton = new wxButton(window, wxID_ANY, label);
            addButton->SetMinSize(wxSize(70, 30));
            addButton->SetFont(wxFontInfo(10));

            // Bind each button to add or remove particles
            addButton->Bind(wxEVT_BUTTON, [this, particleValue](wxCommandEvent &)
                            { OnAddParticle(particleValue); });

            gridSizer2->Add(addButton, 0, wxALIGN_LEFT | wxALL, 0);
        }

        gridSizer->Add(gridSizer2, 0, wxALIGN_CENTER | wxLEFT, 200);

        // "Stop"/"Start" toggle
        wxButton *toggleButton = new wxButton(window, wxID_ANY, "Stop");
        toggleButton->SetMinSize(wxSize(120, 60));
        toggleButton->SetFont(wxFontInfo(10));
        toggleButton->Bind(wxEVT_BUTTON, &App::OnToggleButton, this);
        gridSizer->Add(toggleButton, 0, wxALIGN_CENTER | wxALL, 10);

        // "Back" button
        wxButton *backButton = new wxButton(window, wxID_ANY, "Back");
        backButton->SetMinSize(wxSize(120, 60));
        backButton->SetFont(wxFontInfo(10));
        backButton->Bind(wxEVT_BUTTON, &App::OnGoToModeView, this);
        gridSizer->Add(backButton, 0, wxALIGN_LEFT | wxALL, 10);

        mainSizer->Add(gridSizer, 0, wxALIGN_CENTER | wxBOTTOM, 20);
        sizer->Add(mainSizer, 1, wxEXPAND | wxALL, 0);

        window->Layout();
    }

    void CreateGame(wxFrame *window)
    {
        wxBoxSizer *sizer = dynamic_cast<wxBoxSizer *>(window->GetSizer());
        sizer->Clear(true);

        wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);

        // Top label
        wxStaticText *text = new wxStaticText(window, wxID_ANY, "Particle Simulation",
                                              wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
        text->SetFont(wxFontInfo(20));
        mainSizer->Add(text, 0, wxALIGN_CENTER | wxALL, 20);
    }

    // Event: Clicking "Particle Sim"
    void OnGoToParticleSim(wxCommandEvent &event)
    {
        wxWindow *btn = dynamic_cast<wxWindow *>(event.GetEventObject());
        wxFrame *window = (btn ? dynamic_cast<wxFrame *>(btn->GetParent()) : nullptr);
        if (window)
            CreateParticleSim(window);
    }

    // Event: Clicking "Continue" on Start
    void OnGoToModeView(wxCommandEvent &event)
    {
        wxWindow *btn = dynamic_cast<wxWindow *>(event.GetEventObject());
        wxFrame *window = (btn ? dynamic_cast<wxFrame *>(btn->GetParent()) : nullptr);
        if (window)
            CreateModeView(window);
    }

    // Event: Clicking "Back"
    void OnGoToStartView(wxCommandEvent &event)
    {
        wxWindow *btn = dynamic_cast<wxWindow *>(event.GetEventObject());
        wxFrame *window = (btn ? dynamic_cast<wxFrame *>(btn->GetParent()) : nullptr);
        if (window)
            CreateStartView(window);
    }

    // Called by the +/- particle buttons
    void OnAddParticle(int particleValue)
    {
        if (simPanel)
            simPanel->addParticle(particleValue);
    }

    // Toggle the simulation
    void OnToggleButton(wxCommandEvent &event)
    {
        wxButton *button = dynamic_cast<wxButton *>(event.GetEventObject());
        if (!button || !simPanel)
            return;

        if (button->GetLabel() == "Stop")
        {
            button->SetLabel("Start");
            simPanel->Stop();
        }
        else
        {
            button->SetLabel("Stop");
            simPanel->Start();
        }
    }

    void OnGoToGame(wxCommandEvent &event)
    {
        wxWindow *btn = dynamic_cast<wxWindow *>(event.GetEventObject());
        wxFrame *window = (btn ? dynamic_cast<wxFrame *>(btn->GetParent()) : nullptr);
        if (window)
            CreateGame(window);
    }
};

wxIMPLEMENT_APP(App);
