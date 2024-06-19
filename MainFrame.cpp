#include "MainFrame.h"
#include "mathplot.h"
#include <wx/wx.h>
#include <vector>
#include <chrono>
#include <thread>
#include <random>
#include <boost/asio.hpp>
#include "SerialWrapper.h"
#ifdef _WIN32
#include <windows.h>
#elif defined(__linux__)
#include <glob.h>
#include <unistd.h>
#elif defined(__APPLE__)
#include <IOKit/serial/IOSerialKeys.h>
#include <IOKit/IOBSD.h>
#include <IOKit/IOKitLib.h>
#endif

#include "App.h"
wxDECLARE_APP(App);


wxDEFINE_EVENT(wxEVT_SERIAL_DATA, wxCommandEvent);

MainFrame::MainFrame(const wxString& title) : wxFrame(nullptr, wxID_ANY, title), isConnected{ false }, io(), m_timer(io) {

    logger = new wxLogWindow(this, "Serial Data", true, false);
    wxLog::SetActiveTarget(logger);

    CreateControls();
    CreatePlot();
    SetUpSizers();

    CreateMenuBar();
    CreateToolBar();
    CreateStatusBar(3);
    SetStatusText("Catapult GUI", 0);
    SetStatusText("Not Connected", 1);
    SetStatusText("Port: None");

    BindEventHandlers();

    serialDataCallback = [this](const std::string& line) {
        Log(line);
        };
    
    //AddDataPoints();

}
void MainFrame::CreatePlot()
{
    plotWindow = new mpWindow(panel, wxID_ANY, wxDefaultPosition, wxSize(800, 800), wxSUNKEN_BORDER);
    mpScaleX* xaxis = new mpScaleX(wxT("X"), mpALIGN_BORDER_BOTTOM, true, mpX_NORMAL);
    mpScaleY* yaxis = new mpScaleY(wxT("Y"), mpALIGN_LEFT, true);
    plotWindow->AddLayer(xaxis);
    plotWindow->AddLayer(yaxis);

    vectorLayer = new mpFXYVector(wxT("Data"));

    //for (unsigned int i = 0; i < vectorLayer->getRealTimeBufferSize(); i++)
    //{
    //    xValues.push_back(1000 * (360.0 / 2000.0));
    //    yValues.push_back(0);
    //}

    xValues.push_back(0);
    yValues.push_back(1000 * (360.0 / 2000.0));
    
    vectorLayer->SetData(xValues, yValues);
    vectorLayer->SetContinuity(true);
    plotWindow->AddLayer(vectorLayer);
    plotWindow->EnableDoubleBuffer(true);


}
void MainFrame::CreateControls()
{
    panel = new wxPanel(this);
    refreshComPortButton = new wxButton(panel, wxID_ANY, "Refresh");
    comPortSelector = new wxComboBox(panel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize);
    PopulateComPorts();
    connectButton = new wxButton(panel, wxID_ANY, "Connect");
    startLoggingButton = new wxButton(panel, wxID_ANY, "Start Logging");
        saveButton = new wxButton(panel, wxID_ANY, "Save");
}
void MainFrame::SetUpSizers()
{

    mainSizer = new wxBoxSizer(wxVERTICAL);


    buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    
    buttonSizer->Add(refreshComPortButton);
    buttonSizer->Add(comPortSelector);
    buttonSizer->Add(connectButton, wxSizerFlags());
    buttonSizer->Add(startLoggingButton, wxSizerFlags());

    mainSizer->Add(buttonSizer);    
    mainSizer->Add(plotWindow, wxSizerFlags().Proportion(1).Expand());

    panel->SetSizer(mainSizer);
    mainSizer->SetSizeHints(this);

}
void MainFrame::CreateMenuBar()
{    

   
    menuBar = new wxMenuBar;

    // File Menu
    fileMenu = new wxMenu; 
    fileMenu->Append(wxID_NEW, "&New\tCtrl-N");
    fileMenu->Append(wxID_OPEN, "&Open\tCtrl-O");
    fileMenu->Append(wxID_SAVE, "&Save\tCtrl-S");
    fileMenu->Append(wxID_SAVEAS);
    fileMenu->AppendSeparator();
    fileMenu->Append(wxID_EXIT);
    menuBar->Append(fileMenu, "&File");

    // Edit Menu
    editMenu = new wxMenu;
    editMenu->Append(wxID_UNDO, "&Undo\tCtrl-Z");
    editMenu->Append(wxID_REDO, "&Redo\tCtrl-Y");
    editMenu->AppendSeparator();
    editMenu->Append(wxID_CUT, "Cu&t\tCtrl-X");
    editMenu->Append(wxID_COPY, "&Copy\tCtrl-C");
    editMenu->Append(wxID_PASTE, "&Paste\tCtrl-V");
    editMenu->AppendSeparator();
    editMenu->Append(wxID_PREFERENCES, "&Preferences");
    menuBar->Append(editMenu, "&Edit");

    // Connection Menu
    connectionMenu = new wxMenu;
    connectionMenu->Append(wxID_CONNECT , "&Connect\tCtrl-Shift-C");
    connectionMenu->Append(wxID_DISCONNECT, "&Disconnect\tCtrl-Shift-D");
    connectionMenu->AppendSeparator();
    connectionMenu->Append(wxID_CONNECTIONSETTINGS, "Connection &Settings");


    menuBar->Append(connectionMenu, "&Connection");
    // View Menu
    viewMenu = new wxMenu;
    viewMenu->AppendCheckItem(wxID_ANY, "Show &Status bar");
    menuBar->Append(viewMenu, "&View");

    // Data Menu
    dataMenu = new wxMenu;
    dataMenu->Append(wxID_ANY, "&Start Logging\tCtrl-L");
    dataMenu->Append(wxID_ANY, "&Stop Logging\tCtrl-Shift-L");
    dataMenu->AppendSeparator();
    dataMenu->Append(wxID_ANY, "&Clear Data");
    menuBar->Append(dataMenu, "&Data");
/*
    // Tools Menu
    toolsMenu = new wxMenu;
    toolsMenu->Append(wxID_ANY, "&Calibration");
    toolsMenu->Append(wxID_ANY, "Data &Analysis");
    toolsMenu->AppendSeparator();
    toolsMenu->Append(wxID_ANY, "&Export Data");
    */

    // Help Menu
    helpMenu = new wxMenu;

    helpMenu->Append(wxID_ANY, "&Tutorials");
    helpMenu->AppendSeparator();    
    helpMenu->Append(wxID_ABOUT, "&About");
    menuBar->Append(helpMenu, "&Help");



    SetMenuBar(menuBar);
    
}

