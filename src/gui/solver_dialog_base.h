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
#include <wx/filepicker.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/choice.h>
#include <wx/button.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/collpane.h>
#include <wx/gauge.h>
#include <wx/bmpbuttn.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class SolverAssistant
///////////////////////////////////////////////////////////////////////////////
class SolverAssistant : public wxDialog
{
	private:

	protected:
		wxFilePickerCtrl* m_solver_exe;
		wxChoice* m_solver_choice;
		wxButton* m_solver_search;
		wxStaticLine* m_staticline3;
		wxCollapsiblePane* m_solver_settings_panel;
		wxStaticText* m_solver_settings_ra_t;
		wxTextCtrl* m_solver_settings_ra;
		wxStaticText* m_solver_settings_dec_t;
		wxTextCtrl* m_solver_settings_dec;
		wxStaticText* m_solver_settings_fov_t;
		wxTextCtrl* m_solver_settings_fov;
		wxStaticText* m_solver_settings_downsample_t;
		wxChoice* m_solver_settings_downsample;
		wxStaticLine* m_staticline4;
		wxCollapsiblePane* m_solver_template_panel;
		wxChoice* m_solver_template_choice;
		wxButton* m_solver_template_refresh;
		wxButton* m_solver_template_download;
		wxButton* m_solver_template_open;
		wxGauge* m_download_gauge;
		wxGauge* m_download_check_gauge;
		wxStaticLine* m_staticline5;
		wxButton* m_solver_start;
		wxButton* m_solver_stop;
		wxButton* m_solver_copy;
		wxButton* m_solver_exit;
		wxBitmapButton* m_solver_help;
		wxStaticLine* m_staticline6;
		wxCollapsiblePane* m_solver_result_panel;
		wxStaticText* m_solver_result_ra_t;
		wxStaticText* m_solver_result_ra;
		wxStaticText* m_solver_result_dec_t;
		wxStaticText* m_solver_result_dec;
		wxStaticText* m_solver_result_ratation_t;
		wxStaticText* m_solver_result_ratation;
		wxStaticText* m_solver_result_fovx_t;
		wxStaticText* m_solver_result_fovx;
		wxStaticText* m_solver_result_fovy_t;
		wxStaticText* m_solver_result_fovy;

		// Virtual event handlers, override them in your derived class
		virtual void on_solver_changed( wxFileDirPickerEvent& event ) { event.Skip(); }
		virtual void on_solver_search( wxCommandEvent& event ) { event.Skip(); }
		virtual void refresh_solver_ra( wxMouseEvent& event ) { event.Skip(); }
		virtual void on_ra_enter( wxCommandEvent& event ) { event.Skip(); }
		virtual void refresh_solver_dec( wxMouseEvent& event ) { event.Skip(); }
		virtual void on_dec_enter( wxCommandEvent& event ) { event.Skip(); }
		virtual void on_fov_enter( wxCommandEvent& event ) { event.Skip(); }
		virtual void check_template_downloaded( wxCommandEvent& event ) { event.Skip(); }
		virtual void on_refresh_template( wxCommandEvent& event ) { event.Skip(); }
		virtual void on_download_template( wxCommandEvent& event ) { event.Skip(); }
		virtual void on_open_template_folder( wxCommandEvent& event ) { event.Skip(); }
		virtual void on_start_solver( wxCommandEvent& event ) { event.Skip(); }
		virtual void on_stop_solver( wxCommandEvent& event ) { event.Skip(); }
		virtual void on_copy_solver( wxCommandEvent& event ) { event.Skip(); }
		virtual void on_exit_solver( wxCommandEvent& event ) { event.Skip(); }
		virtual void on_help_solver( wxCommandEvent& event ) { event.Skip(); }
		virtual void on_copy_solver_result_ra( wxMouseEvent& event ) { event.Skip(); }
		virtual void on_copy_solver_result_dec( wxMouseEvent& event ) { event.Skip(); }
		virtual void on_copy_solver_result_rotation( wxMouseEvent& event ) { event.Skip(); }
		virtual void on_copy_solver_result_fovx( wxMouseEvent& event ) { event.Skip(); }
		virtual void on_copy_solver_result_fovy( wxMouseEvent& event ) { event.Skip(); }


	public:

		SolverAssistant( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("SolverAssistant"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );

		~SolverAssistant();

};

