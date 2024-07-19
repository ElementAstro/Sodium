#include "server_dialog.hpp"

SodiumServerDialog::SodiumServerDialog(wxWindow* parent)
    : ServerDialog(parent) {}

SodiumServerDialog::~SodiumServerDialog() {
    pFrame->pServerAssistant = nullptr;
}

void SodiumServerDialog::start_server(wxCommandEvent& event) {
    // TODO: Implement start_server
}

void SodiumServerDialog::stop_server(wxCommandEvent& event) {
    // TODO: Implement stop_server
}

void SodiumServerDialog::config_server(wxCommandEvent& event) {
    // TODO: Implement config_server
}

void SodiumServerDialog::open_server(wxCommandEvent& event) {
    // TODO: Implement open_server
}

void SodiumServerDialog::start_webserver(wxCommandEvent& event) {
    // TODO: Implement start_webserver
}

void SodiumServerDialog::stop_webserver(wxCommandEvent& event) {
    // TODO: Implement stop_webserver
}

wxDialog* ServerAssistantFactory::MakeServerAssistant() {
    return new SodiumServerDialog(pFrame);
}