void MainFrame::CreateToolbar()
{
    toolBar = CreateToolBar(wxTB_HORIZONTAL, wxID_ANY);
    toolBar->AddTool(wxID_NEW, "New", wxNullBitmap, "New Project");

    toolBar->AddTool(wxID_OPEN, "Open", wxNullBitmap, "Open Project");
    toolBar->AddTool(wxID_SAVE, "Save", wxNullBitmap, "Save Project");
    toolBar->AddSeparator();
    toolBar->AddTool(wxID_CONNECT, "Connect", wxNullBitmap, "Connect to Catapult");
    toolBar->AddTool(wxID_DISCONNECT, "Disconnect", wxNullBitmap, "Disconnect from Catapult");
    toolBar->AddSeparator();
    toolBar->AddTool(wxID_STARTLOGGING, "Start Logging", wxNullBitmap, "Start Logging Data");
    toolBar->AddTool(wxID_STOPLOGGING, "Stop Logging", wxNullBitmap, "Stop Logging Data");
    toolBar->Realize();
}

void MainFrame::BindEventHandlers()
{
    refreshComPortButton->Bind(wxEVT_BUTTON, &MainFrame::OnRefreshComPorts, this);
    connectButton->Bind(wxEVT_BUTTON, &MainFrame::OnToggleConnection, this);
    startLoggingButton->Bind(wxEVT_BUTTON, &MainFrame::OnStartLogging, this);
    comPortSelector->Bind(wxEVT_COMBOBOX, &MainFrame::OnComportChoice, this);
    Bind(wxEVT_SERIAL_DATA, &MainFrame::OnSerialData, this);

    // Menu Bar 
    Bind(wxEVT_MENU, &MainFrame::OnNew, this, wxID_NEW);



}

