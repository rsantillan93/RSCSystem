#pragma once

#include <wx/wx.h>
#include <wx/grid.h>
#include <sqlext.h>
#include <map>   // Para usar std::map
#include "MainFrame.h"
#include "CreacionHabitacionesFrame.h"

class BuscarCreacionFrame : public wxFrame {
public:
    // Constructor y destructor
    BuscarCreacionFrame(wxWindow* parent, const wxString& title, const wxPoint& pos, const wxSize& size);
    ~BuscarCreacionFrame();

    // Métodos públicos
    void BuscarOperacionesEnBaseDeDatos(const wxString& nroOperacion, const wxString& descripcion);

private:
    // Manejadores de eventos
    void OnBuscar(wxCommandEvent& event);
    void OnCancelar(wxCommandEvent& event);
    void OnClose(wxCloseEvent& event);
    void OnGridCellDClick(wxGridEvent& event);
    void OnGridSelectCell(wxGridEvent& event);
    void OnKeyDown(wxKeyEvent& event);

    // Método para actualizar la navegación en CreacionHabitacionesFrame
    void ActualizarNavegacionEnCreacionFrame();

    // Métodos auxiliares
    void UpdateRowLabels(int selectedRow);
    bool ConectarBaseDeDatos();
    void CerrarConexion();
    wxString obtenerOperacionIDPorIDHab(const wxString& idHab);

    // Componentes de la UI
    wxTextCtrl* txtNroOperacion;
    wxTextCtrl* txtDescripcion;
    wxGrid* grid;
    wxButton* btnBuscar;
    wxButton* btnCancelar;

    // Recursos de la base de datos
    SQLHENV hEnv;
    SQLHDBC hDbc;

    // Referencia al marco principal
    wxWindow* mainFrameParent;

    // Variables de control
    bool esModoNuevo;
    bool conexionActiva;

    // Mapa que relaciona IDHab con OperacionID
    std::map<wxString, wxString> operacionIdMap;

    wxDECLARE_EVENT_TABLE();
};
