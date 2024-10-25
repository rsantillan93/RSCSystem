#pragma once

#include <wx/wx.h>
#include <wx/grid.h>
#include <sqlext.h>

// Declaraci�n adelantada de la clase CreacionIdentificadorFrame
class CreacionIdentificadorFrame;  // Esto es suficiente para referenciar la clase en el archivo de cabecera

class BuscarFrameIdentificador : public wxFrame {
public:
    // Constructor y Destructor
    BuscarFrameIdentificador(wxWindow* parent, const wxString& title, const wxPoint& pos, const wxSize& size);
    ~BuscarFrameIdentificador();

    // M�todo para actualizar los botones de navegaci�n en el frame principal (CreacionIdentificadorFrame)
    void ActualizarEstadoBotonesNavegacion(CreacionIdentificadorFrame* creacionFrame);

private:
    // Event Handlers (Manejadores de eventos)
    void OnBuscar(wxCommandEvent& event);
    void OnCancelar(wxCommandEvent& event);
    void OnClose(wxCloseEvent& event);
    void OnGridCellDClick(wxGridEvent& event);   // Doble clic en la celda para seleccionar operaci�n
    void OnGridSelectCell(wxGridEvent& event);   // Evento para la selecci�n de celdas
    void OnKeyDown(wxKeyEvent& event);

    // Interacci�n con la base de datos
    bool ConectarBaseDeDatos();
    void BuscarOperacionesEnBaseDeDatos(const wxString& operacion, const wxString& descripcion);
    void CerrarConexion();

    // M�todos de utilidad
    void UpdateRowLabels(int selectedRow);  // Actualizar etiquetas de las filas seleccionadas

    // Componentes de la interfaz de usuario
    wxTextCtrl* txtOperacion;    // Campo para buscar por Operaci�n
    wxTextCtrl* txtDescripcion;  // Campo para buscar por Descripci�n
    wxButton* btnBuscar;
    wxButton* btnCancelar;
    wxGrid* grid;                // Grid para mostrar resultados de la b�squeda

    // Manejadores de la base de datos
    SQLHENV hEnv;
    SQLHDBC hDbc;

    // Referencia al frame principal
    wxWindow* mainFrameParent;  // Referencia al frame principal (CreacionIdentificadorFrame)

    wxDECLARE_EVENT_TABLE();
};
