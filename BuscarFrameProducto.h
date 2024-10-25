#pragma once

#include <wx/wx.h>
#include <wx/grid.h>
#include <sqlext.h>

// Declaración adelantada de la clase CreacionProductoFrame
class CreacionProductoFrame;  // Esto es suficiente para referenciar la clase en el archivo de cabecera

class BuscarFrameProducto : public wxFrame {
public:
    // Constructor y Destructor
    BuscarFrameProducto(wxWindow* parent, const wxString& title, const wxPoint& pos, const wxSize& size);
    ~BuscarFrameProducto();

    // Método para actualizar los botones de navegación en el frame principal (CreacionProductoFrame)
    void ActualizarEstadoBotonesNavegacion(CreacionProductoFrame* creacionFrame);

private:
    // Event Handlers (Manejadores de eventos)
    void OnBuscar(wxCommandEvent& event);
    void OnCancelar(wxCommandEvent& event);
    void OnClose(wxCloseEvent& event);
    void OnGridCellDClick(wxGridEvent& event);   // Doble clic en la celda para seleccionar operación
    void OnGridSelectCell(wxGridEvent& event);   // Evento para la selección de celdas
    void OnKeyDown(wxKeyEvent& event);

    // Interacción con la base de datos
    bool ConectarBaseDeDatos();
    void BuscarOperacionesEnBaseDeDatos(const wxString& operacion, const wxString& descripcion);
    void CerrarConexion();

    // Métodos de utilidad
    void UpdateRowLabels(int selectedRow);  // Actualizar etiquetas de las filas seleccionadas

    // Componentes de la interfaz de usuario
    wxTextCtrl* txtOperacion;    // Campo para buscar por Operación
    wxTextCtrl* txtDescripcion;  // Campo para buscar por Descripción
    wxButton* btnBuscar;
    wxButton* btnCancelar;
    wxGrid* grid;                // Grid para mostrar resultados de la búsqueda

    // Manejadores de la base de datos
    SQLHENV hEnv;
    SQLHDBC hDbc;

    // Referencia al frame principal
    wxWindow* mainFrameParent;  // Referencia al frame principal (CreacionProductoFrame)

    wxDECLARE_EVENT_TABLE();
};
