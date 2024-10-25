#pragma once

#include <wx/wx.h>
#include <sqlext.h>
#include "MainFrame.h"
#include "BuscarFrameProducto.h"  // Incluir la declaraci�n de BuscarFrameProducto

class CreacionProductoFrame : public wxFrame {
public:
    // Constructor y Destructor
    CreacionProductoFrame(const wxString& title, const wxPoint& pos, const wxSize& size, MainFrame* parent, const wxString& usuarioAutenticado);
    ~CreacionProductoFrame();

    // M�todos p�blicos
    bool HayRegistrosEnTabla();
    void OnEditar(wxCommandEvent& event);

    // M�todo para buscar y cargar datos del producto por OperacionID
    void BuscarOperacionProducto(const wxString& operacionID, const wxString& descripcion);

    // Getters para acceder a los miembros privados desde otras clases
    wxTextCtrl* GetTxtOperacionID() { return txtOperacionID; }
    wxButton* GetBtnIzquierda() { return btnIzquierda; }
    wxButton* GetBtnDerecha() { return btnDerecha; }

    // M�todos p�blicos para obtener operaciones
    int ObtenerUltimaOperacion();
    int ObtenerPrimerOperacion();

private:
    // Event Handlers (Manejadores de eventos)
    void OnNuevo(wxCommandEvent& event);
    void OnGuardar(wxCommandEvent& event);
    void OnCancelar(wxCommandEvent& event);
    void OnEliminar(wxCommandEvent& event);
    void OnBuscar(wxCommandEvent& event);
    void OnIzquierda(wxCommandEvent& event);
    void OnDerecha(wxCommandEvent& event);
    void OnExportarExcel(wxCommandEvent& event);
    void OnDescripcionChange(wxCommandEvent& event);
    void OnClose(wxCloseEvent& event);
    void OnKeyDown(wxKeyEvent& event);

    // Interacci�n con la base de datos
    bool ConectarBaseDeDatos();
    void CerrarConexion();
    int ObtenerSiguienteOperacion();
    int ObtenerOperacionAnterior(int currentID);
    int ObtenerOperacionSiguiente(int currentID);
    void CargarDatosOperacion(int operacion);
    void CargarUltimoRegistro();
    bool OperacionExists(int operacion);
    bool DescripcionExiste(const wxString& descripcion);

    // Valores originales para restaurar en caso de cancelar o error
    int originalOperacionID;
    wxString originalDescripcion;
    wxString usuarioAutenticado;

    // M�todos de utilidad
    void EnableInputs(bool enable);
    void ClearInputs();
    void UpdateButtonStates(bool nuevoSelected, bool editarSelected);
    bool ValidateInputs();
    void MostrarErrorSQL(SQLHSTMT hStmt, const wxString& mensaje);

    // M�todos de comparaci�n y similitud
    wxString BuscarDescripcionSimilar(const wxString& descripcion);
    wxString NormalizarCadena(const wxString& cadena);
    int CalcularDistanciaLevenshtein(const wxString& source, const wxString& target);

    // M�todo para verificar si el nombre es repetitivo
    bool EsNombreRepetitivo(const wxString& descripcion);

    // Componentes de la interfaz de usuario
    wxTextCtrl* txtOperacionID;
    wxTextCtrl* txtDescripcion;
    wxStaticText* lblSimilitud;
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

    // Manejadores de la base de datos
    SQLHENV hEnv;
    SQLHDBC hDbc;  // Declarado correctamente
    bool conexionAbierta;
    bool isEditing;
    bool isNewOperation;

    // Referencia al marco principal
    MainFrame* mainFrameParent;

    // Declaraci�n del Event Table
    wxDECLARE_EVENT_TABLE();
};
