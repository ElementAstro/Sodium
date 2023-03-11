///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 3.10.1-0-g8feb16b3)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "lightguider.h"
#include "solver_dialog_base.h"

///////////////////////////////////////////////////////////////////////////

SolverAssistant::SolverAssistant( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* m_dialog_box;
	m_dialog_box = new wxBoxSizer( wxVERTICAL );

	wxFlexGridSizer* m_dialog_sizer;
	m_dialog_sizer = new wxFlexGridSizer( 0, 1, 0, 0 );
	m_dialog_sizer->SetFlexibleDirection( wxBOTH );
	m_dialog_sizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	wxFlexGridSizer* m_solver_solver_sizer;
	m_solver_solver_sizer = new wxFlexGridSizer( 0, 4, 0, 0 );
	m_solver_solver_sizer->SetFlexibleDirection( wxBOTH );
	m_solver_solver_sizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_solver_exe = new wxFilePickerCtrl( this, wxID_ANY, wxEmptyString, _("Select a file"), _("*.*"), wxDefaultPosition, wxDefaultSize, wxFLP_DEFAULT_STYLE );
	m_solver_solver_sizer->Add( m_solver_exe, 0, wxALL, 5 );

	wxString m_solver_choiceChoices[] = { _("Astrometry"), _("Astap") };
	int m_solver_choiceNChoices = sizeof( m_solver_choiceChoices ) / sizeof( wxString );
	m_solver_choice = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_solver_choiceNChoices, m_solver_choiceChoices, 0 );
	m_solver_choice->SetSelection( 1 );
	m_solver_solver_sizer->Add( m_solver_choice, 0, wxALL, 5 );

	m_solver_search = new wxButton( this, wxID_ANY, _("Search"), wxDefaultPosition, wxDefaultSize, 0 );
	m_solver_solver_sizer->Add( m_solver_search, 0, wxALL, 5 );


	m_dialog_sizer->Add( m_solver_solver_sizer, 1, wxEXPAND, 5 );

	m_staticline3 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	m_dialog_sizer->Add( m_staticline3, 0, wxEXPAND | wxALL, 5 );

	m_solver_settings_panel = new wxCollapsiblePane( this, wxID_ANY, _("Solver Settings"), wxDefaultPosition, wxDefaultSize, wxCP_DEFAULT_STYLE );
	m_solver_settings_panel->Collapse( false );

	wxBoxSizer* m_solver_settings_sizer;
	m_solver_settings_sizer = new wxBoxSizer( wxVERTICAL );

	wxFlexGridSizer* m_solver_settings_sizer_2;
	m_solver_settings_sizer_2 = new wxFlexGridSizer( 0, 4, 0, 0 );
	m_solver_settings_sizer_2->SetFlexibleDirection( wxBOTH );
	m_solver_settings_sizer_2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_solver_settings_ra_t = new wxStaticText( m_solver_settings_panel->GetPane(), wxID_ANY, _("RA"), wxDefaultPosition, wxDefaultSize, 0 );
	m_solver_settings_ra_t->Wrap( -1 );
	m_solver_settings_sizer_2->Add( m_solver_settings_ra_t, 0, wxALL, 5 );

	m_solver_settings_ra = new wxTextCtrl( m_solver_settings_panel->GetPane(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	#ifdef __WXGTK__
	if ( !m_solver_settings_ra->HasFlag( wxTE_MULTILINE ) )
	{
	m_solver_settings_ra->SetMaxLength( 10 );
	}
	#else
	m_solver_settings_ra->SetMaxLength( 10 );
	#endif
	m_solver_settings_sizer_2->Add( m_solver_settings_ra, 0, wxALL, 5 );

	m_solver_settings_dec_t = new wxStaticText( m_solver_settings_panel->GetPane(), wxID_ANY, _("DEC"), wxDefaultPosition, wxDefaultSize, 0 );
	m_solver_settings_dec_t->Wrap( -1 );
	m_solver_settings_sizer_2->Add( m_solver_settings_dec_t, 0, wxALL, 5 );

	m_solver_settings_dec = new wxTextCtrl( m_solver_settings_panel->GetPane(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	#ifdef __WXGTK__
	if ( !m_solver_settings_dec->HasFlag( wxTE_MULTILINE ) )
	{
	m_solver_settings_dec->SetMaxLength( 10 );
	}
	#else
	m_solver_settings_dec->SetMaxLength( 10 );
	#endif
	m_solver_settings_sizer_2->Add( m_solver_settings_dec, 0, wxALL, 5 );

	m_solver_settings_fov_t = new wxStaticText( m_solver_settings_panel->GetPane(), wxID_ANY, _("Fov"), wxDefaultPosition, wxDefaultSize, 0 );
	m_solver_settings_fov_t->Wrap( -1 );
	m_solver_settings_sizer_2->Add( m_solver_settings_fov_t, 0, wxALL, 5 );

	m_solver_settings_fov = new wxTextCtrl( m_solver_settings_panel->GetPane(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	#ifdef __WXGTK__
	if ( !m_solver_settings_fov->HasFlag( wxTE_MULTILINE ) )
	{
	m_solver_settings_fov->SetMaxLength( 5 );
	}
	#else
	m_solver_settings_fov->SetMaxLength( 5 );
	#endif
	m_solver_settings_sizer_2->Add( m_solver_settings_fov, 0, wxALL, 5 );

	m_solver_settings_downsample_t = new wxStaticText( m_solver_settings_panel->GetPane(), wxID_ANY, _("Downsample"), wxDefaultPosition, wxDefaultSize, 0 );
	m_solver_settings_downsample_t->Wrap( -1 );
	m_solver_settings_sizer_2->Add( m_solver_settings_downsample_t, 0, wxALL, 5 );

	wxString m_solver_settings_downsampleChoices[] = { _("Auto"), _("1"), _("2"), _("4") };
	int m_solver_settings_downsampleNChoices = sizeof( m_solver_settings_downsampleChoices ) / sizeof( wxString );
	m_solver_settings_downsample = new wxChoice( m_solver_settings_panel->GetPane(), wxID_ANY, wxDefaultPosition, wxDefaultSize, m_solver_settings_downsampleNChoices, m_solver_settings_downsampleChoices, 0 );
	m_solver_settings_downsample->SetSelection( 0 );
	m_solver_settings_sizer_2->Add( m_solver_settings_downsample, 0, wxALL, 5 );


	m_solver_settings_sizer->Add( m_solver_settings_sizer_2, 1, wxEXPAND, 5 );


	m_solver_settings_panel->GetPane()->SetSizer( m_solver_settings_sizer );
	m_solver_settings_panel->GetPane()->Layout();
	m_solver_settings_sizer->Fit( m_solver_settings_panel->GetPane() );
	m_dialog_sizer->Add( m_solver_settings_panel, 1, wxEXPAND | wxALL, 5 );

	m_staticline4 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	m_dialog_sizer->Add( m_staticline4, 0, wxEXPAND | wxALL, 5 );

	m_solver_template_panel = new wxCollapsiblePane( this, wxID_ANY, _("Template Download"), wxDefaultPosition, wxDefaultSize, wxCP_DEFAULT_STYLE );
	m_solver_template_panel->Collapse( false );

	wxBoxSizer* m_solver_template_sizer;
	m_solver_template_sizer = new wxBoxSizer( wxVERTICAL );

	wxFlexGridSizer* fgSizer11;
	fgSizer11 = new wxFlexGridSizer( 0, 4, 0, 0 );
	fgSizer11->SetFlexibleDirection( wxBOTH );
	fgSizer11->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	wxArrayString m_solver_template_choiceChoices;
	m_solver_template_choice = new wxChoice( m_solver_template_panel->GetPane(), wxID_ANY, wxDefaultPosition, wxDefaultSize, m_solver_template_choiceChoices, 0 );
	m_solver_template_choice->SetSelection( 0 );
	fgSizer11->Add( m_solver_template_choice, 0, wxALL, 5 );

	m_solver_template_refresh = new wxButton( m_solver_template_panel->GetPane(), wxID_ANY, _("Refresh"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer11->Add( m_solver_template_refresh, 0, wxALL, 5 );

	m_solver_template_download = new wxButton( m_solver_template_panel->GetPane(), wxID_ANY, _("Download"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer11->Add( m_solver_template_download, 0, wxALL, 5 );

	m_solver_template_open = new wxButton( m_solver_template_panel->GetPane(), wxID_ANY, _("Open"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer11->Add( m_solver_template_open, 0, wxALL, 5 );


	m_solver_template_sizer->Add( fgSizer11, 1, wxEXPAND, 5 );

	wxFlexGridSizer* fgSizer13;
	fgSizer13 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer13->SetFlexibleDirection( wxBOTH );
	fgSizer13->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_download_gauge = new wxGauge( m_solver_template_panel->GetPane(), wxID_ANY, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL );
	m_download_gauge->SetValue( 0 );
	fgSizer13->Add( m_download_gauge, 0, wxALL, 5 );

	m_download_check_gauge = new wxGauge( m_solver_template_panel->GetPane(), wxID_ANY, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL );
	m_download_check_gauge->SetValue( 0 );
	m_download_check_gauge->SetToolTip( _("Check the template's MD5 value") );

	fgSizer13->Add( m_download_check_gauge, 0, wxALL, 5 );


	m_solver_template_sizer->Add( fgSizer13, 1, wxEXPAND, 5 );


	m_solver_template_panel->GetPane()->SetSizer( m_solver_template_sizer );
	m_solver_template_panel->GetPane()->Layout();
	m_solver_template_sizer->Fit( m_solver_template_panel->GetPane() );
	m_dialog_sizer->Add( m_solver_template_panel, 1, wxEXPAND | wxALL, 5 );

	m_staticline5 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	m_dialog_sizer->Add( m_staticline5, 0, wxEXPAND | wxALL, 5 );

	wxFlexGridSizer* m_solver_control_sizer;
	m_solver_control_sizer = new wxFlexGridSizer( 0, 5, 0, 0 );
	m_solver_control_sizer->SetFlexibleDirection( wxBOTH );
	m_solver_control_sizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_solver_start = new wxButton( this, wxID_ANY, _("Solve"), wxDefaultPosition, wxDefaultSize, 0 );
	m_solver_control_sizer->Add( m_solver_start, 0, wxALL, 5 );

	m_solver_stop = new wxButton( this, wxID_ANY, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_solver_control_sizer->Add( m_solver_stop, 0, wxALL, 5 );

	m_solver_copy = new wxButton( this, wxID_ANY, _("Copy"), wxDefaultPosition, wxDefaultSize, 0 );
	m_solver_copy->SetToolTip( _("Copy the result of the RA and DEC") );

	m_solver_control_sizer->Add( m_solver_copy, 0, wxALL, 5 );

	m_solver_exit = new wxButton( this, wxID_ANY, _("Exit"), wxDefaultPosition, wxDefaultSize, 0 );
	m_solver_control_sizer->Add( m_solver_exit, 0, wxALL, 5 );

	m_solver_help = new wxBitmapButton( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 25,25 ), wxBU_AUTODRAW|0 );
	m_solver_control_sizer->Add( m_solver_help, 0, wxALL, 5 );


	m_dialog_sizer->Add( m_solver_control_sizer, 1, wxEXPAND, 5 );

	m_staticline6 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	m_dialog_sizer->Add( m_staticline6, 0, wxEXPAND | wxALL, 5 );

	m_solver_result_panel = new wxCollapsiblePane( this, wxID_ANY, _("Result"), wxDefaultPosition, wxDefaultSize, wxCP_DEFAULT_STYLE );
	m_solver_result_panel->Collapse( false );

	wxFlexGridSizer* m_solver_result_sizer;
	m_solver_result_sizer = new wxFlexGridSizer( 0, 1, 0, 0 );
	m_solver_result_sizer->SetFlexibleDirection( wxBOTH );
	m_solver_result_sizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	wxGridSizer* m_solver_result_radec_sizer;
	m_solver_result_radec_sizer = new wxGridSizer( 0, 3, 0, 0 );

	wxFlexGridSizer* m_solver_result_ra_sizer;
	m_solver_result_ra_sizer = new wxFlexGridSizer( 0, 2, 0, 0 );
	m_solver_result_ra_sizer->SetFlexibleDirection( wxBOTH );
	m_solver_result_ra_sizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_solver_result_ra_t = new wxStaticText( m_solver_result_panel->GetPane(), wxID_ANY, _("RA     "), wxDefaultPosition, wxDefaultSize, 0 );
	m_solver_result_ra_t->Wrap( -1 );
	m_solver_result_ra_t->SetToolTip( _("Left click to copy") );

	m_solver_result_ra_sizer->Add( m_solver_result_ra_t, 0, wxALL, 5 );

	m_solver_result_ra = new wxStaticText( m_solver_result_panel->GetPane(), wxID_ANY, _("               "), wxDefaultPosition, wxDefaultSize, 0 );
	m_solver_result_ra->Wrap( -1 );
	m_solver_result_ra_sizer->Add( m_solver_result_ra, 0, wxALL, 5 );


	m_solver_result_radec_sizer->Add( m_solver_result_ra_sizer, 1, wxEXPAND, 5 );

	wxFlexGridSizer* m_solver_result_dec_sizer;
	m_solver_result_dec_sizer = new wxFlexGridSizer( 0, 2, 0, 0 );
	m_solver_result_dec_sizer->SetFlexibleDirection( wxBOTH );
	m_solver_result_dec_sizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_solver_result_dec_t = new wxStaticText( m_solver_result_panel->GetPane(), wxID_ANY, _("DEC"), wxDefaultPosition, wxDefaultSize, 0 );
	m_solver_result_dec_t->Wrap( -1 );
	m_solver_result_dec_t->SetToolTip( _("Left click to copy") );

	m_solver_result_dec_sizer->Add( m_solver_result_dec_t, 0, wxALL, 5 );

	m_solver_result_dec = new wxStaticText( m_solver_result_panel->GetPane(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_solver_result_dec->Wrap( -1 );
	m_solver_result_dec_sizer->Add( m_solver_result_dec, 0, wxALL, 5 );


	m_solver_result_radec_sizer->Add( m_solver_result_dec_sizer, 1, wxEXPAND, 5 );

	wxFlexGridSizer* m_solver_result_rotation_sizer;
	m_solver_result_rotation_sizer = new wxFlexGridSizer( 0, 2, 0, 0 );
	m_solver_result_rotation_sizer->SetFlexibleDirection( wxBOTH );
	m_solver_result_rotation_sizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_solver_result_ratation_t = new wxStaticText( m_solver_result_panel->GetPane(), wxID_ANY, _("Rotation"), wxDefaultPosition, wxDefaultSize, 0 );
	m_solver_result_ratation_t->Wrap( -1 );
	m_solver_result_rotation_sizer->Add( m_solver_result_ratation_t, 0, wxALL, 5 );

	m_solver_result_ratation = new wxStaticText( m_solver_result_panel->GetPane(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_solver_result_ratation->Wrap( -1 );
	m_solver_result_rotation_sizer->Add( m_solver_result_ratation, 0, wxALL, 5 );


	m_solver_result_radec_sizer->Add( m_solver_result_rotation_sizer, 1, wxEXPAND, 5 );

	wxFlexGridSizer* m_solver_result_fovx_sizer;
	m_solver_result_fovx_sizer = new wxFlexGridSizer( 0, 2, 0, 0 );
	m_solver_result_fovx_sizer->SetFlexibleDirection( wxBOTH );
	m_solver_result_fovx_sizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_solver_result_fovx_t = new wxStaticText( m_solver_result_panel->GetPane(), wxID_ANY, _("Fov(x)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_solver_result_fovx_t->Wrap( -1 );
	m_solver_result_fovx_sizer->Add( m_solver_result_fovx_t, 0, wxALL, 5 );

	m_solver_result_fovx = new wxStaticText( m_solver_result_panel->GetPane(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_solver_result_fovx->Wrap( -1 );
	m_solver_result_fovx_sizer->Add( m_solver_result_fovx, 0, wxALL, 5 );


	m_solver_result_radec_sizer->Add( m_solver_result_fovx_sizer, 1, wxEXPAND, 5 );

	wxFlexGridSizer* m_solver_result_fovy_sizer;
	m_solver_result_fovy_sizer = new wxFlexGridSizer( 0, 2, 0, 0 );
	m_solver_result_fovy_sizer->SetFlexibleDirection( wxBOTH );
	m_solver_result_fovy_sizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_solver_result_fovy_t = new wxStaticText( m_solver_result_panel->GetPane(), wxID_ANY, _("Fov(y)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_solver_result_fovy_t->Wrap( -1 );
	m_solver_result_fovy_sizer->Add( m_solver_result_fovy_t, 0, wxALL, 5 );

	m_solver_result_fovy = new wxStaticText( m_solver_result_panel->GetPane(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_solver_result_fovy->Wrap( -1 );
	m_solver_result_fovy_sizer->Add( m_solver_result_fovy, 0, wxALL, 5 );


	m_solver_result_radec_sizer->Add( m_solver_result_fovy_sizer, 1, wxEXPAND, 5 );

	wxFlexGridSizer* fgSizer20;
	fgSizer20 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer20->SetFlexibleDirection( wxBOTH );
	fgSizer20->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );


	m_solver_result_radec_sizer->Add( fgSizer20, 1, wxEXPAND, 5 );


	m_solver_result_sizer->Add( m_solver_result_radec_sizer, 1, wxEXPAND, 5 );


	m_solver_result_panel->GetPane()->SetSizer( m_solver_result_sizer );
	m_solver_result_panel->GetPane()->Layout();
	m_solver_result_sizer->Fit( m_solver_result_panel->GetPane() );
	m_dialog_sizer->Add( m_solver_result_panel, 1, wxEXPAND | wxALL, 5 );


	m_dialog_box->Add( m_dialog_sizer, 1, wxEXPAND, 5 );


	this->SetSizer( m_dialog_box );
	this->Layout();
	m_dialog_box->Fit( this );

	this->Centre( wxBOTH );

	// Connect Events
	m_solver_exe->Connect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( SolverAssistant::on_solver_changed ), NULL, this );
	m_solver_search->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SolverAssistant::on_solver_search ), NULL, this );
	m_solver_settings_ra_t->Connect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( SolverAssistant::refresh_solver_ra ), NULL, this );
	m_solver_settings_ra->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( SolverAssistant::on_ra_enter ), NULL, this );
	m_solver_settings_dec_t->Connect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( SolverAssistant::refresh_solver_dec ), NULL, this );
	m_solver_settings_dec->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( SolverAssistant::on_dec_enter ), NULL, this );
	m_solver_settings_fov->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( SolverAssistant::on_fov_enter ), NULL, this );
	m_solver_template_choice->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( SolverAssistant::check_template_downloaded ), NULL, this );
	m_solver_template_refresh->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SolverAssistant::on_refresh_template ), NULL, this );
	m_solver_template_download->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SolverAssistant::on_download_template ), NULL, this );
	m_solver_template_open->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SolverAssistant::on_open_template_folder ), NULL, this );
	m_solver_start->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SolverAssistant::on_start_solver ), NULL, this );
	m_solver_stop->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SolverAssistant::on_stop_solver ), NULL, this );
	m_solver_copy->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SolverAssistant::on_copy_solver ), NULL, this );
	m_solver_exit->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SolverAssistant::on_exit_solver ), NULL, this );
	m_solver_help->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SolverAssistant::on_help_solver ), NULL, this );
	m_solver_result_ra_t->Connect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( SolverAssistant::on_copy_solver_result_ra ), NULL, this );
	m_solver_result_ra->Connect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( SolverAssistant::on_copy_solver_result_ra ), NULL, this );
	m_solver_result_dec_t->Connect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( SolverAssistant::on_copy_solver_result_dec ), NULL, this );
	m_solver_result_dec->Connect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( SolverAssistant::on_copy_solver_result_dec ), NULL, this );
	m_solver_result_ratation_t->Connect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( SolverAssistant::on_copy_solver_result_rotation ), NULL, this );
	m_solver_result_ratation->Connect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( SolverAssistant::on_copy_solver_result_rotation ), NULL, this );
	m_solver_result_fovx_t->Connect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( SolverAssistant::on_copy_solver_result_fovx ), NULL, this );
	m_solver_result_fovx->Connect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( SolverAssistant::on_copy_solver_result_fovx ), NULL, this );
	m_solver_result_fovy_t->Connect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( SolverAssistant::on_copy_solver_result_fovy ), NULL, this );
	m_solver_result_fovy->Connect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( SolverAssistant::on_copy_solver_result_fovy ), NULL, this );
}

