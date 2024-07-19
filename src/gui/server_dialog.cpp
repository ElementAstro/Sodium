#include "server_dialog.h"

LightGuiderServerDialog::LightGuiderServerDialog(wxWindow* parent)
    : ServerDialog(parent) {}

LightGuiderServerDialog::~LightGuiderServerDialog() {
    pFrame->pServerAssistant = nullptr;
}

void LightGuiderServerDialog::start_server(wxCommandEvent& event) {
    // TODO: Implement start_server
}

void LightGuiderServerDialog::stop_server(wxCommandEvent& event) {
    // TODO: Implement stop_server
}

void LightGuiderServerDialog::config_server(wxCommandEvent& event) {
    // TODO: Implement config_server
}

void LightGuiderServerDialog::open_server(wxCommandEvent& event) {
    // TODO: Implement open_server
}

void LightGuiderServerDialog::start_webserver(wxCommandEvent& event) {
    // TODO: Implement start_webserver
}

void LightGuiderServerDialog::stop_webserver(wxCommandEvent& event) {
    // TODO: Implement stop_webserver
}

wxDialog* ServerAssistantFactory::MakeServerAssistant() {
    return new LightGuiderServerDialog(pFrame);
}
