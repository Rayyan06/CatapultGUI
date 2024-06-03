#pragma once
#include <wx/wx.h>
#include "mathplot.h"
class MainFrame : public wxFrame
{
public:
	MainFrame(const wxString& title);
	~MainFrame();
private:
	wxPanel* panel;
	wxButton* readButton;
	wxTextCtrl* dataLine;

	mpWindow* plotWindow;

	wxMenuBar* menuBar;
	void CreateMenuBar();
	void CreatePlot();
	void BindEventHandlers();


};


