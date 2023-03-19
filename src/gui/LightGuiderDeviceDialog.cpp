#include "LightGuiderDeviceDialog.h"

LightGuiderDeviceDialog::LightGuiderDeviceDialog( wxWindow* parent )
:
DeviceDialog( parent )
{

}

void LightGuiderDeviceDialog::on_camera_settings_click( wxCommandEvent& event )
{
// TODO: Implement on_camera_settings_click
}

void LightGuiderDeviceDialog::on_camera_connect_click( wxCommandEvent& event )
{
// TODO: Implement on_camera_connect_click
}

void LightGuiderDeviceDialog::on_camera_scan_click( wxCommandEvent& event )
{
// TODO: Implement on_camera_scan_click
}

wxDialog *LightGuiderDeviceDialogFactory::MakeDeviceDialog()
{
    return new LightGuiderDeviceDialog(pFrame);
}