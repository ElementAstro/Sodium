#include "SodiumDeviceDialog.hpp"

SodiumDeviceDialog::SodiumDeviceDialog(wxWindow* parent)
    : DeviceDialog(parent) {}

void SodiumDeviceDialog::on_camera_settings_click(wxCommandEvent& event) {
    // TODO: Implement on_camera_settings_click
}

void SodiumDeviceDialog::on_camera_connect_click(wxCommandEvent& event) {
    // TODO: Implement on_camera_connect_click
}

void SodiumDeviceDialog::on_camera_scan_click(wxCommandEvent& event) {
    // TODO: Implement on_camera_scan_click
}

wxDialog* SodiumDeviceDialogFactory::MakeDeviceDialog() {
    return new SodiumDeviceDialog(pFrame);
}