/////////////////////////////////////////////////////////////////////////////
// Name:        led.h
// Purpose:     wxLed class
// Author:      Thomas Monjalon
// Created:     09/06/2005
// Revision:    09/06/2005
// Licence:     wxWidgets
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_LED_H_
#define _WX_LED_H_

#include <wx/bitmap.h>
#include <wx/dcclient.h>
#include <wx/thread.h>
#include <wx/window.h>

class WXDLLEXPORT wxLed : public wxWindow {
public:
    wxLed(wxWindow* parent, wxWindowID id, const char* disabledColor = "000000",
          const wxPoint& pos = wxDefaultPosition,
          const wxSize& size = wxDefaultSize);
    ~wxLed();

    bool Enable(bool enable = true);
    void SetColor(const char* rgb);

protected:
    char m_enabledColor[7];
    char m_disabledColor[7];
    char m_color[7];
    wxBitmap* m_bitmap;
    wxMutex m_mutex;

    void OnPaint(wxPaintEvent& event);
    virtual void SetBitmap(const char* color);

private:
    wxDECLARE_EVENT_TABLE();
};

#endif  // _WX_LED_H_
