#include <wx/wx.h>
#include "simpleParticle.cpp"
#include <cmath>

class App : public wxApp
{
private:
    ParticleSimPanel *simPanel; // Pointer to ParticleSimPanel

public:
    bool OnInit()
    {
        wxFrame *mainWindow = new wxFrame(NULL, wxID_ANY, "Windows", wxDefaultPosition, wxSize(1200, 800));
        mainWindow->SetSizer(new wxBoxSizer(wxVERTICAL)); // Initial sizer
        CreateStartView(mainWindow);
        mainWindow->Show();
        return true;
    }

    // Create the main window content
    void CreateStartView(wxFrame *window)
    {
        wxBoxSizer *sizer = static_cast<wxBoxSizer *>(window->GetSizer());
        sizer->Clear(true); // Clear any existing content

        wxStaticText *text = new wxStaticText(window, wxID_ANY, "Welcome User!\nThis project was created by NN",
                                              wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
        text->SetFont(wxFont(30, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
        sizer->Add(text, 1, wxALIGN_CENTER | wxALL, 150);

        wxButton *button = new wxButton(window, wxID_ANY, "Continue", wxDefaultPosition, wxSize(300, 100));
        button->SetFont(wxFont(20, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
        sizer->Add(button, 0, wxALIGN_CENTER | wxALL, 120);

        // Bind the button to transition to the new content
        button->Bind(wxEVT_BUTTON, &App::OnGoToModeView, this);

        window->Layout(); // Refresh the layout
    }

    // Create the second view's content
    void CreateModeView(wxFrame *window)
    {
        wxBoxSizer *sizer = static_cast<wxBoxSizer *>(window->GetSizer());
        sizer->Clear(true); // Clear the current content

        // Sizers
        wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
        wxGridSizer *gridSizer = new wxGridSizer(3, 3, 100, 10);

        wxStaticText *text = new wxStaticText(window, wxID_ANY, "Select a mode",
                                              wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
        text->SetFont(wxFont(30, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
        mainSizer->Add(text, 1, wxALIGN_CENTER | wxALL, 120);

        const char *mode[3] = {"Particle Sim", "TBD", "TBD"};

        for (int i = 1; i <= 9; ++i)
        {

            if (i == 7)
            {
                wxButton *buttonParticle = new wxButton(window, wxID_ANY, mode[i - 7]);
                buttonParticle->SetMinSize(wxSize(220, 90));
                buttonParticle->SetFont(wxFont(20, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
                gridSizer->Add(buttonParticle, 0, wxALIGN_CENTER | wxALL, 10);
                buttonParticle->Bind(wxEVT_BUTTON, &App::OnGoToParticleSim, this);
            }
            else if (i == 8 || i == 9)
            { // Placeholder for now
                wxButton *button = new wxButton(window, wxID_ANY, mode[i - 7]);
                button->SetMinSize(wxSize(220, 90));
                button->SetFont(wxFont(20, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
                gridSizer->Add(button, 0, wxALIGN_CENTER | wxALL, 10);
            }
            else
            {
                wxStaticText *text = new wxStaticText(window, wxID_ANY, "");
                gridSizer->Add(text, 0, wxALIGN_CENTER | wxALL, 10);
            }
        }

        // Add the grid sizer to the main sizer
        mainSizer->Add(gridSizer, 1, wxALIGN_CENTER | wxALL, 200);

        wxButton *backButton = new wxButton(window, wxID_ANY, "Back");
        backButton->SetMinSize(wxSize(120, 60));
        backButton->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
        mainSizer->Add(backButton, 0, wxALIGN_CENTER | wxBOTTOM, 20);
        backButton->Bind(wxEVT_BUTTON, &App::OnGoToStartView, this);

        // Set the new sizer for the window
        sizer->Add(mainSizer, 1, wxEXPAND | wxALL, 0);
        window->Layout(); // Refresh
    }

    void CreateParticleSim(wxFrame *window)
    {
        wxBoxSizer *sizer = static_cast<wxBoxSizer *>(window->GetSizer());
        sizer->Clear(true);

        wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);

        // TOP
        wxStaticText *text = new wxStaticText(window, wxID_ANY, "Particle Simulation",
                                              wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
        text->SetFont(wxFont(20, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
        mainSizer->Add(text, 0, wxALIGN_CENTER | wxALL, 20);

        // MIDDLE
        // Use *this->*simPanel (the member variable) instead of a new local:
        simPanel = new ParticleSimPanel(window);
        mainSizer->Add(simPanel, 1, wxEXPAND | wxALL, 30);

        // BOTTOM
        wxGridSizer *gridSizer = new wxGridSizer(1, 3, 0, 250);

        // BUTTOM LEFT

        wxGridSizer *gridSizer2 = new wxGridSizer(2, 4, 10, 5);

        for (int i = 0; i < 8; i++)
        {
            wxString label;
            int particleValue;

            if (i < 4)
            {
                particleValue = static_cast<int>(std::pow(10, i));
                label = wxString::Format("+%d", particleValue);
            }
            else
            {
                particleValue = -static_cast<int>(std::pow(10, i - 4));
                label = wxString::Format("-%d", particleValue);
            }

            wxButton *addButton = new wxButton(window, wxID_ANY, label);
            addButton->SetMinSize(wxSize(70, 30));
            addButton->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

            addButton->Bind(wxEVT_BUTTON, [this, particleValue](wxCommandEvent &event)
                            { OnAddParticle(particleValue); });

            gridSizer2->Add(addButton, 0, wxALIGN_LEFT | wxALL, 0);
        }

        gridSizer->Add(gridSizer2, 0, wxALIGN_CENTER | wxLEFT, 200);

        wxButton *toggleButton = new wxButton(window, wxID_ANY, "Stop");
        toggleButton->SetMinSize(wxSize(120, 60));
        toggleButton->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
        toggleButton->Bind(wxEVT_BUTTON, &App::OnToggleButton, this);
        gridSizer->Add(toggleButton, 0, wxALIGN_LEFT | wxALL, 10);

        wxButton *backButton = new wxButton(window, wxID_ANY, "Back");
        backButton->SetMinSize(wxSize(120, 60));
        backButton->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
        backButton->Bind(wxEVT_BUTTON, &App::OnGoToModeView, this);
        gridSizer->Add(backButton, 0, wxALIGN_LEFT | wxALL, 10);

        mainSizer->Add(gridSizer, 0, wxALIGN_CENTER | wxBOTTOM, 20);

        sizer->Add(mainSizer, 1, wxEXPAND | wxALL, 0);
        window->Layout();
    }

    void OnGoToParticleSim(wxCommandEvent &event)
    {
        wxWindow *button = static_cast<wxWindow *>(event.GetEventObject());
        wxFrame *window = dynamic_cast<wxFrame *>(button->GetParent());
        if (window)
        {
            CreateParticleSim(window);
        }
    }

    // Event handler to go to the second view
    void OnGoToModeView(wxCommandEvent &event)
    {
        wxWindow *button = static_cast<wxWindow *>(event.GetEventObject());
        wxFrame *window = dynamic_cast<wxFrame *>(button->GetParent());
        if (window)
        {
            CreateModeView(window);
        }
    }

    // Event handler to go back to the main view
    void OnGoToStartView(wxCommandEvent &event)
    {
        wxWindow *button = static_cast<wxWindow *>(event.GetEventObject());
        wxFrame *window = dynamic_cast<wxFrame *>(button->GetParent());
        if (window)
        {
            CreateStartView(window);
        }
    }

    void OnAddParticle(int particleValue)
    {
        if (simPanel)
        {
            simPanel->addParticle(particleValue); // Add the specified number of particles
        }
    }

    void OnToggleButton(wxCommandEvent &event)
    {
        wxButton *button = static_cast<wxButton *>(event.GetEventObject());
        if (button)
        {
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
    }
};

wxIMPLEMENT_APP(App);
