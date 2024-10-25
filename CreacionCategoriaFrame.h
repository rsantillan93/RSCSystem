#pragma once

#include <wx/wx.h>
#include <sqlext.h>
#include "MainFrame.h"
#include "BuscarFrameCategoria.h"  // Incluir la declaración de BuscarFrameCategoria

class CreacionCategoriaFrame : public wxFrame {
public:
    CreacionCategoriaFrame(const wxString& title, const wxPoint& pos, const wxSize& size, MainFrame* parent, const wxString& usuarioAutenticado);
    ~CreacionCategoriaFrame();

    bool HayRegistrosEnTabla();
    void OnEditar(wxCommandEvent& event);

    // Método público para buscar el OperacionID
    void BuscarOperacionCategoria(wxString operacionID, wxString descripcion);

    // Nuevo método para actualizar el estado de los botones de navegación
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
    void OnDescripcionChange(wxCommandEvent& event); // Evento para cambios en la descripción
    void OnClose(wxCloseEvent& event);
    void OnKeyDown(wxKeyEvent& event);

    // Database Interaction
    bool ConectarBaseDeDatos();               // Método para conectar a la base de datos
    void CerrarConexion();                    // Método para cerrar la conexión a la base de datos
    int ObtenerSiguienteOperacion();          // Método para obtener la siguiente Operacion
    int ObtenerUltimaOperacion();             // Método para obtener la última Operacion en la tabla
    int ObtenerPrimerOperacion();             // Método para obtener la primera Operacion en la tabla
    int ObtenerOperacionAnterior(int currentID); // Método para obtener la Operacion anterior al actual
    int ObtenerOperacionSiguiente(int currentID); // Método para obtener la siguiente Operacion al actual
    void CargarDatosOperacion(int operacion);   // Método para cargar los datos de una categoría según Operacion
    void CargarUltimoRegistro();              // Método para cargar el último registro en la interfaz

    // Verificación de duplicados
    bool OperacionExists(int operacion);      // Método para verificar si una Operacion ya existe

    // Valores originales para restaurar en caso de cancelar o error
    int originalOperacion;
    wxString originalDescripcion;
    wxString usuarioAutenticado; // Almacena el usuario autenticado

    // Utility Methods
    void EnableInputs(bool enable);                             // Método para habilitar o deshabilitar campos de entrada
    void ClearInputs();                                         // Método para limpiar los campos de entrada
    void UpdateButtonStates(bool nuevoSelected, bool editarSelected); // Método para actualizar el estado de los botones
    bool ValidateInputs();                                      // Método para validar los campos antes de guardar

    // Métodos de comparación y similitud
    wxString BuscarDescripcionSimilar(const wxString& descripcion); // Método para buscar descripciones similares en la base de datos
    wxString NormalizarCadena(const wxString& cadena);          // Método para normalizar cadenas (eliminar tildes, 'h', y mayúsculas/minúsculas)
    int CalcularDistanciaLevenshtein(const wxString& source, const wxString& target); // Método para calcular la distancia de Levenshtein entre dos cadenas

    // Nuevo método privado para buscar el OperacionID relacionado con el IDHab
    wxString BuscarOperacionPorIDHab(const wxString& idHab);

    // UI Components
    wxTextCtrl* txtOperacion;      // Campo para mostrar/ingresar el número de operación
    wxTextCtrl* txtDescripcion;    // Campo para mostrar/ingresar la descripción de la categoría
    wxStaticText* lblSimilitud;    // Campo para mostrar información de similitud de descripción
    wxButton* btnNuevo;            // Botón para crear un nuevo registro
    wxButton* btnGuardar;          // Botón para guardar el registro
    wxButton* btnEditar;           // Botón para editar el registro
    wxButton* btnCancelar;         // Botón para cancelar la operación actual
    wxButton* btnEliminar;         // Botón para eliminar el registro
    wxButton* btnBuscar;           // Botón para buscar registros
    wxButton* btnIzquierda;        // Botón para navegar al registro anterior
    wxButton* btnDerecha;          // Botón para navegar al siguiente registro
    wxButton* btnExportar;
    wxFont defaultFont;            // Fuente predeterminada para la interfaz

    // Database Handles
    SQLHENV hEnv;   // Manejador de entorno para ODBC
    SQLHDBC hDbc;   // Manejador de conexión a la base de datos
    bool conexionAbierta; // Estado de la conexión a la base de datos
    bool isEditing;       // Indica si se está en modo edición
    bool isNewOperation;  // Indica si se está en modo de creación de una nueva operación

    // Parent frame reference
    MainFrame* mainFrameParent; // Referencia al marco principal para reactivarlo cuando se cierra

    wxDECLARE_EVENT_TABLE();
};
