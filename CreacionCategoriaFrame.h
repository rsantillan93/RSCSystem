#pragma once

#include <wx/wx.h>
#include <sqlext.h>
#include "MainFrame.h"
#include "BuscarFrameCategoria.h"  // Incluir la declaraci�n de BuscarFrameCategoria

class CreacionCategoriaFrame : public wxFrame {
public:
    CreacionCategoriaFrame(const wxString& title, const wxPoint& pos, const wxSize& size, MainFrame* parent, const wxString& usuarioAutenticado);
    ~CreacionCategoriaFrame();

    bool HayRegistrosEnTabla();
    void OnEditar(wxCommandEvent& event);

    // M�todo p�blico para buscar el OperacionID
    void BuscarOperacionCategoria(wxString operacionID, wxString descripcion);

    // Nuevo m�todo para actualizar el estado de los botones de navegaci�n
    void ActualizarEstadoBotonesNavegacionCategoria();

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
    void OnDescripcionChange(wxCommandEvent& event); // Evento para cambios en la descripci�n
    void OnClose(wxCloseEvent& event);
    void OnKeyDown(wxKeyEvent& event);

    // Database Interaction
    bool ConectarBaseDeDatos();               // M�todo para conectar a la base de datos
    void CerrarConexion();                    // M�todo para cerrar la conexi�n a la base de datos
    int ObtenerSiguienteOperacion();          // M�todo para obtener la siguiente Operacion
    int ObtenerUltimaOperacion();             // M�todo para obtener la �ltima Operacion en la tabla
    int ObtenerPrimerOperacion();             // M�todo para obtener la primera Operacion en la tabla
    int ObtenerOperacionAnterior(int currentID); // M�todo para obtener la Operacion anterior al actual
    int ObtenerOperacionSiguiente(int currentID); // M�todo para obtener la siguiente Operacion al actual
    void CargarDatosOperacion(int operacion);   // M�todo para cargar los datos de una categor�a seg�n Operacion
    void CargarUltimoRegistro();              // M�todo para cargar el �ltimo registro en la interfaz

    // Verificaci�n de duplicados
    bool OperacionExists(int operacion);      // M�todo para verificar si una Operacion ya existe

    // Valores originales para restaurar en caso de cancelar o error
    int originalOperacion;
    wxString originalDescripcion;
    wxString usuarioAutenticado; // Almacena el usuario autenticado

    // Utility Methods
    void EnableInputs(bool enable);                             // M�todo para habilitar o deshabilitar campos de entrada
    void ClearInputs();                                         // M�todo para limpiar los campos de entrada
    void UpdateButtonStates(bool nuevoSelected, bool editarSelected); // M�todo para actualizar el estado de los botones
    bool ValidateInputs();                                      // M�todo para validar los campos antes de guardar

    // M�todos de comparaci�n y similitud
    wxString BuscarDescripcionSimilar(const wxString& descripcion); // M�todo para buscar descripciones similares en la base de datos
    wxString NormalizarCadena(const wxString& cadena);          // M�todo para normalizar cadenas (eliminar tildes, 'h', y may�sculas/min�sculas)
    int CalcularDistanciaLevenshtein(const wxString& source, const wxString& target); // M�todo para calcular la distancia de Levenshtein entre dos cadenas

    // Nuevo m�todo privado para buscar el OperacionID relacionado con el IDHab
    wxString BuscarOperacionPorIDHab(const wxString& idHab);

    // UI Components
    wxTextCtrl* txtOperacion;      // Campo para mostrar/ingresar el n�mero de operaci�n
    wxTextCtrl* txtDescripcion;    // Campo para mostrar/ingresar la descripci�n de la categor�a
    wxStaticText* lblSimilitud;    // Campo para mostrar informaci�n de similitud de descripci�n
    wxButton* btnNuevo;            // Bot�n para crear un nuevo registro
    wxButton* btnGuardar;          // Bot�n para guardar el registro
    wxButton* btnEditar;           // Bot�n para editar el registro
    wxButton* btnCancelar;         // Bot�n para cancelar la operaci�n actual
    wxButton* btnEliminar;         // Bot�n para eliminar el registro
    wxButton* btnBuscar;           // Bot�n para buscar registros
    wxButton* btnIzquierda;        // Bot�n para navegar al registro anterior
    wxButton* btnDerecha;          // Bot�n para navegar al siguiente registro
    wxButton* btnExportar;
    wxFont defaultFont;            // Fuente predeterminada para la interfaz

    // Database Handles
    SQLHENV hEnv;   // Manejador de entorno para ODBC
    SQLHDBC hDbc;   // Manejador de conexi�n a la base de datos
    bool conexionAbierta; // Estado de la conexi�n a la base de datos
    bool isEditing;       // Indica si se est� en modo edici�n
    bool isNewOperation;  // Indica si se est� en modo de creaci�n de una nueva operaci�n

    // Parent frame reference
    MainFrame* mainFrameParent; // Referencia al marco principal para reactivarlo cuando se cierra

    wxDECLARE_EVENT_TABLE();
};
