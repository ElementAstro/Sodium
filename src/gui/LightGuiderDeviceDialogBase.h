///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 3.10.1-0-g8feb16b3)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/choice.h>
#include <wx/bmpbuttn.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/gbsizer.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/panel.h>
#include <wx/slider.h>
#include <wx/combobox.h>
#include <wx/grid.h>
#include <wx/listbook.h>
#include <wx/listctrl.h>
#include <wx/srchctrl.h>
#include <wx/dialog.h>
#include <wx/aui/aui.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class DeviceDialog
///////////////////////////////////////////////////////////////////////////////
class DeviceDialog : public wxDialog
{
	private:

	protected:
		wxListbook* m_main_listbook;
		wxPanel* m_camera_panel;
		wxStaticText* m_camera_title;
		wxChoice* m_camera_choice;
		wxBitmapButton* m_camera_settings_btn;
		wxBitmapButton* m_camera_connect_btn;
		wxBitmapButton* m_camera_scan_btn;
		wxStaticLine* m_staticline11;
		wxStaticText* m_camera_name_t;
		wxStaticText* m_camera_name;
		wxStaticText* m_camera_description_t;
		wxStaticText* m_camera_description;
		wxStaticLine* m_staticline12;
		wxStaticText* m_camera_gain_t;
		wxTextCtrl* m_camera_gain;
		wxStaticText* m_camera_offset_t;
		wxTextCtrl* m_camera_offset;
		wxStaticText* m_camera_bin_t;
		wxChoice* m_camera_bin;
		wxStaticText* m_camera_cooling_t;
		wxCheckBox* m_camera_cooling;
		wxStaticText* m_camera_temperature_t;
		wxTextCtrl* m_camera_temperature;
		wxStaticText* m_camera_power_t;
		wxTextCtrl* m_camera_power;
		wxStaticText* m_camera_driverversion_t;
		wxStaticText* m_camera_driverversion;
		wxStaticText* m_camera_sensor_t;
		wxStaticText* m_camera_sensor;
		wxStaticText* m_camera_height_t;
		wxStaticText* m_camera_height;
		wxStaticText* m_camera_width_t;
		wxStaticText* m_camera_width;
		wxStaticText* m_camera_pixelx_t;
		wxStaticText* m_camera_pixelx;
		wxStaticText* m_camera_pixely_t;
		wxStaticText* m_camera_pixely;
		wxStaticText* m_camera_maxbinx_t;
		wxStaticText* m_camera_maxbinx;
		wxStaticText* m_camera_biny_t;
		wxStaticText* m_camera_biny;
		wxStaticText* m_camera_maxexposure_t;
		wxStaticText* m_camera_maxexposure;
		wxStaticText* m_camera_minexposure_t;
		wxStaticText* m_camera_minexposure;
		wxStaticLine* m_staticline13;
		wxStaticText* m_camera_temperature_title;
		wxPanel* m_panel7;
		wxPanel* m_telescope_panel;
		wxStaticText* m_staticText321;
		wxChoice* m_choice41;
		wxBitmapButton* m_bpButton21;
		wxBitmapButton* m_bpButton31;
		wxBitmapButton* m_bpButton41;
		wxStaticLine* m_staticline111;
		wxStaticText* m_staticText711;
		wxStaticText* m_staticText721;
		wxStaticText* m_staticText691;
		wxStaticText* m_staticText701;
		wxStaticLine* m_staticline121;
		wxStaticText* m_staticText160;
		wxStaticText* m_staticText161;
		wxStaticText* m_staticText162;
		wxStaticText* m_staticText163;
		wxStaticText* m_staticText164;
		wxStaticText* m_staticText165;
		wxStaticText* m_staticText166;
		wxStaticText* m_staticText167;
		wxStaticText* m_staticText168;
		wxStaticText* m_staticText169;
		wxStaticText* m_staticText170;
		wxStaticText* m_staticText171;
		wxStaticText* m_staticText172;
		wxStaticText* m_staticText173;
		wxStaticText* m_staticText174;
		wxStaticText* m_staticText175;
		wxStaticText* m_staticText176;
		wxStaticText* m_staticText177;
		wxStaticText* m_staticText178;
		wxStaticText* m_staticText179;
		wxStaticLine* m_staticline24;
		wxButton* m_button25;
		wxButton* m_button26;
		wxButton* m_button27;
		wxButton* m_button28;
		wxButton* m_button29;
		wxButton* m_button30;
		wxStaticLine* m_staticline26;
		wxStaticText* m_staticText188;
		wxStaticLine* m_staticline21;
		wxStaticText* m_staticText189;
		wxTextCtrl* m_textCtrl47;
		wxStaticText* m_staticText190;
		wxTextCtrl* m_textCtrl48;
		wxStaticText* m_staticText191;
		wxTextCtrl* m_textCtrl49;
		wxStaticText* m_staticText192;
		wxStaticText* m_staticText193;
		wxTextCtrl* m_textCtrl50;
		wxStaticText* m_staticText194;
		wxTextCtrl* m_textCtrl51;
		wxStaticText* m_staticText195;
		wxTextCtrl* m_textCtrl52;
		wxStaticText* m_staticText196;
		wxButton* m_button23;
		wxButton* m_button24;
		wxStaticLine* m_staticline22;
		wxBitmapButton* m_bpButton28;
		wxBitmapButton* m_bpButton29;
		wxBitmapButton* m_bpButton30;
		wxBitmapButton* m_bpButton311;
		wxBitmapButton* m_bpButton32;
		wxBitmapButton* m_bpButton33;
		wxBitmapButton* m_bpButton34;
		wxBitmapButton* m_bpButton35;
		wxBitmapButton* m_bpButton36;
		wxStaticText* m_staticText197;
		wxSlider* m_slider2;
		wxStaticText* m_staticText198;
		wxSlider* m_slider3;
		wxStaticLine* m_staticline23;
		wxPanel* m_focuser_panel;
		wxStaticText* m_staticText201;
		wxChoice* m_choice11;
		wxBitmapButton* m_bpButton37;
		wxBitmapButton* m_bpButton38;
		wxBitmapButton* m_bpButton39;
		wxStaticLine* m_staticline25;
		wxStaticText* m_staticText202;
		wxStaticText* m_staticText203;
		wxStaticText* m_staticText204;
		wxStaticText* m_staticText205;
		wxStaticText* m_staticText206;
		wxStaticText* m_staticText207;
		wxStaticText* m_staticText208;
		wxStaticText* m_staticText209;
		wxStaticText* m_staticText210;
		wxStaticText* m_staticText211;
		wxStaticText* m_staticText212;
		wxStaticText* m_staticText213;
		wxStaticText* m_staticText214;
		wxStaticText* m_staticText215;
		wxStaticText* m_staticText216;
		wxStaticText* m_staticText217;
		wxStaticLine* m_staticline28;
		wxStaticText* m_staticText223;
		wxCheckBox* m_checkBox6;
		wxStaticLine* m_staticline29;
		wxStaticText* m_staticText224;
		wxTextCtrl* m_textCtrl59;
		wxButton* m_button31;
		wxButton* m_button32;
		wxButton* m_button33;
		wxButton* m_button34;
		wxStaticText* m_staticText218;
		wxStaticLine* m_staticline27;
		wxStaticText* m_staticText219;
		wxTextCtrl* m_textCtrl55;
		wxStaticText* m_staticText220;
		wxTextCtrl* m_textCtrl56;
		wxStaticText* m_staticText221;
		wxTextCtrl* m_textCtrl57;
		wxStaticText* m_staticText222;
		wxTextCtrl* m_textCtrl58;
		wxPanel* m_filterwheel_panel;
		wxStaticText* m_staticText3211;
		wxChoice* m_choice411;
		wxBitmapButton* m_bpButton211;
		wxBitmapButton* m_bpButton312;
		wxBitmapButton* m_bpButton411;
		wxStaticLine* m_staticline1111;
		wxStaticText* m_staticText7111;
		wxStaticText* m_staticText7211;
		wxStaticText* m_staticText6911;
		wxStaticText* m_staticText7011;
		wxStaticLine* m_staticline1211;
		wxStaticText* m_staticText297;
		wxStaticText* m_staticText298;
		wxStaticText* m_staticText299;
		wxStaticText* m_staticText300;
		wxStaticLine* m_staticline261;
		wxStaticText* m_staticText306;
		wxComboBox* m_comboBox1;
		wxButton* m_button51;
		wxGrid* m_grid3;
		wxPanel* m_guider_panel;
		wxPanel* m_plugin_panel;
		wxListbook* m_listbook3;
		wxPanel* m_plugin_installed_panel;
		wxListbook* m_listbook4;
		wxPanel* m_pulgin_panel_openapt;
		wxPanel* m_plugin_download_panel;
		wxPanel* m_plugin_local_panel;
		wxStaticText* m_staticText307;
		wxStaticLine* m_staticline44;
		wxButton* m_button52;
		wxSearchCtrl* m_searchCtrl1;
		wxButton* m_button53;
		wxListbook* m_listbook5;
		wxPanel* m_server_panel;
		wxPanel* m_settings_panel;

		// Virtual event handlers, override them in your derived class
		virtual void on_camera_settings_click( wxCommandEvent& event ) { event.Skip(); }
		virtual void on_camera_connect_click( wxCommandEvent& event ) { event.Skip(); }
		virtual void on_camera_scan_click( wxCommandEvent& event ) { event.Skip(); }


	public:

		DeviceDialog();
		DeviceDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("OpenAPT Client"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
		bool Create( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("OpenAPT Client"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
		wxAuiManager m_mgr;

		~DeviceDialog();

};

