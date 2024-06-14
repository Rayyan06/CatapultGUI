#pragma once
#include <wx/wx.h>
#include <thread>
#include <vector>
#include "mathplot.h"
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include "SerialWrapper.h"
#define ever ;;

enum {
	wxID_CONNECT = wxID_HIGHEST + 1,
	wxID_DISCONNECT,
	wxID_CONNECTIONSETTINGS,
	wxID_STARTLOGGING,
	wxID_STOPLOGGING

};

class MainFrame : public wxFrame
{
public:
	MainFrame(const wxString& title);
	~MainFrame();
private:
	wxPanel* panel;

	wxButton* refreshComPortButton;
	wxComboBox* comPortSelector;
	wxButton* connectButton;
	wxButton* startLoggingButton;
	wxButton* saveButton;
	wxTextCtrl* dataLine;

	/* Sizers */
	wxBoxSizer* mainSizer;
	wxBoxSizer* buttonSizer;

	mpWindow* plotWindow;
	mpFXYVector* vectorLayer;
	std::vector<double> xValues, yValues;
	double xTime = 0;

	std::thread dataThread;

		/*
	Serial Port stuff
	*/
	std::string comport; // The name of the port

	bool isConnected;

	boost::asio::io_service io;
	std::unique_ptr<SerialWrapper> serialPort;

	/* Buffer to read into */
	boost::asio::streambuf read_buf;

	wxLog* logger;

	// Menu Bar
	wxMenuBar* menuBar;
	wxMenu* fileMenu;
	wxMenu* editMenu;
	wxMenu* connectionMenu;
	wxMenu* viewMenu;
	wxMenu* dataMenu;
	wxMenu* toolsMenu;
	wxMenu* helpMenu;

	// Toolbar
	wxToolBar* toolBar;

	void CreateMenuBar();
	void CreateToolbar();
	void CreatePlot();
	void CreateControls();
	void SetUpSizers();
	void BindEventHandlers();
	void PopulateComPorts();

	// Event Handlers
	void OnNew(wxCommandEvent& event);
	void OnOpen(wxCommandEvent& event);
	void OnSave(wxCommandEvent& event);
	void OnSaveAs(wxCommandEvent& event);
	void OnExit(wxCommandEvent& event);

	void OnUndo(wxCommandEvent& event);
	void OnRedo(wxCommandEvent& event);
	void OnCut(wxCommandEvent& event);
	void OnCopy(wxCommandEvent& event);
	void OnPaste(wxCommandEvent& event);
	void OnPreferences(wxCommandEvent& event);

	void OnConnect(wxCommandEvent& event);
	void OnDisconnect(wxCommandEvent& event);
	void OnConnectionSettings(wxCommandEvent& event);

	void OnShowToolbar(wxCommandEvent& event);
	void OnShowStatusBar(wxCommandEvent& event);
	void OnDataPlots(wxCommandEvent& event);
	void OnFullScreen(wxCommandEvent& event);

	void OnStartLogging(wxCommandEvent& event);
	void OnStopLogging(wxCommandEvent& event);
	void OnClearData(wxCommandEvent& event);
	void OnAnalyzeData(wxCommandEvent& event);

	void OnCalibration(wxCommandEvent& event);
	void OnDataAnalysis(wxCommandEvent& event);

	void OnUserGuide(wxCommandEvent& event);
	void OnTutorials(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);

	void AddDataPoint(wxCommandEvent& event);
	void OnRefreshComPorts(wxCommandEvent& event);
	void StartSerial(); 
	void ReadSerial();
	void Log(const std::string& line);
	void ReadHandler(const boost::system::error_code& error, size_t bytes_transferred);
	void OnComportChoice(wxCommandEvent& event);
	void OnSerialData(wxCommandEvent& event);

};


