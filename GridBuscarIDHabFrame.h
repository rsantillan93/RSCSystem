#pragma once
#include <wx/wx.h>
#include <wx/grid.h>
#include <sql.h>
#include <sqlext.h>

class GridBuscarIDHabFrame : public wxFrame
{
public:
    GridBuscarIDHabFrame(wxWindow* parent, const wxString& title, const wxPoint& pos, const wxSize& size);
    ~GridBuscarIDHabFrame();

    void DesactivarEventosGrid();
    void ActivarEventosGrid();

private:
    wxGrid* grid;
    wxTextCtrl* txtNroOperacion;
    wxTextCtrl* txtDescripcion;
    wxButton* btnBuscar;
    wxButton* btnCancelar;
    void OnKeyDown(wxKeyEvent& event);

    wxWindow* mainFrameParent;
    SQLHENV hEnv = SQL_NULL_HENV;
    SQLHDBC hDbc = SQL_NULL_HDBC;

    void OnBuscar(wxCommandEvent& event);
    void OnCancelar(wxCommandEvent& event);
    void OnClose(wxCloseEvent& event);
    void OnGridCellDClick(wxGridEvent& event);
    void BuscarHabitacionesEnBaseDeDatos(const wxString& idHab, const wxString& descripcion);
    bool ConectarBaseDeDatos();
    void CerrarConexion();
    void OnGridSelectCell(wxGridEvent& event);  // Declaración del nuevo método para selección de celdas
    void UpdateRowLabels(int selectedRow);  // Método para actualizar etiquetas de filas

    wxDECLARE_EVENT_TABLE();
};