SolverAssistant::~SolverAssistant()
{
	// Disconnect Events
	m_solver_exe->Disconnect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( SolverAssistant::on_solver_changed ), NULL, this );
	m_solver_search->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SolverAssistant::on_solver_search ), NULL, this );
	m_solver_settings_ra_t->Disconnect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( SolverAssistant::refresh_solver_ra ), NULL, this );
	m_solver_settings_ra->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( SolverAssistant::on_ra_enter ), NULL, this );
	m_solver_settings_dec_t->Disconnect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( SolverAssistant::refresh_solver_dec ), NULL, this );
	m_solver_settings_dec->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( SolverAssistant::on_dec_enter ), NULL, this );
	m_solver_settings_fov->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( SolverAssistant::on_fov_enter ), NULL, this );
	m_solver_template_choice->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( SolverAssistant::check_template_downloaded ), NULL, this );
	m_solver_template_refresh->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SolverAssistant::on_refresh_template ), NULL, this );
	m_solver_template_download->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SolverAssistant::on_download_template ), NULL, this );
	m_solver_template_open->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SolverAssistant::on_open_template_folder ), NULL, this );
	m_solver_start->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SolverAssistant::on_start_solver ), NULL, this );
	m_solver_stop->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SolverAssistant::on_stop_solver ), NULL, this );
	m_solver_copy->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SolverAssistant::on_copy_solver ), NULL, this );
	m_solver_exit->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SolverAssistant::on_exit_solver ), NULL, this );
	m_solver_help->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SolverAssistant::on_help_solver ), NULL, this );
	m_solver_result_ra_t->Disconnect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( SolverAssistant::on_copy_solver_result_ra ), NULL, this );
	m_solver_result_ra->Disconnect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( SolverAssistant::on_copy_solver_result_ra ), NULL, this );
	m_solver_result_dec_t->Disconnect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( SolverAssistant::on_copy_solver_result_dec ), NULL, this );
	m_solver_result_dec->Disconnect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( SolverAssistant::on_copy_solver_result_dec ), NULL, this );
	m_solver_result_ratation_t->Disconnect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( SolverAssistant::on_copy_solver_result_rotation ), NULL, this );
	m_solver_result_ratation->Disconnect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( SolverAssistant::on_copy_solver_result_rotation ), NULL, this );
	m_solver_result_fovx_t->Disconnect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( SolverAssistant::on_copy_solver_result_fovx ), NULL, this );
	m_solver_result_fovx->Disconnect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( SolverAssistant::on_copy_solver_result_fovx ), NULL, this );
	m_solver_result_fovy_t->Disconnect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( SolverAssistant::on_copy_solver_result_fovy ), NULL, this );
	m_solver_result_fovy->Disconnect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( SolverAssistant::on_copy_solver_result_fovy ), NULL, this );

}
