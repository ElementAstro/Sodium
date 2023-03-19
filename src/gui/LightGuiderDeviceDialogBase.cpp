///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 3.10.1-0-g8feb16b3)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "lightguider.h"
#include "LightGuiderDeviceDialogBase.h"

///////////////////////////////////////////////////////////////////////////

DeviceDialog::DeviceDialog()
{
}

DeviceDialog::DeviceDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style )
{
	this->Create( parent, id, title, pos, size, style );
}

bool DeviceDialog::Create( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style )
{
	if ( !wxDialog::Create( parent, id, title, pos, size, style ) )
	{
		return false;
	}

	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	m_mgr.SetManagedWindow(this);
	m_mgr.SetFlags(wxAUI_MGR_DEFAULT);

	m_main_listbook = new wxListbook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLB_DEFAULT );
	m_mgr.AddPane( m_main_listbook, wxAuiPaneInfo() .Left() .CaptionVisible( false ).CloseButton( false ).PaneBorder( false ).Movable( false ).Dock().Resizable().FloatingSize( wxDefaultSize ).Floatable( false ).CentrePane().DefaultPane() );

	m_camera_panel = new wxPanel( m_main_listbook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* m_camera_box;
	m_camera_box = new wxBoxSizer( wxVERTICAL );

	wxFlexGridSizer* m_camera_sizer;
	m_camera_sizer = new wxFlexGridSizer( 0, 1, 0, 0 );
	m_camera_sizer->SetFlexibleDirection( wxBOTH );
	m_camera_sizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	wxFlexGridSizer* m_camera_connect_sizer;
	m_camera_connect_sizer = new wxFlexGridSizer( 0, 5, 0, 0 );
	m_camera_connect_sizer->SetFlexibleDirection( wxBOTH );
	m_camera_connect_sizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_camera_title = new wxStaticText( m_camera_panel, wxID_ANY, _("Camera"), wxDefaultPosition, wxDefaultSize, 0 );
	m_camera_title->Wrap( -1 );
	m_camera_connect_sizer->Add( m_camera_title, 0, wxALL, 5 );

	wxString m_camera_choiceChoices[] = { _("Simulator") };
	int m_camera_choiceNChoices = sizeof( m_camera_choiceChoices ) / sizeof( wxString );
	m_camera_choice = new wxChoice( m_camera_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_camera_choiceNChoices, m_camera_choiceChoices, 0 );
	m_camera_choice->SetSelection( 0 );
	m_camera_connect_sizer->Add( m_camera_choice, 0, wxALL, 5 );

	m_camera_settings_btn = new wxBitmapButton( m_camera_panel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|0 );
	m_camera_settings_btn->SetToolTip( _("Settings") );

	m_camera_connect_sizer->Add( m_camera_settings_btn, 0, wxALL, 5 );

	m_camera_connect_btn = new wxBitmapButton( m_camera_panel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|0 );
	m_camera_connect_btn->SetToolTip( _("Connect / Disconnect") );

	m_camera_connect_sizer->Add( m_camera_connect_btn, 0, wxALL, 5 );

	m_camera_scan_btn = new wxBitmapButton( m_camera_panel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|0 );
	m_camera_scan_btn->SetToolTip( _("Scan all of the devices available") );

	m_camera_connect_sizer->Add( m_camera_scan_btn, 0, wxALL, 5 );


	m_camera_sizer->Add( m_camera_connect_sizer, 1, wxEXPAND, 5 );

	m_staticline11 = new wxStaticLine( m_camera_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	m_camera_sizer->Add( m_staticline11, 0, wxEXPAND | wxALL, 5 );

	wxGridBagSizer* m_camera_name_sizer;
	m_camera_name_sizer = new wxGridBagSizer( 0, 0 );
	m_camera_name_sizer->SetFlexibleDirection( wxBOTH );
	m_camera_name_sizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_camera_name_t = new wxStaticText( m_camera_panel, wxID_ANY, _("Name"), wxDefaultPosition, wxDefaultSize, 0 );
	m_camera_name_t->Wrap( -1 );
	m_camera_name_sizer->Add( m_camera_name_t, wxGBPosition( 0, 0 ), wxGBSpan( 1, 1 ), wxALL, 5 );

	m_camera_name = new wxStaticText( m_camera_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_camera_name->Wrap( -1 );
	m_camera_name_sizer->Add( m_camera_name, wxGBPosition( 0, 1 ), wxGBSpan( 1, 1 ), wxALL, 5 );


	m_camera_sizer->Add( m_camera_name_sizer, 1, wxEXPAND, 5 );

	wxGridBagSizer* m_camera_description_sizer;
	m_camera_description_sizer = new wxGridBagSizer( 0, 0 );
	m_camera_description_sizer->SetFlexibleDirection( wxBOTH );
	m_camera_description_sizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_camera_description_t = new wxStaticText( m_camera_panel, wxID_ANY, _("Description"), wxDefaultPosition, wxDefaultSize, 0 );
	m_camera_description_t->Wrap( -1 );
	m_camera_description_sizer->Add( m_camera_description_t, wxGBPosition( 0, 0 ), wxGBSpan( 1, 1 ), wxALL, 5 );

	m_camera_description = new wxStaticText( m_camera_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_camera_description->Wrap( -1 );
	m_camera_description_sizer->Add( m_camera_description, wxGBPosition( 0, 1 ), wxGBSpan( 1, 1 ), wxALL, 5 );


	m_camera_sizer->Add( m_camera_description_sizer, 1, wxEXPAND, 5 );

	m_staticline12 = new wxStaticLine( m_camera_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	m_camera_sizer->Add( m_staticline12, 0, wxEXPAND | wxALL, 5 );

	wxFlexGridSizer* m_camera_info_sizer;
	m_camera_info_sizer = new wxFlexGridSizer( 0, 4, 0, 0 );
	m_camera_info_sizer->SetFlexibleDirection( wxBOTH );
	m_camera_info_sizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_camera_gain_t = new wxStaticText( m_camera_panel, wxID_ANY, _("Gain"), wxDefaultPosition, wxDefaultSize, 0 );
	m_camera_gain_t->Wrap( -1 );
	m_camera_info_sizer->Add( m_camera_gain_t, 0, wxALL, 5 );

	m_camera_gain = new wxTextCtrl( m_camera_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	#ifdef __WXGTK__
	if ( !m_camera_gain->HasFlag( wxTE_MULTILINE ) )
	{
	m_camera_gain->SetMaxLength( 3 );
	}
	#else
	m_camera_gain->SetMaxLength( 3 );
	#endif
	m_camera_gain->Enable( false );

	m_camera_info_sizer->Add( m_camera_gain, 0, wxALL, 5 );

	m_camera_offset_t = new wxStaticText( m_camera_panel, wxID_ANY, _("Offset"), wxDefaultPosition, wxDefaultSize, 0 );
	m_camera_offset_t->Wrap( -1 );
	m_camera_info_sizer->Add( m_camera_offset_t, 0, wxALL, 5 );

	m_camera_offset = new wxTextCtrl( m_camera_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	#ifdef __WXGTK__
	if ( !m_camera_offset->HasFlag( wxTE_MULTILINE ) )
	{
	m_camera_offset->SetMaxLength( 3 );
	}
	#else
	m_camera_offset->SetMaxLength( 3 );
	#endif
	m_camera_offset->Enable( false );

	m_camera_info_sizer->Add( m_camera_offset, 0, wxALL, 5 );

	m_camera_bin_t = new wxStaticText( m_camera_panel, wxID_ANY, _("Bin"), wxDefaultPosition, wxDefaultSize, 0 );
	m_camera_bin_t->Wrap( -1 );
	m_camera_info_sizer->Add( m_camera_bin_t, 0, wxALL, 5 );

	wxString m_camera_binChoices[] = { _("1x1"), _("2x2"), _("4x4") };
	int m_camera_binNChoices = sizeof( m_camera_binChoices ) / sizeof( wxString );
	m_camera_bin = new wxChoice( m_camera_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_camera_binNChoices, m_camera_binChoices, 0 );
	m_camera_bin->SetSelection( 0 );
	m_camera_info_sizer->Add( m_camera_bin, 0, wxALL, 5 );

	m_camera_cooling_t = new wxStaticText( m_camera_panel, wxID_ANY, _("Cooling"), wxDefaultPosition, wxDefaultSize, 0 );
	m_camera_cooling_t->Wrap( -1 );
	m_camera_info_sizer->Add( m_camera_cooling_t, 0, wxALL, 5 );

	m_camera_cooling = new wxCheckBox( m_camera_panel, wxID_ANY, _("Enable"), wxDefaultPosition, wxDefaultSize, 0 );
	m_camera_info_sizer->Add( m_camera_cooling, 0, wxALL, 5 );

	m_camera_temperature_t = new wxStaticText( m_camera_panel, wxID_ANY, _("Temperature"), wxDefaultPosition, wxDefaultSize, 0 );
	m_camera_temperature_t->Wrap( -1 );
	m_camera_info_sizer->Add( m_camera_temperature_t, 0, wxALL, 5 );

	m_camera_temperature = new wxTextCtrl( m_camera_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_camera_temperature->Enable( false );

	m_camera_info_sizer->Add( m_camera_temperature, 0, wxALL, 5 );

	m_camera_power_t = new wxStaticText( m_camera_panel, wxID_ANY, _("Power"), wxDefaultPosition, wxDefaultSize, 0 );
	m_camera_power_t->Wrap( -1 );
	m_camera_info_sizer->Add( m_camera_power_t, 0, wxALL, 5 );

	m_camera_power = new wxTextCtrl( m_camera_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_camera_power->Enable( false );

	m_camera_info_sizer->Add( m_camera_power, 0, wxALL, 5 );

	m_camera_driverversion_t = new wxStaticText( m_camera_panel, wxID_ANY, _("DriverVersion"), wxDefaultPosition, wxDefaultSize, 0 );
	m_camera_driverversion_t->Wrap( -1 );
	m_camera_info_sizer->Add( m_camera_driverversion_t, 0, wxALL, 5 );

	m_camera_driverversion = new wxStaticText( m_camera_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_camera_driverversion->Wrap( -1 );
	m_camera_info_sizer->Add( m_camera_driverversion, 0, wxALL, 5 );

	m_camera_sensor_t = new wxStaticText( m_camera_panel, wxID_ANY, _("Sensor"), wxDefaultPosition, wxDefaultSize, 0 );
	m_camera_sensor_t->Wrap( -1 );
	m_camera_info_sizer->Add( m_camera_sensor_t, 0, wxALL, 5 );

	m_camera_sensor = new wxStaticText( m_camera_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_camera_sensor->Wrap( -1 );
	m_camera_info_sizer->Add( m_camera_sensor, 0, wxALL, 5 );

	m_camera_height_t = new wxStaticText( m_camera_panel, wxID_ANY, _("ImageHeight"), wxDefaultPosition, wxDefaultSize, 0 );
	m_camera_height_t->Wrap( -1 );
	m_camera_info_sizer->Add( m_camera_height_t, 0, wxALL, 5 );

	m_camera_height = new wxStaticText( m_camera_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_camera_height->Wrap( -1 );
	m_camera_info_sizer->Add( m_camera_height, 0, wxALL, 5 );

	m_camera_width_t = new wxStaticText( m_camera_panel, wxID_ANY, _("ImageWidth"), wxDefaultPosition, wxDefaultSize, 0 );
	m_camera_width_t->Wrap( -1 );
	m_camera_info_sizer->Add( m_camera_width_t, 0, wxALL, 5 );

	m_camera_width = new wxStaticText( m_camera_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_camera_width->Wrap( -1 );
	m_camera_info_sizer->Add( m_camera_width, 0, wxALL, 5 );

	m_camera_pixelx_t = new wxStaticText( m_camera_panel, wxID_ANY, _("Pixel(x)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_camera_pixelx_t->Wrap( -1 );
	m_camera_info_sizer->Add( m_camera_pixelx_t, 0, wxALL, 5 );

	m_camera_pixelx = new wxStaticText( m_camera_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_camera_pixelx->Wrap( -1 );
	m_camera_info_sizer->Add( m_camera_pixelx, 0, wxALL, 5 );

	m_camera_pixely_t = new wxStaticText( m_camera_panel, wxID_ANY, _("Pixel(y)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_camera_pixely_t->Wrap( -1 );
	m_camera_info_sizer->Add( m_camera_pixely_t, 0, wxALL, 5 );

	m_camera_pixely = new wxStaticText( m_camera_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_camera_pixely->Wrap( -1 );
	m_camera_info_sizer->Add( m_camera_pixely, 0, wxALL, 5 );

	m_camera_maxbinx_t = new wxStaticText( m_camera_panel, wxID_ANY, _("MaxBin(x)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_camera_maxbinx_t->Wrap( -1 );
	m_camera_info_sizer->Add( m_camera_maxbinx_t, 0, wxALL, 5 );

	m_camera_maxbinx = new wxStaticText( m_camera_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_camera_maxbinx->Wrap( -1 );
	m_camera_info_sizer->Add( m_camera_maxbinx, 0, wxALL, 5 );

	m_camera_biny_t = new wxStaticText( m_camera_panel, wxID_ANY, _("MaxBin(y)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_camera_biny_t->SetLabelMarkup( _("MaxBin(y)") );
	m_camera_biny_t->Wrap( -1 );
	m_camera_info_sizer->Add( m_camera_biny_t, 0, wxALL, 5 );

	m_camera_biny = new wxStaticText( m_camera_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_camera_biny->SetLabelMarkup( wxEmptyString );
	m_camera_biny->Wrap( -1 );
	m_camera_info_sizer->Add( m_camera_biny, 0, wxALL, 5 );

	m_camera_maxexposure_t = new wxStaticText( m_camera_panel, wxID_ANY, _("MaxExposure"), wxDefaultPosition, wxDefaultSize, 0 );
	m_camera_maxexposure_t->Wrap( -1 );
	m_camera_info_sizer->Add( m_camera_maxexposure_t, 0, wxALL, 5 );

	m_camera_maxexposure = new wxStaticText( m_camera_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_camera_maxexposure->Wrap( -1 );
	m_camera_info_sizer->Add( m_camera_maxexposure, 0, wxALL, 5 );

	m_camera_minexposure_t = new wxStaticText( m_camera_panel, wxID_ANY, _("MinExposure"), wxDefaultPosition, wxDefaultSize, 0 );
	m_camera_minexposure_t->Wrap( -1 );
	m_camera_info_sizer->Add( m_camera_minexposure_t, 0, wxALL, 5 );

	m_camera_minexposure = new wxStaticText( m_camera_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_camera_minexposure->Wrap( -1 );
	m_camera_info_sizer->Add( m_camera_minexposure, 0, wxALL, 5 );


	m_camera_sizer->Add( m_camera_info_sizer, 1, wxEXPAND, 5 );

	m_staticline13 = new wxStaticLine( m_camera_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	m_camera_sizer->Add( m_staticline13, 0, wxEXPAND | wxALL, 5 );

	m_camera_temperature_title = new wxStaticText( m_camera_panel, wxID_ANY, _("Temperature Graph"), wxDefaultPosition, wxDefaultSize, 0 );
	m_camera_temperature_title->Wrap( -1 );
	m_camera_sizer->Add( m_camera_temperature_title, 0, wxALL, 5 );

	wxFlexGridSizer* fgSizer31;
	fgSizer31 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer31->SetFlexibleDirection( wxBOTH );
	fgSizer31->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_panel7 = new wxPanel( m_camera_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	fgSizer31->Add( m_panel7, 1, wxEXPAND | wxALL, 5 );


	m_camera_sizer->Add( fgSizer31, 1, wxEXPAND, 5 );


	m_camera_box->Add( m_camera_sizer, 1, wxEXPAND, 5 );


	m_camera_panel->SetSizer( m_camera_box );
	m_camera_panel->Layout();
	m_camera_box->Fit( m_camera_panel );
	m_main_listbook->AddPage( m_camera_panel, _("Camera"), true );
	m_telescope_panel = new wxPanel( m_main_listbook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer12;
	bSizer12 = new wxBoxSizer( wxVERTICAL );

	wxGridSizer* gSizer3;
	gSizer3 = new wxGridSizer( 0, 2, 0, 0 );

	wxFlexGridSizer* fgSizer221;
	fgSizer221 = new wxFlexGridSizer( 0, 1, 0, 0 );
	fgSizer221->SetFlexibleDirection( wxBOTH );
	fgSizer221->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	wxFlexGridSizer* fgSizer241;
	fgSizer241 = new wxFlexGridSizer( 0, 5, 0, 0 );
	fgSizer241->SetFlexibleDirection( wxBOTH );
	fgSizer241->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText321 = new wxStaticText( m_telescope_panel, wxID_ANY, _("Telescope"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText321->Wrap( -1 );
	fgSizer241->Add( m_staticText321, 0, wxALL, 5 );

	wxString m_choice41Choices[] = { _("Simulator") };
	int m_choice41NChoices = sizeof( m_choice41Choices ) / sizeof( wxString );
	m_choice41 = new wxChoice( m_telescope_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice41NChoices, m_choice41Choices, 0 );
	m_choice41->SetSelection( 0 );
	fgSizer241->Add( m_choice41, 0, wxALL, 5 );

	m_bpButton21 = new wxBitmapButton( m_telescope_panel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|0 );
	fgSizer241->Add( m_bpButton21, 0, wxALL, 5 );

	m_bpButton31 = new wxBitmapButton( m_telescope_panel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|0 );
	fgSizer241->Add( m_bpButton31, 0, wxALL, 5 );

	m_bpButton41 = new wxBitmapButton( m_telescope_panel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|0 );
	fgSizer241->Add( m_bpButton41, 0, wxALL, 5 );


	fgSizer221->Add( fgSizer241, 1, wxEXPAND, 5 );

	m_staticline111 = new wxStaticLine( m_telescope_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	fgSizer221->Add( m_staticline111, 0, wxEXPAND | wxALL, 5 );

	wxGridBagSizer* gbSizer31;
	gbSizer31 = new wxGridBagSizer( 0, 0 );
	gbSizer31->SetFlexibleDirection( wxBOTH );
	gbSizer31->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText711 = new wxStaticText( m_telescope_panel, wxID_ANY, _("Name"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText711->Wrap( -1 );
	gbSizer31->Add( m_staticText711, wxGBPosition( 0, 0 ), wxGBSpan( 1, 1 ), wxALL, 5 );

	m_staticText721 = new wxStaticText( m_telescope_panel, wxID_ANY, _("Simulator"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText721->Wrap( -1 );
	gbSizer31->Add( m_staticText721, wxGBPosition( 0, 1 ), wxGBSpan( 1, 1 ), wxALL, 5 );


	fgSizer221->Add( gbSizer31, 1, wxEXPAND, 5 );

	wxGridBagSizer* gbSizer51;
	gbSizer51 = new wxGridBagSizer( 0, 0 );
	gbSizer51->SetFlexibleDirection( wxBOTH );
	gbSizer51->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText691 = new wxStaticText( m_telescope_panel, wxID_ANY, _("Description"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText691->Wrap( -1 );
	gbSizer51->Add( m_staticText691, wxGBPosition( 0, 0 ), wxGBSpan( 1, 1 ), wxALL, 5 );

	m_staticText701 = new wxStaticText( m_telescope_panel, wxID_ANY, _("A basic simulator telescope"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText701->Wrap( -1 );
	gbSizer51->Add( m_staticText701, wxGBPosition( 0, 1 ), wxGBSpan( 1, 1 ), wxALL, 5 );


	fgSizer221->Add( gbSizer51, 1, wxEXPAND, 5 );

	m_staticline121 = new wxStaticLine( m_telescope_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	fgSizer221->Add( m_staticline121, 0, wxEXPAND | wxALL, 5 );

	wxFlexGridSizer* fgSizer311;
	fgSizer311 = new wxFlexGridSizer( 0, 4, 0, 0 );
	fgSizer311->SetFlexibleDirection( wxBOTH );
	fgSizer311->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText160 = new wxStaticText( m_telescope_panel, wxID_ANY, _("DriverVersion"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText160->Wrap( -1 );
	fgSizer311->Add( m_staticText160, 0, wxALL, 5 );

	m_staticText161 = new wxStaticText( m_telescope_panel, wxID_ANY, _("          "), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText161->Wrap( -1 );
	fgSizer311->Add( m_staticText161, 0, wxALL, 5 );

	m_staticText162 = new wxStaticText( m_telescope_panel, wxID_ANY, _("DrvierInfo"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText162->Wrap( -1 );
	fgSizer311->Add( m_staticText162, 0, wxALL, 5 );

	m_staticText163 = new wxStaticText( m_telescope_panel, wxID_ANY, _("          "), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText163->Wrap( -1 );
	fgSizer311->Add( m_staticText163, 0, wxALL, 5 );

	m_staticText164 = new wxStaticText( m_telescope_panel, wxID_ANY, _("Longitude"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText164->Wrap( -1 );
	fgSizer311->Add( m_staticText164, 0, wxALL, 5 );

	m_staticText165 = new wxStaticText( m_telescope_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText165->Wrap( -1 );
	fgSizer311->Add( m_staticText165, 0, wxALL, 5 );

	m_staticText166 = new wxStaticText( m_telescope_panel, wxID_ANY, _("Latitude"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText166->Wrap( -1 );
	fgSizer311->Add( m_staticText166, 0, wxALL, 5 );

	m_staticText167 = new wxStaticText( m_telescope_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText167->Wrap( -1 );
	fgSizer311->Add( m_staticText167, 0, wxALL, 5 );

	m_staticText168 = new wxStaticText( m_telescope_panel, wxID_ANY, _("AZ"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText168->Wrap( -1 );
	fgSizer311->Add( m_staticText168, 0, wxALL, 5 );

	m_staticText169 = new wxStaticText( m_telescope_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText169->Wrap( -1 );
	fgSizer311->Add( m_staticText169, 0, wxALL, 5 );

	m_staticText170 = new wxStaticText( m_telescope_panel, wxID_ANY, _("ALT"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText170->Wrap( -1 );
	fgSizer311->Add( m_staticText170, 0, wxALL, 5 );

	m_staticText171 = new wxStaticText( m_telescope_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText171->Wrap( -1 );
	fgSizer311->Add( m_staticText171, 0, wxALL, 5 );

	m_staticText172 = new wxStaticText( m_telescope_panel, wxID_ANY, _("RA"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText172->Wrap( -1 );
	fgSizer311->Add( m_staticText172, 0, wxALL, 5 );

	m_staticText173 = new wxStaticText( m_telescope_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText173->Wrap( -1 );
	fgSizer311->Add( m_staticText173, 0, wxALL, 5 );

	m_staticText174 = new wxStaticText( m_telescope_panel, wxID_ANY, _("DEC"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText174->Wrap( -1 );
	fgSizer311->Add( m_staticText174, 0, wxALL, 5 );

	m_staticText175 = new wxStaticText( m_telescope_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText175->Wrap( -1 );
	fgSizer311->Add( m_staticText175, 0, wxALL, 5 );

	m_staticText176 = new wxStaticText( m_telescope_panel, wxID_ANY, _("Elevation"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText176->Wrap( -1 );
	fgSizer311->Add( m_staticText176, 0, wxALL, 5 );

	m_staticText177 = new wxStaticText( m_telescope_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText177->Wrap( -1 );
	fgSizer311->Add( m_staticText177, 0, wxALL, 5 );

	m_staticText178 = new wxStaticText( m_telescope_panel, wxID_ANY, _("Transmit"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText178->Wrap( -1 );
	fgSizer311->Add( m_staticText178, 0, wxALL, 5 );

	m_staticText179 = new wxStaticText( m_telescope_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText179->Wrap( -1 );
	fgSizer311->Add( m_staticText179, 0, wxALL, 5 );


	fgSizer221->Add( fgSizer311, 1, wxEXPAND, 5 );

	m_staticline24 = new wxStaticLine( m_telescope_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	fgSizer221->Add( m_staticline24, 0, wxEXPAND | wxALL, 5 );

	wxFlexGridSizer* fgSizer49;
	fgSizer49 = new wxFlexGridSizer( 0, 3, 0, 0 );
	fgSizer49->SetFlexibleDirection( wxBOTH );
	fgSizer49->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_button25 = new wxButton( m_telescope_panel, wxID_ANY, _("Track"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer49->Add( m_button25, 0, wxALL, 5 );

	m_button26 = new wxButton( m_telescope_panel, wxID_ANY, _("Home"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer49->Add( m_button26, 0, wxALL, 5 );

	m_button27 = new wxButton( m_telescope_panel, wxID_ANY, _("Park"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer49->Add( m_button27, 0, wxALL, 5 );

	m_button28 = new wxButton( m_telescope_panel, wxID_ANY, _("Random"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer49->Add( m_button28, 0, wxALL, 5 );

	m_button29 = new wxButton( m_telescope_panel, wxID_ANY, _("Sync"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer49->Add( m_button29, 0, wxALL, 5 );

	m_button30 = new wxButton( m_telescope_panel, wxID_ANY, _("GetInfo"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer49->Add( m_button30, 0, wxALL, 5 );


	fgSizer221->Add( fgSizer49, 1, wxEXPAND, 5 );

	m_staticline26 = new wxStaticLine( m_telescope_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	fgSizer221->Add( m_staticline26, 0, wxEXPAND | wxALL, 5 );


	gSizer3->Add( fgSizer221, 1, wxEXPAND, 5 );

	wxFlexGridSizer* fgSizer44;
	fgSizer44 = new wxFlexGridSizer( 0, 1, 0, 0 );
	fgSizer44->SetFlexibleDirection( wxBOTH );
	fgSizer44->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText188 = new wxStaticText( m_telescope_panel, wxID_ANY, _("Telescope Control"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText188->Wrap( -1 );
	fgSizer44->Add( m_staticText188, 0, wxALL, 5 );

	m_staticline21 = new wxStaticLine( m_telescope_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	fgSizer44->Add( m_staticline21, 0, wxEXPAND | wxALL, 5 );

	wxFlexGridSizer* fgSizer46;
	fgSizer46 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer46->SetFlexibleDirection( wxBOTH );
	fgSizer46->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	wxFlexGridSizer* fgSizer45;
	fgSizer45 = new wxFlexGridSizer( 0, 7, 0, 0 );
	fgSizer45->SetFlexibleDirection( wxBOTH );
	fgSizer45->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText189 = new wxStaticText( m_telescope_panel, wxID_ANY, _("RA"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText189->Wrap( -1 );
	fgSizer45->Add( m_staticText189, 0, wxALL, 5 );

	m_textCtrl47 = new wxTextCtrl( m_telescope_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	#ifdef __WXGTK__
	if ( !m_textCtrl47->HasFlag( wxTE_MULTILINE ) )
	{
	m_textCtrl47->SetMaxLength( 2 );
	}
	#else
	m_textCtrl47->SetMaxLength( 2 );
	#endif
	m_textCtrl47->SetMaxSize( wxSize( 30,-1 ) );

	fgSizer45->Add( m_textCtrl47, 0, wxALL, 5 );

	m_staticText190 = new wxStaticText( m_telescope_panel, wxID_ANY, _("h"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText190->Wrap( -1 );
	fgSizer45->Add( m_staticText190, 0, wxALL, 5 );

	m_textCtrl48 = new wxTextCtrl( m_telescope_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	#ifdef __WXGTK__
	if ( !m_textCtrl48->HasFlag( wxTE_MULTILINE ) )
	{
	m_textCtrl48->SetMaxLength( 2 );
	}
	#else
	m_textCtrl48->SetMaxLength( 2 );
	#endif
	m_textCtrl48->SetMaxSize( wxSize( 30,-1 ) );

	fgSizer45->Add( m_textCtrl48, 0, wxALL, 5 );

	m_staticText191 = new wxStaticText( m_telescope_panel, wxID_ANY, _("m"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText191->Wrap( -1 );
	fgSizer45->Add( m_staticText191, 0, wxALL, 5 );

	m_textCtrl49 = new wxTextCtrl( m_telescope_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	#ifdef __WXGTK__
	if ( !m_textCtrl49->HasFlag( wxTE_MULTILINE ) )
	{
	m_textCtrl49->SetMaxLength( 2 );
	}
	#else
	m_textCtrl49->SetMaxLength( 2 );
	#endif
	m_textCtrl49->SetMaxSize( wxSize( 30,-1 ) );

	fgSizer45->Add( m_textCtrl49, 0, wxALL, 5 );

	m_staticText192 = new wxStaticText( m_telescope_panel, wxID_ANY, _("s"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText192->Wrap( -1 );
	fgSizer45->Add( m_staticText192, 0, wxALL, 5 );

	m_staticText193 = new wxStaticText( m_telescope_panel, wxID_ANY, _("DEC"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText193->Wrap( -1 );
	fgSizer45->Add( m_staticText193, 0, wxALL, 5 );

	m_textCtrl50 = new wxTextCtrl( m_telescope_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	#ifdef __WXGTK__
	if ( !m_textCtrl50->HasFlag( wxTE_MULTILINE ) )
	{
	m_textCtrl50->SetMaxLength( 4 );
	}
	#else
	m_textCtrl50->SetMaxLength( 4 );
	#endif
	m_textCtrl50->SetMaxSize( wxSize( 30,-1 ) );

	fgSizer45->Add( m_textCtrl50, 0, wxALL, 5 );

	m_staticText194 = new wxStaticText( m_telescope_panel, wxID_ANY, _("h"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText194->Wrap( -1 );
	fgSizer45->Add( m_staticText194, 0, wxALL, 5 );

	m_textCtrl51 = new wxTextCtrl( m_telescope_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrl51->SetMaxSize( wxSize( 30,-1 ) );

	fgSizer45->Add( m_textCtrl51, 0, wxALL, 5 );

	m_staticText195 = new wxStaticText( m_telescope_panel, wxID_ANY, _("m"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText195->Wrap( -1 );
	fgSizer45->Add( m_staticText195, 0, wxALL, 5 );

	m_textCtrl52 = new wxTextCtrl( m_telescope_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrl52->SetMaxSize( wxSize( 30,-1 ) );

	fgSizer45->Add( m_textCtrl52, 0, wxALL, 5 );

	m_staticText196 = new wxStaticText( m_telescope_panel, wxID_ANY, _("s"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText196->SetLabelMarkup( _("s") );
	m_staticText196->Wrap( -1 );
	fgSizer45->Add( m_staticText196, 0, wxALL, 5 );


	fgSizer46->Add( fgSizer45, 1, wxEXPAND, 5 );

	wxFlexGridSizer* fgSizer47;
	fgSizer47 = new wxFlexGridSizer( 0, 1, 0, 0 );
	fgSizer47->SetFlexibleDirection( wxBOTH );
	fgSizer47->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_button23 = new wxButton( m_telescope_panel, wxID_ANY, _("MyButton"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer47->Add( m_button23, 0, wxALL, 5 );

	m_button24 = new wxButton( m_telescope_panel, wxID_ANY, _("MyButton"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer47->Add( m_button24, 0, wxALL, 5 );


	fgSizer46->Add( fgSizer47, 1, wxEXPAND, 5 );


	fgSizer44->Add( fgSizer46, 1, wxEXPAND, 5 );

	m_staticline22 = new wxStaticLine( m_telescope_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	fgSizer44->Add( m_staticline22, 0, wxEXPAND | wxALL, 5 );

	wxGridSizer* gSizer4;
	gSizer4 = new wxGridSizer( 3, 3, 0, 0 );

	m_bpButton28 = new wxBitmapButton( m_telescope_panel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 100,100 ), wxBU_AUTODRAW|0 );
	gSizer4->Add( m_bpButton28, 0, wxALL, 0 );

	m_bpButton29 = new wxBitmapButton( m_telescope_panel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 100,100 ), wxBU_AUTODRAW|0 );
	gSizer4->Add( m_bpButton29, 0, wxALL, 0 );

	m_bpButton30 = new wxBitmapButton( m_telescope_panel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 100,100 ), wxBU_AUTODRAW|0 );
	gSizer4->Add( m_bpButton30, 0, wxALL, 0 );

	m_bpButton311 = new wxBitmapButton( m_telescope_panel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 100,100 ), wxBU_AUTODRAW|0 );
	gSizer4->Add( m_bpButton311, 0, wxALL, 0 );

	m_bpButton32 = new wxBitmapButton( m_telescope_panel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 100,100 ), wxBU_AUTODRAW|0 );
	gSizer4->Add( m_bpButton32, 0, wxALL, 0 );

	m_bpButton33 = new wxBitmapButton( m_telescope_panel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 100,100 ), wxBU_AUTODRAW|0 );
	gSizer4->Add( m_bpButton33, 0, wxALL, 0 );

	m_bpButton34 = new wxBitmapButton( m_telescope_panel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 100,100 ), wxBU_AUTODRAW|0 );
	gSizer4->Add( m_bpButton34, 0, wxALL, 0 );

	m_bpButton35 = new wxBitmapButton( m_telescope_panel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 100,100 ), wxBU_AUTODRAW|0 );
	gSizer4->Add( m_bpButton35, 0, wxALL, 0 );

	m_bpButton36 = new wxBitmapButton( m_telescope_panel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 100,100 ), wxBU_AUTODRAW|0 );
	gSizer4->Add( m_bpButton36, 0, wxALL, 0 );


	fgSizer44->Add( gSizer4, 1, wxEXPAND, 5 );

	wxFlexGridSizer* fgSizer48;
	fgSizer48 = new wxFlexGridSizer( 0, 4, 0, 0 );
	fgSizer48->SetFlexibleDirection( wxBOTH );
	fgSizer48->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText197 = new wxStaticText( m_telescope_panel, wxID_ANY, _("RA"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText197->Wrap( -1 );
	fgSizer48->Add( m_staticText197, 0, wxALL, 5 );

	m_slider2 = new wxSlider( m_telescope_panel, wxID_ANY, 50, 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
	fgSizer48->Add( m_slider2, 0, wxALL, 5 );

	m_staticText198 = new wxStaticText( m_telescope_panel, wxID_ANY, _("DEC"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText198->Wrap( -1 );
	fgSizer48->Add( m_staticText198, 0, wxALL, 5 );

	m_slider3 = new wxSlider( m_telescope_panel, wxID_ANY, 50, 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
	fgSizer48->Add( m_slider3, 0, wxALL, 5 );


	fgSizer44->Add( fgSizer48, 1, wxEXPAND, 5 );

	m_staticline23 = new wxStaticLine( m_telescope_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	fgSizer44->Add( m_staticline23, 0, wxEXPAND | wxALL, 5 );


	gSizer3->Add( fgSizer44, 1, wxEXPAND, 5 );


	bSizer12->Add( gSizer3, 1, wxEXPAND, 5 );


	m_telescope_panel->SetSizer( bSizer12 );
	m_telescope_panel->Layout();
	bSizer12->Fit( m_telescope_panel );
	m_main_listbook->AddPage( m_telescope_panel, _("Telescope"), false );
	m_focuser_panel = new wxPanel( m_main_listbook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer13;
	bSizer13 = new wxBoxSizer( wxVERTICAL );

	wxGridSizer* gSizer5;
	gSizer5 = new wxGridSizer( 0, 2, 0, 0 );

	wxFlexGridSizer* fgSizer52;
	fgSizer52 = new wxFlexGridSizer( 0, 1, 0, 0 );
	fgSizer52->SetFlexibleDirection( wxBOTH );
	fgSizer52->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	wxFlexGridSizer* fgSizer50;
	fgSizer50 = new wxFlexGridSizer( 0, 5, 0, 0 );
	fgSizer50->SetFlexibleDirection( wxBOTH );
	fgSizer50->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText201 = new wxStaticText( m_focuser_panel, wxID_ANY, _("Focuser"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText201->Wrap( -1 );
	fgSizer50->Add( m_staticText201, 0, wxALL, 5 );

	wxArrayString m_choice11Choices;
	m_choice11 = new wxChoice( m_focuser_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice11Choices, 0 );
	m_choice11->SetSelection( 0 );
	fgSizer50->Add( m_choice11, 0, wxALL, 5 );

	m_bpButton37 = new wxBitmapButton( m_focuser_panel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|0 );
	fgSizer50->Add( m_bpButton37, 0, wxALL, 5 );

	m_bpButton38 = new wxBitmapButton( m_focuser_panel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|0 );
	fgSizer50->Add( m_bpButton38, 0, wxALL, 5 );

	m_bpButton39 = new wxBitmapButton( m_focuser_panel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|0 );
	fgSizer50->Add( m_bpButton39, 0, wxALL, 5 );


	fgSizer52->Add( fgSizer50, 1, wxEXPAND, 5 );

	m_staticline25 = new wxStaticLine( m_focuser_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	fgSizer52->Add( m_staticline25, 0, wxEXPAND | wxALL, 5 );

	wxFlexGridSizer* fgSizer51;
	fgSizer51 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer51->SetFlexibleDirection( wxBOTH );
	fgSizer51->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText202 = new wxStaticText( m_focuser_panel, wxID_ANY, _("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText202->Wrap( -1 );
	fgSizer51->Add( m_staticText202, 0, wxALL, 5 );

	m_staticText203 = new wxStaticText( m_focuser_panel, wxID_ANY, _("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText203->Wrap( -1 );
	fgSizer51->Add( m_staticText203, 0, wxALL, 5 );

	m_staticText204 = new wxStaticText( m_focuser_panel, wxID_ANY, _("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText204->Wrap( -1 );
	fgSizer51->Add( m_staticText204, 0, wxALL, 5 );

	m_staticText205 = new wxStaticText( m_focuser_panel, wxID_ANY, _("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText205->Wrap( -1 );
	fgSizer51->Add( m_staticText205, 0, wxALL, 5 );

	m_staticText206 = new wxStaticText( m_focuser_panel, wxID_ANY, _("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText206->Wrap( -1 );
	fgSizer51->Add( m_staticText206, 0, wxALL, 5 );

	m_staticText207 = new wxStaticText( m_focuser_panel, wxID_ANY, _("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText207->Wrap( -1 );
	fgSizer51->Add( m_staticText207, 0, wxALL, 5 );

	m_staticText208 = new wxStaticText( m_focuser_panel, wxID_ANY, _("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText208->Wrap( -1 );
	fgSizer51->Add( m_staticText208, 0, wxALL, 5 );

	m_staticText209 = new wxStaticText( m_focuser_panel, wxID_ANY, _("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText209->Wrap( -1 );
	fgSizer51->Add( m_staticText209, 0, wxALL, 5 );

	m_staticText210 = new wxStaticText( m_focuser_panel, wxID_ANY, _("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText210->Wrap( -1 );
	fgSizer51->Add( m_staticText210, 0, wxALL, 5 );

	m_staticText211 = new wxStaticText( m_focuser_panel, wxID_ANY, _("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText211->Wrap( -1 );
	fgSizer51->Add( m_staticText211, 0, wxALL, 5 );

	m_staticText212 = new wxStaticText( m_focuser_panel, wxID_ANY, _("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText212->Wrap( -1 );
	fgSizer51->Add( m_staticText212, 0, wxALL, 5 );

	m_staticText213 = new wxStaticText( m_focuser_panel, wxID_ANY, _("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText213->Wrap( -1 );
	fgSizer51->Add( m_staticText213, 0, wxALL, 5 );

	m_staticText214 = new wxStaticText( m_focuser_panel, wxID_ANY, _("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText214->Wrap( -1 );
	fgSizer51->Add( m_staticText214, 0, wxALL, 5 );

	m_staticText215 = new wxStaticText( m_focuser_panel, wxID_ANY, _("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText215->Wrap( -1 );
	fgSizer51->Add( m_staticText215, 0, wxALL, 5 );

	m_staticText216 = new wxStaticText( m_focuser_panel, wxID_ANY, _("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText216->Wrap( -1 );
	fgSizer51->Add( m_staticText216, 0, wxALL, 5 );

	m_staticText217 = new wxStaticText( m_focuser_panel, wxID_ANY, _("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText217->Wrap( -1 );
	fgSizer51->Add( m_staticText217, 0, wxALL, 5 );


	fgSizer52->Add( fgSizer51, 1, wxEXPAND, 5 );

	m_staticline28 = new wxStaticLine( m_focuser_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	fgSizer52->Add( m_staticline28, 0, wxEXPAND | wxALL, 5 );

	wxFlexGridSizer* fgSizer56;
	fgSizer56 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer56->SetFlexibleDirection( wxBOTH );
	fgSizer56->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText223 = new wxStaticText( m_focuser_panel, wxID_ANY, _("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText223->Wrap( -1 );
	fgSizer56->Add( m_staticText223, 0, wxALL, 5 );

	m_checkBox6 = new wxCheckBox( m_focuser_panel, wxID_ANY, _("Check Me!"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer56->Add( m_checkBox6, 0, wxALL, 5 );


	fgSizer52->Add( fgSizer56, 1, wxEXPAND, 5 );

	m_staticline29 = new wxStaticLine( m_focuser_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	fgSizer52->Add( m_staticline29, 0, wxEXPAND | wxALL, 5 );

	wxFlexGridSizer* fgSizer57;
	fgSizer57 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer57->SetFlexibleDirection( wxBOTH );
	fgSizer57->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText224 = new wxStaticText( m_focuser_panel, wxID_ANY, _("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText224->Wrap( -1 );
	fgSizer57->Add( m_staticText224, 0, wxALL, 5 );

	m_textCtrl59 = new wxTextCtrl( m_focuser_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	#ifdef __WXGTK__
	if ( !m_textCtrl59->HasFlag( wxTE_MULTILINE ) )
	{
	m_textCtrl59->SetMaxLength( 6 );
	}
	#else
	m_textCtrl59->SetMaxLength( 6 );
	#endif
	fgSizer57->Add( m_textCtrl59, 0, wxALL, 5 );


	fgSizer52->Add( fgSizer57, 1, wxEXPAND, 5 );

	wxFlexGridSizer* fgSizer55;
	fgSizer55 = new wxFlexGridSizer( 0, 4, 0, 0 );
	fgSizer55->SetFlexibleDirection( wxBOTH );
	fgSizer55->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_button31 = new wxButton( m_focuser_panel, wxID_ANY, _("<<"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer55->Add( m_button31, 0, wxALL, 5 );

	m_button32 = new wxButton( m_focuser_panel, wxID_ANY, _("<"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer55->Add( m_button32, 0, wxALL, 5 );

	m_button33 = new wxButton( m_focuser_panel, wxID_ANY, _(">"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer55->Add( m_button33, 0, wxALL, 5 );

	m_button34 = new wxButton( m_focuser_panel, wxID_ANY, _(">>"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer55->Add( m_button34, 0, wxALL, 5 );


	fgSizer52->Add( fgSizer55, 1, wxEXPAND, 5 );


	gSizer5->Add( fgSizer52, 1, wxEXPAND, 5 );

	wxFlexGridSizer* fgSizer53;
	fgSizer53 = new wxFlexGridSizer( 0, 1, 0, 0 );
	fgSizer53->SetFlexibleDirection( wxBOTH );
	fgSizer53->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText218 = new wxStaticText( m_focuser_panel, wxID_ANY, _("Focuser Settings"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText218->Wrap( -1 );
	fgSizer53->Add( m_staticText218, 0, wxALL, 5 );

	m_staticline27 = new wxStaticLine( m_focuser_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	fgSizer53->Add( m_staticline27, 0, wxEXPAND | wxALL, 5 );

	wxFlexGridSizer* fgSizer54;
	fgSizer54 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer54->SetFlexibleDirection( wxBOTH );
	fgSizer54->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText219 = new wxStaticText( m_focuser_panel, wxID_ANY, _("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText219->Wrap( -1 );
	fgSizer54->Add( m_staticText219, 0, wxALL, 5 );

	m_textCtrl55 = new wxTextCtrl( m_focuser_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer54->Add( m_textCtrl55, 0, wxALL, 5 );

	m_staticText220 = new wxStaticText( m_focuser_panel, wxID_ANY, _("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText220->Wrap( -1 );
	fgSizer54->Add( m_staticText220, 0, wxALL, 5 );

	m_textCtrl56 = new wxTextCtrl( m_focuser_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer54->Add( m_textCtrl56, 0, wxALL, 5 );

	m_staticText221 = new wxStaticText( m_focuser_panel, wxID_ANY, _("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText221->Wrap( -1 );
	fgSizer54->Add( m_staticText221, 0, wxALL, 5 );

	m_textCtrl57 = new wxTextCtrl( m_focuser_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer54->Add( m_textCtrl57, 0, wxALL, 5 );

	m_staticText222 = new wxStaticText( m_focuser_panel, wxID_ANY, _("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText222->Wrap( -1 );
	fgSizer54->Add( m_staticText222, 0, wxALL, 5 );

	m_textCtrl58 = new wxTextCtrl( m_focuser_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer54->Add( m_textCtrl58, 0, wxALL, 5 );


	fgSizer53->Add( fgSizer54, 1, wxEXPAND, 5 );


	gSizer5->Add( fgSizer53, 1, wxEXPAND, 5 );


	bSizer13->Add( gSizer5, 1, wxEXPAND, 5 );


	m_focuser_panel->SetSizer( bSizer13 );
	m_focuser_panel->Layout();
	bSizer13->Fit( m_focuser_panel );
	m_main_listbook->AddPage( m_focuser_panel, _("Focuser"), false );
	m_filterwheel_panel = new wxPanel( m_main_listbook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer16;
	bSizer16 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer121;
	bSizer121 = new wxBoxSizer( wxVERTICAL );

	wxGridSizer* gSizer31;
	gSizer31 = new wxGridSizer( 0, 2, 0, 0 );

	wxFlexGridSizer* fgSizer2211;
	fgSizer2211 = new wxFlexGridSizer( 0, 1, 0, 0 );
	fgSizer2211->SetFlexibleDirection( wxBOTH );
	fgSizer2211->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	wxFlexGridSizer* fgSizer2411;
	fgSizer2411 = new wxFlexGridSizer( 0, 5, 0, 0 );
	fgSizer2411->SetFlexibleDirection( wxBOTH );
	fgSizer2411->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText3211 = new wxStaticText( m_filterwheel_panel, wxID_ANY, _("Telescope"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3211->Wrap( -1 );
	fgSizer2411->Add( m_staticText3211, 0, wxALL, 5 );

	wxString m_choice411Choices[] = { _("Simulator") };
	int m_choice411NChoices = sizeof( m_choice411Choices ) / sizeof( wxString );
	m_choice411 = new wxChoice( m_filterwheel_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice411NChoices, m_choice411Choices, 0 );
	m_choice411->SetSelection( 0 );
	fgSizer2411->Add( m_choice411, 0, wxALL, 5 );

	m_bpButton211 = new wxBitmapButton( m_filterwheel_panel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|0 );
	fgSizer2411->Add( m_bpButton211, 0, wxALL, 5 );

	m_bpButton312 = new wxBitmapButton( m_filterwheel_panel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|0 );
	fgSizer2411->Add( m_bpButton312, 0, wxALL, 5 );

	m_bpButton411 = new wxBitmapButton( m_filterwheel_panel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|0 );
	fgSizer2411->Add( m_bpButton411, 0, wxALL, 5 );


	fgSizer2211->Add( fgSizer2411, 1, wxEXPAND, 5 );

	m_staticline1111 = new wxStaticLine( m_filterwheel_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	fgSizer2211->Add( m_staticline1111, 0, wxEXPAND | wxALL, 5 );

	wxGridBagSizer* gbSizer311;
	gbSizer311 = new wxGridBagSizer( 0, 0 );
	gbSizer311->SetFlexibleDirection( wxBOTH );
	gbSizer311->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText7111 = new wxStaticText( m_filterwheel_panel, wxID_ANY, _("Name"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText7111->Wrap( -1 );
	gbSizer311->Add( m_staticText7111, wxGBPosition( 0, 0 ), wxGBSpan( 1, 1 ), wxALL, 5 );

	m_staticText7211 = new wxStaticText( m_filterwheel_panel, wxID_ANY, _("Simulator"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText7211->Wrap( -1 );
	gbSizer311->Add( m_staticText7211, wxGBPosition( 0, 1 ), wxGBSpan( 1, 1 ), wxALL, 5 );


	fgSizer2211->Add( gbSizer311, 1, wxEXPAND, 5 );

	wxGridBagSizer* gbSizer511;
	gbSizer511 = new wxGridBagSizer( 0, 0 );
	gbSizer511->SetFlexibleDirection( wxBOTH );
	gbSizer511->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText6911 = new wxStaticText( m_filterwheel_panel, wxID_ANY, _("Description"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6911->Wrap( -1 );
	gbSizer511->Add( m_staticText6911, wxGBPosition( 0, 0 ), wxGBSpan( 1, 1 ), wxALL, 5 );

	m_staticText7011 = new wxStaticText( m_filterwheel_panel, wxID_ANY, _("A basic simulator telescope"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText7011->Wrap( -1 );
	gbSizer511->Add( m_staticText7011, wxGBPosition( 0, 1 ), wxGBSpan( 1, 1 ), wxALL, 5 );


	fgSizer2211->Add( gbSizer511, 1, wxEXPAND, 5 );

	m_staticline1211 = new wxStaticLine( m_filterwheel_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	fgSizer2211->Add( m_staticline1211, 0, wxEXPAND | wxALL, 5 );

	wxFlexGridSizer* fgSizer76;
	fgSizer76 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer76->SetFlexibleDirection( wxBOTH );
	fgSizer76->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText297 = new wxStaticText( m_filterwheel_panel, wxID_ANY, _("DriverVersion"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText297->Wrap( -1 );
	fgSizer76->Add( m_staticText297, 0, wxALL, 5 );

	m_staticText298 = new wxStaticText( m_filterwheel_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText298->Wrap( -1 );
	fgSizer76->Add( m_staticText298, 0, wxALL, 5 );

	m_staticText299 = new wxStaticText( m_filterwheel_panel, wxID_ANY, _("DrvierInfo"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText299->Wrap( -1 );
	fgSizer76->Add( m_staticText299, 0, wxALL, 5 );

	m_staticText300 = new wxStaticText( m_filterwheel_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText300->Wrap( -1 );
	fgSizer76->Add( m_staticText300, 0, wxALL, 5 );


	fgSizer2211->Add( fgSizer76, 1, wxEXPAND, 5 );

	m_staticline261 = new wxStaticLine( m_filterwheel_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	fgSizer2211->Add( m_staticline261, 0, wxEXPAND | wxALL, 5 );

	wxFlexGridSizer* fgSizer77;
	fgSizer77 = new wxFlexGridSizer( 0, 3, 0, 0 );
	fgSizer77->SetFlexibleDirection( wxBOTH );
	fgSizer77->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText306 = new wxStaticText( m_filterwheel_panel, wxID_ANY, _("Position"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText306->Wrap( -1 );
	fgSizer77->Add( m_staticText306, 0, wxALL, 5 );

	m_comboBox1 = new wxComboBox( m_filterwheel_panel, wxID_ANY, _("Combo!"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	fgSizer77->Add( m_comboBox1, 0, wxALL, 5 );

	m_button51 = new wxButton( m_filterwheel_panel, wxID_ANY, _("MoveTo"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer77->Add( m_button51, 0, wxALL, 5 );


	fgSizer2211->Add( fgSizer77, 1, wxEXPAND, 5 );


	gSizer31->Add( fgSizer2211, 1, wxEXPAND, 5 );

	wxFlexGridSizer* fgSizer78;
	fgSizer78 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer78->SetFlexibleDirection( wxBOTH );
	fgSizer78->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_grid3 = new wxGrid( m_filterwheel_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );

	// Grid
	m_grid3->CreateGrid( 5, 1 );
	m_grid3->EnableEditing( true );
	m_grid3->EnableGridLines( true );
	m_grid3->EnableDragGridSize( false );
	m_grid3->SetMargins( 0, 0 );

	// Columns
	m_grid3->EnableDragColMove( false );
	m_grid3->EnableDragColSize( true );
	m_grid3->SetColLabelAlignment( wxALIGN_CENTER, wxALIGN_CENTER );

	// Rows
	m_grid3->EnableDragRowSize( true );
	m_grid3->SetRowLabelAlignment( wxALIGN_CENTER, wxALIGN_CENTER );

	// Label Appearance

	// Cell Defaults
	m_grid3->SetDefaultCellAlignment( wxALIGN_LEFT, wxALIGN_TOP );
	fgSizer78->Add( m_grid3, 0, wxALL, 5 );


	gSizer31->Add( fgSizer78, 1, wxEXPAND, 5 );


	bSizer121->Add( gSizer31, 1, wxEXPAND, 5 );


	bSizer16->Add( bSizer121, 1, wxEXPAND, 5 );


	m_filterwheel_panel->SetSizer( bSizer16 );
	m_filterwheel_panel->Layout();
	bSizer16->Fit( m_filterwheel_panel );
	m_main_listbook->AddPage( m_filterwheel_panel, _("FilterWheel"), false );
	m_guider_panel = new wxPanel( m_main_listbook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_main_listbook->AddPage( m_guider_panel, _("Guider"), false );
	m_plugin_panel = new wxPanel( m_main_listbook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer20;
	bSizer20 = new wxBoxSizer( wxVERTICAL );

	m_listbook3 = new wxListbook( m_plugin_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLB_DEFAULT );
	m_plugin_installed_panel = new wxPanel( m_listbook3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer21;
	bSizer21 = new wxBoxSizer( wxVERTICAL );

	m_listbook4 = new wxListbook( m_plugin_installed_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLB_DEFAULT );
	m_pulgin_panel_openapt = new wxPanel( m_listbook4, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_listbook4->AddPage( m_pulgin_panel_openapt, _("OpenAPT"), false );
	#ifdef __WXGTK__ // Small icon style not supported in GTK
	wxListView* m_listbook4ListView = m_listbook4->GetListView();
	long m_listbook4Flags = m_listbook4ListView->GetWindowStyleFlag();
	if( m_listbook4Flags & wxLC_SMALL_ICON )
	{
		m_listbook4Flags = ( m_listbook4Flags & ~wxLC_SMALL_ICON ) | wxLC_ICON;
	}
	m_listbook4ListView->SetWindowStyleFlag( m_listbook4Flags );
	#endif

	bSizer21->Add( m_listbook4, 1, wxEXPAND | wxALL, 5 );


	m_plugin_installed_panel->SetSizer( bSizer21 );
	m_plugin_installed_panel->Layout();
	bSizer21->Fit( m_plugin_installed_panel );
	m_listbook3->AddPage( m_plugin_installed_panel, _("Installed"), true );
	m_plugin_download_panel = new wxPanel( m_listbook3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_listbook3->AddPage( m_plugin_download_panel, _("Download"), false );
	m_plugin_local_panel = new wxPanel( m_listbook3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer23;
	bSizer23 = new wxBoxSizer( wxVERTICAL );

	wxFlexGridSizer* fgSizer81;
	fgSizer81 = new wxFlexGridSizer( 0, 1, 0, 0 );
	fgSizer81->SetFlexibleDirection( wxBOTH );
	fgSizer81->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText307 = new wxStaticText( m_plugin_local_panel, wxID_ANY, _("Local Modules Installer"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText307->Wrap( -1 );
	fgSizer81->Add( m_staticText307, 0, wxALL, 5 );

	m_staticline44 = new wxStaticLine( m_plugin_local_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	fgSizer81->Add( m_staticline44, 0, wxEXPAND | wxALL, 5 );

	wxFlexGridSizer* fgSizer80;
	fgSizer80 = new wxFlexGridSizer( 0, 0, 0, 0 );
	fgSizer80->SetFlexibleDirection( wxBOTH );
	fgSizer80->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_button52 = new wxButton( m_plugin_local_panel, wxID_ANY, _("Refresh"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer80->Add( m_button52, 0, wxALL, 5 );

	m_searchCtrl1 = new wxSearchCtrl( m_plugin_local_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	#ifndef __WXMAC__
	m_searchCtrl1->ShowSearchButton( true );
	#endif
	m_searchCtrl1->ShowCancelButton( false );
	fgSizer80->Add( m_searchCtrl1, 0, wxALL, 5 );

	m_button53 = new wxButton( m_plugin_local_panel, wxID_ANY, _("Search"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer80->Add( m_button53, 0, wxALL, 5 );


	fgSizer81->Add( fgSizer80, 1, wxEXPAND, 5 );

	m_listbook5 = new wxListbook( m_plugin_local_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLB_DEFAULT );
	#ifdef __WXGTK__ // Small icon style not supported in GTK
	wxListView* m_listbook5ListView = m_listbook5->GetListView();
	long m_listbook5Flags = m_listbook5ListView->GetWindowStyleFlag();
	if( m_listbook5Flags & wxLC_SMALL_ICON )
	{
		m_listbook5Flags = ( m_listbook5Flags & ~wxLC_SMALL_ICON ) | wxLC_ICON;
	}
	m_listbook5ListView->SetWindowStyleFlag( m_listbook5Flags );
	#endif

	fgSizer81->Add( m_listbook5, 1, wxEXPAND | wxALL, 5 );


	bSizer23->Add( fgSizer81, 1, wxEXPAND, 5 );


	m_plugin_local_panel->SetSizer( bSizer23 );
	m_plugin_local_panel->Layout();
	bSizer23->Fit( m_plugin_local_panel );
	m_listbook3->AddPage( m_plugin_local_panel, _("Local"), false );
	#ifdef __WXGTK__ // Small icon style not supported in GTK
	wxListView* m_listbook3ListView = m_listbook3->GetListView();
	long m_listbook3Flags = m_listbook3ListView->GetWindowStyleFlag();
	if( m_listbook3Flags & wxLC_SMALL_ICON )
	{
		m_listbook3Flags = ( m_listbook3Flags & ~wxLC_SMALL_ICON ) | wxLC_ICON;
	}
	m_listbook3ListView->SetWindowStyleFlag( m_listbook3Flags );
	#endif

	bSizer20->Add( m_listbook3, 1, wxEXPAND | wxALL, 5 );


	m_plugin_panel->SetSizer( bSizer20 );
	m_plugin_panel->Layout();
	bSizer20->Fit( m_plugin_panel );
	m_main_listbook->AddPage( m_plugin_panel, _("Plugins"), false );
	m_server_panel = new wxPanel( m_main_listbook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* m_server_box;
	m_server_box = new wxBoxSizer( wxVERTICAL );


	m_server_panel->SetSizer( m_server_box );
	m_server_panel->Layout();
	m_server_box->Fit( m_server_panel );
	m_main_listbook->AddPage( m_server_panel, _("Server"), false );
	m_settings_panel = new wxPanel( m_main_listbook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* m_settings_box;
	m_settings_box = new wxBoxSizer( wxVERTICAL );


	m_settings_panel->SetSizer( m_settings_box );
	m_settings_panel->Layout();
	m_settings_box->Fit( m_settings_panel );
	m_main_listbook->AddPage( m_settings_panel, _("Settings"), false );
	#ifdef __WXGTK__ // Small icon style not supported in GTK
	wxListView* m_main_listbookListView = m_main_listbook->GetListView();
	long m_main_listbookFlags = m_main_listbookListView->GetWindowStyleFlag();
	if( m_main_listbookFlags & wxLC_SMALL_ICON )
	{
		m_main_listbookFlags = ( m_main_listbookFlags & ~wxLC_SMALL_ICON ) | wxLC_ICON;
	}
	m_main_listbookListView->SetWindowStyleFlag( m_main_listbookFlags );
	#endif


	m_mgr.Update();
	this->Centre( wxBOTH );

	// Connect Events
	m_camera_settings_btn->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DeviceDialog::on_camera_settings_click ), NULL, this );
	m_camera_connect_btn->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DeviceDialog::on_camera_connect_click ), NULL, this );
	m_camera_scan_btn->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DeviceDialog::on_camera_scan_click ), NULL, this );

	return true;
}

DeviceDialog::~DeviceDialog()
{
	// Disconnect Events
	m_camera_settings_btn->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DeviceDialog::on_camera_settings_click ), NULL, this );
	m_camera_connect_btn->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DeviceDialog::on_camera_connect_click ), NULL, this );
	m_camera_scan_btn->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DeviceDialog::on_camera_scan_click ), NULL, this );

	m_mgr.UnInit();

}
