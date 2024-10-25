// BuscarFrameHabitacion.h
#pragma once

#include <wx/wx.h>
#include <wx/grid.h>
#include <sqlext.h>  // Incluye los tipos y funciones necesarias para ODBC

class BuscarFrameHabitacion : public wxFrame
{
public:
    // Constructor y destructor
    BuscarFrameHabitacion(wxWindow* parent, const wxString& title, const wxPoint& pos, const wxSize& size);
    ~BuscarFrameHabitacion();  // Destructor definido en el .cpp

    // M�todo p�blico para permitir edici�n desde la ventana principal
    void OnEditar(wxCommandEvent& event);

private:
    // Manejadores de eventos
    void OnBuscar(wxCommandEvent& event);
    void OnCancelar(wxCommandEvent& event);
    void OnClose(wxCloseEvent& event);  // Manejador para cierre de ventana
    void OnGridSelectCell(wxGridEvent& event);
    void OnGridCellDClick(wxGridEvent& event);  // Manejador para doble clic en celda
    void OnKeyDown(wxKeyEvent& event);

    // M�todo para actualizar la navegaci�n en CreacionHabitacionesFrame
    void ActualizarNavegacionEnHabitacionFrame();

    // Funcionalidad interna
    void BuscarOperacionesEnBaseDeDatos(const wxString& nroOperacion, const wxString& descripcion);
    void UpdateRowLabels(int selectedRow);
    bool ConectarBaseDeDatos();  // Conectar con la base de datos
    void CerrarConexion();  // Cerrar y liberar la conexi�n

    // Componentes de la interfaz de usuario
    wxTextCtrl* txtNroOperacion;
    wxTextCtrl* txtDescripcion;
    wxGrid* grid;
    wxButton* btnBuscar;
    wxButton* btnCancelar;
    wxWindow* mainFrameParent;  // Referencia a la ventana principal

    // Manejadores de conexi�n ODBC
    SQLHENV hEnv = SQL_NULL_HENV;  // Handle de entorno, inicializado a NULL
    SQLHDBC hDbc = SQL_NULL_HDBC;  // Handle de conexi�n, inicializado a NULL

    wxDECLARE_EVENT_TABLE();  // Declaraci�n de la tabla de eventos
};
