///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 3.10.1-0-g8feb16b3)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/bitmap.h>
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/collpane.h>
#include <wx/colour.h>
#include <wx/dialog.h>
#include <wx/filepicker.h>
#include <wx/font.h>
#include <wx/gdicmn.h>
#include <wx/icon.h>
#include <wx/image.h>
#include <wx/intl.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/xrc/xmlres.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class ServerDialog
///////////////////////////////////////////////////////////////////////////////
class ServerDialog : public wxDialog {
private:
protected:
    wxStaticText* m_server_title;
    wxStaticText* m_server_host_t;
    wxTextCtrl* m_server_host;
    wxStaticText* m_server_port_t;
    wxTextCtrl* m_server_port;
    wxButton* m_server_start;
    wxButton* m_server_stop;
    wxButton* m_server_config;
    wxButton* m_server_open;
    wxStaticLine* m_staticline1;
    wxStaticText* m_webserver_title;
    wxStaticText* m_webserver_host_t;
    wxTextCtrl* m_webserver_host;
    wxStaticText* m_webserver_port_t;
    wxTextCtrl* m_webserver_port;
    wxStaticText* m_webserver_ssl_t;
    wxCheckBox* m_server_ssl;
    wxStaticText* m_webserver_mod_t;
    wxCheckBox* m_webserver_mod;
    wxCollapsiblePane* m_server_ssl_settings_panel;
    wxStaticText* m_server_ssl_key_t;
    wxFilePickerCtrl* m_server_ssl_key;
    wxStaticText* m_server_ssl_cert_t;
    wxFilePickerCtrl* m_server_ssl_cert;
    wxButton* m_webserver_start;
    wxButton* m_webserver_stop;
    wxStaticLine* m_staticline2;

    // Virtual event handlers, override them in your derived class
    virtual void start_server(wxCommandEvent& event) { event.Skip(); }
    virtual void stop_server(wxCommandEvent& event) { event.Skip(); }
    virtual void config_server(wxCommandEvent& event) { event.Skip(); }
    virtual void open_server(wxCommandEvent& event) { event.Skip(); }
    virtual void start_webserver(wxCommandEvent& event) { event.Skip(); }
    virtual void stop_webserver(wxCommandEvent& event) { event.Skip(); }

public:
    ServerDialog(wxWindow* parent, wxWindowID id = wxID_ANY,
                 const wxString& title = _("ServerDialog"),
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxDEFAULT_DIALOG_STYLE);

    ~ServerDialog();
};
