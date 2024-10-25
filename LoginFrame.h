// LoginFrame.h
#pragma once
#include <wx/wx.h>
#include <windows.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <sql.h>
#include <atomic>
#include <thread>
#include "MainFrame.h"

class LoginFrame : public wxFrame
{
public:
    LoginFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
    ~LoginFrame();

private:
    // Event handlers
    void OnIngresar(wxCommandEvent& event);
    void OnEnterPressed(wxCommandEvent& event);
    void OnCerrar(wxCommandEvent& event);

    // Authentication and database connection
    void AutenticarUsuarioEnHilo(const wxString& usuario, const wxString& contrasena);
    void IniciarConexionEnHilo();
    bool ConectarBaseDeDatos();
    void CerrarConexion();
    bool AutenticarUsuario(SQLHDBC hDbc, const wxString& usuario, const wxString& contrasena);

    // Database handles
    SQLHENV hEnv;       // Environment handle
    SQLHDBC hDbc;       // Connection handle
    std::atomic<bool> conexionAbierta; // Connection state (atomic for synchronization)

    // UI components
    wxTextCtrl* txtUsuario;          // Text field for Username
    wxTextCtrl* txtContrasena;       // Text field for Password
    wxButton* btnIngresar;           // Button to trigger login
    wxButton* btnCerrar;             // Button to close the application

    wxFont defaultFont; // Font for the entire system

    wxDECLARE_EVENT_TABLE();
};
