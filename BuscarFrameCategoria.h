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
    void OnBuscar(wxCommandEvent& event);      // Evento para realizar la búsqueda
    void OnCancelar(wxCommandEvent& event);    // Evento para cancelar la operación y cerrar la ventana
    void OnClose(wxCloseEvent& event);         // Evento para cerrar la ventana
    void OnGridCellDClick(wxGridEvent& event); // Evento para doble clic en una celda de la grilla
    void OnGridSelectCell(wxGridEvent& event); // Evento para la selección de celdas

    // Database Interaction
    bool ConectarBaseDeDatos();    // Método para conectar a la base de datos
    void BuscarOperacionesEnBaseDeDatos(const wxString& operacion, const wxString& descripcion); // Buscar por operación y descripción
    void CerrarConexion();         // Método para cerrar la conexión a la base de datos
    void OnKeyDown(wxKeyEvent& event);

    // Utility Methods
    void UpdateRowLabels(int selectedRow);  // Método para actualizar las etiquetas de las filas

    // UI Components
    wxTextCtrl* txtOperacion;   // Campo para buscar por número de Operacion
    wxTextCtrl* txtDescripcion; // Campo para buscar por Descripción
    wxButton* btnBuscar;        // Botón de búsqueda
    wxButton* btnCancelar;      // Botón de cancelar
    wxGrid* grid;               // Grilla para mostrar resultados

    // Database Handles
    SQLHENV hEnv;               // Manejador de entorno para ODBC
    SQLHDBC hDbc;               // Manejador de conexión a la base de datos

    // Parent frame reference
    wxWindow* mainFrameParent;   // Referencia al marco principal

    // Nuevo método para actualizar los botones de navegación en el frame principal
    void ActualizarEstadoBotonesNavegacionCategoria();

    wxDECLARE_EVENT_TABLE();
};