void MainFrame::PopulateComPorts()
{
    comPortSelector->Clear();
#ifdef _WIN32
    for (int i = 1; i <= 256; ++i)
    {
        std::string comPort = "COM" + std::to_string(i);
        try {
            HANDLE hComm = CreateFileA(comPort.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
            if (hComm != INVALID_HANDLE_VALUE)
            {
                comPortSelector->Append(comPort);
                CloseHandle(hComm);
            }
        }
        catch (std::exception& e) {
            std::cerr << "Exception: " << e.what() << std::endl;
        }
    }
#elif defined(__linux__)
    glob_t glob_result;
    glob("/dev/ttyUSB*", GLOB_TILDE, NULL, &glob_result);
    for (unsigned int i = 0; i < glob_result.gl_pathc; ++i)
    {
        comPortSelector->Append(glob_result.gl_pathv[i]);
    }
    globfree(&glob_result);
#elif defined(__APPLE__)
    io_iterator_t serialPortIterator;
    IOServiceGetMatchingServices(kIOMasterPortDefault, IOServiceMatching(kIOSerialBSDServiceValue), &serialPortIterator);
    io_object_t serialPortService;
    while ((serialPortService = IOIteratorNext(serialPortIterator)))
    {
        CFStringRef bsdPathAsCFString = (CFStringRef)IORegistryEntryCreateCFProperty(serialPortService, CFSTR(kIOCalloutDeviceKey), kCFAllocatorDefault, 0);
        if (bsdPathAsCFString)
        {
            char bsdPath[MAXPATHLEN];
            if (CFStringGetCString(bsdPathAsCFString, bsdPath, sizeof(bsdPath), kCFStringEncodingUTF8))
            {
                comPortSelector->Append(bsdPath);
            }
            CFRelease(bsdPathAsCFString);
        }
        IOObjectRelease(serialPortService);
    }
    IOObjectRelease(serialPortIterator);
#endif
}

void MainFrame::OnNew(wxCommandEvent& event)
{
    SetStatusText("Create New File Lol", 1);
}

void MainFrame::OnOpen(wxCommandEvent& event)
{
}

void MainFrame::OnSave(wxCommandEvent& event)
{
}

void MainFrame::OnSaveAs(wxCommandEvent& event)
{
}



void MainFrame::AddDataPoint(wxCommandEvent& event)
{
    m_timer.expires_after(boost::asio::chrono::seconds(1));
    //std::string line = event.GetString().ToStdString();
    std::string line = event.GetString().ToStdString();

    //wxLogMessage("Hello, this is the line %f", line);

    if (line.empty()) return;
    if (line.length() != 13) return;

    std::string delta_t = line.substr(0, 5);
    std::string position = line.substr(6);

    xTime += std::stof(delta_t) / 200;
    int angle = std::stoi(position) * (360.0 / 2000.0);


    vectorLayer->AddData(xTime, angle, xValues, yValues);

    // plotWindow->Fit();
    plotWindow->Fit(xTime-10000, xTime, 0, 400);

}

void MainFrame::OnRefreshComPorts(wxCommandEvent& event)
{
    PopulateComPorts();
}


void MainFrame::ReadSerial()
{

    // TODO: Line error handling with wxMessageBox
    try {
        serialPort->asyncReadLine();    
        io.run();

    }
    catch (const boost::system::system_error& e) {
        wxMessageBox("Please ensure that the serial device is connected. ", "Error", wxICON_ERROR | wxOK);
    }
}

void MainFrame::Log(const std::string& line)
{
    wxLogMessage("%s", line);
    wxCommandEvent event (wxEVT_SERIAL_DATA);
    event.SetString(line);

    wxQueueEvent(this, event.Clone());
    wxYield();
}


void MainFrame::OnToggleConnection(wxCommandEvent& event)
{

    /* Disconnect logic */
    if (serialPort) {

        serialPort->close();
        serialPort.reset(); //Release the unique ptr
        //io_thread.join(); // Wait for io_service thread to finish
        m_timer.cancel();
        connectButton->SetLabel("Connect");
        isConnected = false;
        return;

    }

    try {

        /* Connection logic */
        wxString selectedComPort = comPortSelector->GetValue();

        if (selectedComPort.empty())
        {
            wxMessageBox("Please Select a COM port", "Error", wxOK | wxICON_ERROR);
        }

        io.stop();
        io.reset();

        serialPort = std::make_unique<SerialWrapper>(io, selectedComPort.ToStdString(), 9600);

        serialPort->setCallback([this](const std::string& line) {
            this->Log(line); });

        wxLogMessage("Serial Port opened on comport: %s", selectedComPort);

        isConnected = true;
        connectButton->SetLabel("Disconnect");

    }
    catch (const boost::system::system_error& e) {
        wxMessageBox(wxString::Format("Failed to open COM port: %s,", e.what()), "Error", wxOK | wxICON_ERROR);
    }


}



void MainFrame::OnComportChoice(wxCommandEvent& event)
{
    wxComboBox* comboBox = (wxComboBox*)event.GetEventObject();
    wxString selected = comboBox->GetString(event.GetSelection());
    comport = selected.ToStdString();
    SetStatusText("You selected: " + comport, 1);
}

void MainFrame::OnSerialData(wxCommandEvent& event)
{
    AddDataPoint(event);
}

void MainFrame::OnTimerTrigger()
{
  

    vectorLayer->AddData(xTime + 1000, yValues.back(), xValues, yValues);

    // plotWindow->Fit();
    plotWindow->Fit(xTime - 10000, xTime, 0, 400);
}


void MainFrame::OnStartLogging(wxCommandEvent& event)
{

    m_timer = boost::asio::steady_timer(io, boost::asio::chrono::seconds(1));

    post(io, [this] {
        OnTimerTrigger();
        });

    m_timer.async_wait([this](const boost::system::error_code& ec) {   if (ec) {
        wxMessageBox(wxString::Format("Timer Error: %s", ec.what()), "Error", wxICON_ERROR | wxOK);
    } });
    if (!serialPort || !serialPort->isOpen()) {
        wxMessageBox("Serial port not open", "Error", wxOK | wxICON_ERROR);
        return;
    }

    wxLogMessage("Logging button pressed, logging begins soon...");

    dataThread = std::thread(&MainFrame::ReadSerial, this);
    dataThread.detach();


}


MainFrame::~MainFrame()
{
    io.stop();

    //if (io_thread.joinable()) {
    //    io_thread.join();
    //}

    m_timer.cancel();
    delete plotWindow;

                      
}