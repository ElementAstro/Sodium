#include "solver_dialog.hpp"

SodiumSolverAssistant::SodiumSolverAssistant(wxWindow* parent)
    : SolverAssistant(parent) {}

void SodiumSolverAssistant::on_solver_changed(
    wxFileDirPickerEvent& event) {
    // TODO: Implement on_solver_changed
}

void SodiumSolverAssistant::on_solver_search(wxCommandEvent& event) {
    // TODO: Implement on_solver_search
}

void SodiumSolverAssistant::refresh_solver_ra(wxMouseEvent& event) {
    // TODO: Implement refresh_solver_ra
}

void SodiumSolverAssistant::on_ra_enter(wxCommandEvent& event) {
    // TODO: Implement on_ra_enter
}

void SodiumSolverAssistant::refresh_solver_dec(wxMouseEvent& event) {
    // TODO: Implement refresh_solver_dec
}

void SodiumSolverAssistant::on_dec_enter(wxCommandEvent& event) {
    // TODO: Implement on_dec_enter
}

void SodiumSolverAssistant::on_fov_enter(wxCommandEvent& event) {
    // TODO: Implement on_fov_enter
}

void SodiumSolverAssistant::check_template_downloaded(
    wxCommandEvent& event) {
    // TODO: Implement check_template_downloaded
}

void SodiumSolverAssistant::on_refresh_template(wxCommandEvent& event) {
    // TODO: Implement on_refresh_template
}

void SodiumSolverAssistant::on_download_template(wxCommandEvent& event) {
    // TODO: Implement on_download_template
}

void SodiumSolverAssistant::on_open_template_folder(
    wxCommandEvent& event) {
    // TODO: Implement on_open_template_folder
}

void SodiumSolverAssistant::on_start_solver(wxCommandEvent& event) {
    // TODO: Implement on_start_solver
}

void SodiumSolverAssistant::on_stop_solver(wxCommandEvent& event) {
    // TODO: Implement on_stop_solver
}

void SodiumSolverAssistant::on_copy_solver(wxCommandEvent& event) {
    // TODO: Implement on_copy_solver
}

void SodiumSolverAssistant::on_exit_solver(wxCommandEvent& event) {
    // TODO: Implement on_exit_solver
}

void SodiumSolverAssistant::on_help_solver(wxCommandEvent& event) {
    // TODO: Implement on_help_solver
}

void SodiumSolverAssistant::on_copy_solver_result_ra(wxMouseEvent& event) {
    // TODO: Implement on_copy_solver_result_ra
}

void SodiumSolverAssistant::on_copy_solver_result_dec(
    wxMouseEvent& event) {
    // TODO: Implement on_copy_solver_result_dec
}

void SodiumSolverAssistant::on_copy_solver_result_rotation(
    wxMouseEvent& event) {
    // TODO: Implement on_copy_solver_result_rotation
}

void SodiumSolverAssistant::on_copy_solver_result_fovx(
    wxMouseEvent& event) {
    // TODO: Implement on_copy_solver_result_fovx
}

void SodiumSolverAssistant::on_copy_solver_result_fovy(
    wxMouseEvent& event) {
    // TODO: Implement on_copy_solver_result_fovy
}

wxDialog* SolverAssistantFactory::MakeSolverAssistant() {
    return new SodiumSolverAssistant(pFrame);
}
