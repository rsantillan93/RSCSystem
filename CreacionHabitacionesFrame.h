#pragma once

#include <wx/wx.h>
#include <sqlext.h>
#include "MainFrame.h"
#include "BuscarCreacionFrame.h"

class CreacionHabitacionesFrame : public wxFrame {
public:
    CreacionHabitacionesFrame(const wxString& title, const wxPoint& pos, const wxSize& size, MainFrame* parent, const wxString& usuarioAutenticado);
    ~CreacionHabitacionesFrame();

    bool HayRegistrosEnTabla();
    void SetIDHab(const wxString& idHab); // Método para actualizar el ID de la habitación desde otras ventanas
    void OnEditar(wxCommandEvent& event);

    // Métodos públicos para acceder a los miembros necesarios desde BuscarCreacionFrame
    wxTextCtrl* GetTxtNroOperacion() { return txtNroOperacion; }
    wxButton* GetBtnIzquierda() { return btnIzquierda; }
    wxButton* GetBtnDerecha() { return btnDerecha; }

    // Aseguramos que los métodos ObtenerUltimoIDHab y ObtenerPrimerIDHab sean accesibles
    int ObtenerUltimoIDHab();
    int ObtenerPrimerIDHab();

private:
    // Event Handlers
    void OnNuevo(wxCommandEvent& event);
    void OnGuardar(wxCommandEvent& event);
    void OnCancelar(wxCommandEvent& event);
    void OnEliminar(wxCommandEvent& event);
    void OnBuscar(wxCommandEvent& event);
    void OnIzquierda(wxCommandEvent& event);
    void OnDerecha(wxCommandEvent& event);
    void OnExportarExcel(wxCommandEvent& event);
    void OnClose(wxCloseEvent& event);
    bool IDHabExists(int idHab);
    void OnKeyDown(wxKeyEvent& event);

    // Nueva función para verificar si el OperacionID ya existe en la base de datos
    bool OperacionIDExists(int operacionID);

    // Database Interaction
    bool ConectarBaseDeDatos();
    void CerrarConexion();
    int ObtenerSiguienteOperacionID();
    int ObtenerIDHabAnterior(int currentID);
    int ObtenerIDHabSiguiente(int currentID);
    void CargarDatosIDHab(int operacionID);
    void CargarUltimoRegistro();

    // Valores originales para restaurar en caso de cancelar o error
    int originalNroOperacion;
    wxString originalIDHab;
    wxString originalDescripcion;
    wxString originalUM;
    wxString usuarioAutenticado;

    // Utility Methods
    void EnableInputs(bool enable);
    void ClearInputs();
    void UpdateButtonStates(bool nuevoSelected, bool editarSelected);
    bool ValidateInputs();

    // UI Components
    wxTextCtrl* txtNroOperacion;
    wxTextCtrl* txtIDHab;
    wxTextCtrl* txtDescripcion;
    wxTextCtrl* txtUM;
    wxButton* btnNuevo;
    wxButton* btnGuardar;
    wxButton* btnEditar;
    wxButton* btnCancelar;
    wxButton* btnEliminar;
    wxButton* btnBuscar;
    wxButton* btnIzquierda;
    wxButton* btnDerecha;
    wxButton* btnExportar;
    wxFont defaultFont;

    // Database Handles
    SQLHENV hEnv;
    SQLHDBC hDbc;
    bool conexionAbierta;
    bool isEditing;
    bool isNewOperation;

    // Parent frame reference
    MainFrame* mainFrameParent;

    wxDECLARE_EVENT_TABLE();
};
