///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 3.10.1-0-g8feb16b3)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "lightguider.h"
#include "server_dialog_base.h"

///////////////////////////////////////////////////////////////////////////
ServerDialog::ServerDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* m_dialog_box;
	m_dialog_box = new wxBoxSizer( wxVERTICAL );

	wxFlexGridSizer* m_dialog_sizer;
	m_dialog_sizer = new wxFlexGridSizer( 0, 1, 0, 0 );
	m_dialog_sizer->SetFlexibleDirection( wxBOTH );
	m_dialog_sizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_server_title = new wxStaticText( this, wxID_ANY, _("Socket Server"), wxDefaultPosition, wxDefaultSize, 0 );
	m_server_title->Wrap( -1 );
	m_dialog_sizer->Add( m_server_title, 0, wxALL, 5 );

	wxFlexGridSizer* m_server_settings_sizer;
	m_server_settings_sizer = new wxFlexGridSizer( 0, 4, 0, 0 );
	m_server_settings_sizer->SetFlexibleDirection( wxBOTH );
	m_server_settings_sizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_server_host_t = new wxStaticText( this, wxID_ANY, _("Host:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_server_host_t->Wrap( -1 );
	m_server_settings_sizer->Add( m_server_host_t, 0, wxALL, 5 );

	m_server_host = new wxTextCtrl( this, wxID_ANY, _("0.0.0.0"), wxDefaultPosition, wxDefaultSize, 0 );
	m_server_settings_sizer->Add( m_server_host, 0, wxALL, 5 );

	m_server_port_t = new wxStaticText( this, wxID_ANY, _("Port:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_server_port_t->Wrap( -1 );
	m_server_settings_sizer->Add( m_server_port_t, 0, wxALL, 5 );

	m_server_port = new wxTextCtrl( this, wxID_ANY, _("4400"), wxDefaultPosition, wxDefaultSize, 0 );
	#ifdef __WXGTK__
	if ( !m_server_port->HasFlag( wxTE_MULTILINE ) )
	{
	m_server_port->SetMaxLength( 4 );
	}
	#else
	m_server_port->SetMaxLength( 4 );
	#endif
	m_server_settings_sizer->Add( m_server_port, 0, wxALL, 5 );


	m_dialog_sizer->Add( m_server_settings_sizer, 1, wxEXPAND, 5 );

	wxFlexGridSizer* m_server_control_sizer;
	m_server_control_sizer = new wxFlexGridSizer( 0, 4, 0, 0 );
	m_server_control_sizer->SetFlexibleDirection( wxBOTH );
	m_server_control_sizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_server_start = new wxButton( this, wxID_ANY, _("Start"), wxDefaultPosition, wxDefaultSize, 0 );
	m_server_start->SetToolTip( _("Start socket server on specific host and port") );
	m_server_start->SetHelpText( _("Start socket server on specific host and port , default host is 0.0.0.0 which allow all ips to connect to server ,and port is 4400 the same as PHD2.") );

	m_server_control_sizer->Add( m_server_start, 0, wxALL, 5 );

	m_server_stop = new wxButton( this, wxID_ANY, _("Stop"), wxDefaultPosition, wxDefaultSize, 0 );
	m_server_stop->Enable( false );
	m_server_stop->SetToolTip( _("Stop server") );

	m_server_control_sizer->Add( m_server_stop, 0, wxALL, 5 );

	m_server_config = new wxButton( this, wxID_ANY, _("Config"), wxDefaultPosition, wxDefaultSize, 0 );
	m_server_config->SetToolTip( _("Detailed server configuration") );

	m_server_control_sizer->Add( m_server_config, 0, wxALL, 5 );

	m_server_open = new wxButton( this, wxID_ANY, _("Open"), wxDefaultPosition, wxDefaultSize, 0 );
	m_server_open->Enable( false );

	m_server_control_sizer->Add( m_server_open, 0, wxALL, 5 );


	m_dialog_sizer->Add( m_server_control_sizer, 1, wxEXPAND, 5 );

	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	m_dialog_sizer->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );

	m_webserver_title = new wxStaticText( this, wxID_ANY, _("Web Server"), wxDefaultPosition, wxDefaultSize, 0 );
	m_webserver_title->Wrap( -1 );
	m_dialog_sizer->Add( m_webserver_title, 0, wxALL, 5 );

	wxFlexGridSizer* m_webserver_settings_sizer;
	m_webserver_settings_sizer = new wxFlexGridSizer( 0, 4, 0, 0 );
	m_webserver_settings_sizer->SetFlexibleDirection( wxBOTH );
	m_webserver_settings_sizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_webserver_host_t = new wxStaticText( this, wxID_ANY, _("Host:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_webserver_host_t->Wrap( -1 );
	m_webserver_settings_sizer->Add( m_webserver_host_t, 0, wxALL, 5 );

	m_webserver_host = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_webserver_settings_sizer->Add( m_webserver_host, 0, wxALL, 5 );

	m_webserver_port_t = new wxStaticText( this, wxID_ANY, _("Port:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_webserver_port_t->Wrap( -1 );
	m_webserver_settings_sizer->Add( m_webserver_port_t, 0, wxALL, 5 );

	m_webserver_port = new wxTextCtrl( this, wxID_ANY, _("5000"), wxDefaultPosition, wxDefaultSize, 0 );
	#ifdef __WXGTK__
	if ( !m_webserver_port->HasFlag( wxTE_MULTILINE ) )
	{
	m_webserver_port->SetMaxLength( 4 );
	}
	#else
	m_webserver_port->SetMaxLength( 4 );
	#endif
	m_webserver_settings_sizer->Add( m_webserver_port, 0, wxALL, 5 );

	m_webserver_ssl_t = new wxStaticText( this, wxID_ANY, _("SSL"), wxDefaultPosition, wxDefaultSize, 0 );
	m_webserver_ssl_t->Wrap( -1 );
	m_webserver_settings_sizer->Add( m_webserver_ssl_t, 0, wxALL, 5 );

	m_server_ssl = new wxCheckBox( this, wxID_ANY, _("Enable"), wxDefaultPosition, wxDefaultSize, 0 );
	m_webserver_settings_sizer->Add( m_server_ssl, 0, wxALL, 5 );

	m_webserver_mod_t = new wxStaticText( this, wxID_ANY, _("Loader"), wxDefaultPosition, wxDefaultSize, 0 );
	m_webserver_mod_t->Wrap( -1 );
	m_webserver_settings_sizer->Add( m_webserver_mod_t, 0, wxALL, 5 );

	m_webserver_mod = new wxCheckBox( this, wxID_ANY, _("Enable"), wxDefaultPosition, wxDefaultSize, 0 );
	m_webserver_mod->SetValue(true);
	m_webserver_settings_sizer->Add( m_webserver_mod, 0, wxALL, 5 );


	m_dialog_sizer->Add( m_webserver_settings_sizer, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );

	m_server_ssl_settings_panel = new wxCollapsiblePane( this, wxID_ANY, _("SSL Settings"), wxDefaultPosition, wxDefaultSize, wxCP_DEFAULT_STYLE );
	m_server_ssl_settings_panel->Collapse( true );

	wxFlexGridSizer* m_server_ssl_settings_sizer;
	m_server_ssl_settings_sizer = new wxFlexGridSizer( 0, 2, 0, 0 );
	m_server_ssl_settings_sizer->SetFlexibleDirection( wxBOTH );
	m_server_ssl_settings_sizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_server_ssl_key_t = new wxStaticText( m_server_ssl_settings_panel->GetPane(), wxID_ANY, _("Key:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_server_ssl_key_t->Wrap( -1 );
	m_server_ssl_settings_sizer->Add( m_server_ssl_key_t, 0, wxALL, 5 );

	m_server_ssl_key = new wxFilePickerCtrl( m_server_ssl_settings_panel->GetPane(), wxID_ANY, wxEmptyString, _("Select a SSL key file"), _("*.key*"), wxDefaultPosition, wxDefaultSize, wxFLP_DEFAULT_STYLE );
	m_server_ssl_settings_sizer->Add( m_server_ssl_key, 0, wxALL, 5 );

	m_server_ssl_cert_t = new wxStaticText( m_server_ssl_settings_panel->GetPane(), wxID_ANY, _("Cert"), wxDefaultPosition, wxDefaultSize, 0 );
	m_server_ssl_cert_t->Wrap( -1 );
	m_server_ssl_settings_sizer->Add( m_server_ssl_cert_t, 0, wxALL, 5 );

	m_server_ssl_cert = new wxFilePickerCtrl( m_server_ssl_settings_panel->GetPane(), wxID_ANY, wxEmptyString, _("Select a SSL cert file"), _("*.cert*"), wxDefaultPosition, wxDefaultSize, wxFLP_DEFAULT_STYLE );
	m_server_ssl_settings_sizer->Add( m_server_ssl_cert, 0, wxALL, 5 );


	m_server_ssl_settings_panel->GetPane()->SetSizer( m_server_ssl_settings_sizer );
	m_server_ssl_settings_panel->GetPane()->Layout();
	m_server_ssl_settings_sizer->Fit( m_server_ssl_settings_panel->GetPane() );
	bSizer2->Add( m_server_ssl_settings_panel, 1, wxEXPAND | wxALL, 5 );


	m_dialog_sizer->Add( bSizer2, 1, wxEXPAND, 5 );

	wxFlexGridSizer* m_webserver_control_sizer;
	m_webserver_control_sizer = new wxFlexGridSizer( 0, 4, 0, 0 );
	m_webserver_control_sizer->AddGrowableCol( 2 );
	m_webserver_control_sizer->AddGrowableRow( 2 );
	m_webserver_control_sizer->SetFlexibleDirection( wxBOTH );
	m_webserver_control_sizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_webserver_start = new wxButton( this, wxID_ANY, _("Start"), wxDefaultPosition, wxDefaultSize, 0 );
	m_webserver_control_sizer->Add( m_webserver_start, 0, wxALL, 5 );

	m_webserver_stop = new wxButton( this, wxID_ANY, _("Stop"), wxDefaultPosition, wxDefaultSize, 0 );
	m_webserver_stop->SetToolTip( _("Stop Server") );

	m_webserver_control_sizer->Add( m_webserver_stop, 0, wxALL, 5 );


	m_dialog_sizer->Add( m_webserver_control_sizer, 1, wxEXPAND, 5 );

	m_staticline2 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	m_dialog_sizer->Add( m_staticline2, 0, wxEXPAND | wxALL, 5 );


	m_dialog_box->Add( m_dialog_sizer, 1, wxEXPAND, 5 );


	this->SetSizer( m_dialog_box );
	this->Layout();
	m_dialog_box->Fit( this );

	this->Centre( wxBOTH );

	// Connect Events
	m_server_start->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ServerDialog::start_server ), NULL, this );
	m_server_stop->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ServerDialog::stop_server ), NULL, this );
	m_server_config->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ServerDialog::config_server ), NULL, this );
	m_server_open->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ServerDialog::open_server ), NULL, this );
	m_webserver_start->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ServerDialog::start_webserver ), NULL, this );
	m_webserver_stop->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ServerDialog::stop_webserver ), NULL, this );
}

ServerDialog::~ServerDialog()
{
	// Disconnect Events
	m_server_start->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ServerDialog::start_server ), NULL, this );
	m_server_stop->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ServerDialog::stop_server ), NULL, this );
	m_server_config->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ServerDialog::config_server ), NULL, this );
	m_server_open->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ServerDialog::open_server ), NULL, this );
	m_webserver_start->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ServerDialog::start_webserver ), NULL, this );
	m_webserver_stop->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ServerDialog::stop_webserver ), NULL, this );

}