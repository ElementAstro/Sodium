#include "solver_dialog.h"

LightGuiderSolverAssistant::LightGuiderSolverAssistant( wxWindow* parent )
:
SolverAssistant( parent )
{

}

void LightGuiderSolverAssistant::on_solver_changed( wxFileDirPickerEvent& event )
{
// TODO: Implement on_solver_changed
}

void LightGuiderSolverAssistant::on_solver_search( wxCommandEvent& event )
{
// TODO: Implement on_solver_search
}

void LightGuiderSolverAssistant::refresh_solver_ra( wxMouseEvent& event )
{
// TODO: Implement refresh_solver_ra
}

void LightGuiderSolverAssistant::on_ra_enter( wxCommandEvent& event )
{
// TODO: Implement on_ra_enter
}

void LightGuiderSolverAssistant::refresh_solver_dec( wxMouseEvent& event )
{
// TODO: Implement refresh_solver_dec
}

void LightGuiderSolverAssistant::on_dec_enter( wxCommandEvent& event )
{
// TODO: Implement on_dec_enter
}

void LightGuiderSolverAssistant::on_fov_enter( wxCommandEvent& event )
{
// TODO: Implement on_fov_enter
}

void LightGuiderSolverAssistant::check_template_downloaded( wxCommandEvent& event )
{
// TODO: Implement check_template_downloaded
}

void LightGuiderSolverAssistant::on_refresh_template( wxCommandEvent& event )
{
// TODO: Implement on_refresh_template
}

void LightGuiderSolverAssistant::on_download_template( wxCommandEvent& event )
{
// TODO: Implement on_download_template
}

void LightGuiderSolverAssistant::on_open_template_folder( wxCommandEvent& event )
{
// TODO: Implement on_open_template_folder
}

void LightGuiderSolverAssistant::on_start_solver( wxCommandEvent& event )
{
// TODO: Implement on_start_solver
}

void LightGuiderSolverAssistant::on_stop_solver( wxCommandEvent& event )
{
// TODO: Implement on_stop_solver
}

void LightGuiderSolverAssistant::on_copy_solver( wxCommandEvent& event )
{
// TODO: Implement on_copy_solver
}

void LightGuiderSolverAssistant::on_exit_solver( wxCommandEvent& event )
{
// TODO: Implement on_exit_solver
}

void LightGuiderSolverAssistant::on_help_solver( wxCommandEvent& event )
{
// TODO: Implement on_help_solver
}

void LightGuiderSolverAssistant::on_copy_solver_result_ra( wxMouseEvent& event )
{
// TODO: Implement on_copy_solver_result_ra
}

void LightGuiderSolverAssistant::on_copy_solver_result_dec( wxMouseEvent& event )
{
// TODO: Implement on_copy_solver_result_dec
}

void LightGuiderSolverAssistant::on_copy_solver_result_rotation( wxMouseEvent& event )
{
// TODO: Implement on_copy_solver_result_rotation
}

void LightGuiderSolverAssistant::on_copy_solver_result_fovx( wxMouseEvent& event )
{
// TODO: Implement on_copy_solver_result_fovx
}

void LightGuiderSolverAssistant::on_copy_solver_result_fovy( wxMouseEvent& event )
{
// TODO: Implement on_copy_solver_result_fovy
}

wxDialog *SolverAssistantFactory::MakeSolverAssistant()
{
    return new LightGuiderSolverAssistant(pFrame);
}

