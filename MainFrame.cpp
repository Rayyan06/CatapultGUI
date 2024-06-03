#include "MainFrame.h"
#include "mathplot.h"
#include <wx/wx.h>
#include <vector>


MainFrame::MainFrame(const wxString& title) : wxFrame(nullptr, wxID_ANY, title) {

    panel = new wxPanel(this);
    

    CreatePlot();

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(plotWindow, wxSizerFlags().Expand());

    panel->SetSizer(mainSizer);
    mainSizer->SetSizeHints(this);

    CreateMenuBar();
    CreateStatusBar();
    SetStatusText("Catapult GUI");
    BindEventHandlers();

}
void MainFrame::CreatePlot()
{
    plotWindow = new mpWindow(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER);
    mpScaleX* xaxis = new mpScaleX(wxT("X"), mpALIGN_BOTTOM, true, mpX_NORMAL);
    mpScaleY* yaxis = new mpScaleY(wxT("Y"), mpALIGN_LEFT, true);
    plotWindow->AddLayer(xaxis);
    plotWindow->AddLayer(yaxis);

    mpFXYVector* vectorLayer = new mpFXYVector(wxT("Data"));
    std::vector<double> xValues, yValues;
    for (int i = 0; i < 100; ++i)
    {
        xValues.push_back(i);
        yValues.push_back(sin(i * 0.1));
    }
    vectorLayer->SetData(xValues, yValues);
    vectorLayer->SetContinuity(true);
    plotWindow->AddLayer(vectorLayer);
}
void MainFrame::CreateMenuBar()
{
    wxMenu* fileMenu = new wxMenu;
    fileMenu->Append(wxID_EXIT);

    wxMenu* helpMenu = new wxMenu;
    helpMenu->Append(wxID_ABOUT);

    menuBar = new wxMenuBar;
    menuBar->Append(fileMenu, "&File");
    menuBar->Append(helpMenu, "&Help");

    SetMenuBar(menuBar);
    
}

void MainFrame::BindEventHandlers()
{

}


MainFrame::~MainFrame()
{
}