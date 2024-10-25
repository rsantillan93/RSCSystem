#pragma once

#include <wx/wx.h>
#include <wx/grid.h>
#include <sqlext.h>

class BuscarFrameCategoria : public wxFrame {
public:
    BuscarFrameCategoria(wxWindow* parent, const wxString& title, const wxPoint& pos, const wxSize& size);
    ~BuscarFrameCategoria();

private:
    // Event Handlers
    void OnBuscar(wxCommandEvent& event);      // Evento para realizar la b�squeda
    void OnCancelar(wxCommandEvent& event);    // Evento para cancelar la operaci�n y cerrar la ventana
    void OnClose(wxCloseEvent& event);         // Evento para cerrar la ventana
    void OnGridCellDClick(wxGridEvent& event); // Evento para doble clic en una celda de la grilla
    void OnGridSelectCell(wxGridEvent& event); // Evento para la selecci�n de celdas

    // Database Interaction
    bool ConectarBaseDeDatos();    // M�todo para conectar a la base de datos
    void BuscarOperacionesEnBaseDeDatos(const wxString& operacion, const wxString& descripcion); // Buscar por operaci�n y descripci�n
    void CerrarConexion();         // M�todo para cerrar la conexi�n a la base de datos
    void OnKeyDown(wxKeyEvent& event);

    // Utility Methods
    void UpdateRowLabels(int selectedRow);  // M�todo para actualizar las etiquetas de las filas

    // UI Components
    wxTextCtrl* txtOperacion;   // Campo para buscar por n�mero de Operacion
    wxTextCtrl* txtDescripcion; // Campo para buscar por Descripci�n
    wxButton* btnBuscar;        // Bot�n de b�squeda
    wxButton* btnCancelar;      // Bot�n de cancelar
    wxGrid* grid;               // Grilla para mostrar resultados

    // Database Handles
    SQLHENV hEnv;               // Manejador de entorno para ODBC
    SQLHDBC hDbc;               // Manejador de conexi�n a la base de datos

    // Parent frame reference
    wxWindow* mainFrameParent;   // Referencia al marco principal

    // Nuevo m�todo para actualizar los botones de navegaci�n en el frame principal
    void ActualizarEstadoBotonesNavegacionCategoria();

    wxDECLARE_EVENT_TABLE();
};
