#ifndef HABITACIONFRAME_H
#define HABITACIONFRAME_H

#include <wx/wx.h>
#include <wx/grid.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <sql.h>
#include <xlnt/xlnt.hpp>
#include <set>

// Declaración adelantada de la clase MainFrame
class MainFrame;

class HabitacionFrame : public wxFrame
{
public:
    HabitacionFrame(const wxString& title, const wxPoint& pos, const wxSize& size, MainFrame* parent = nullptr, const wxString& usuarioAutenticado = "");
    ~HabitacionFrame();

    // Métodos públicos
    void ActualizarDatosEnBaseDeDatos(); // Actualiza los datos en la base de datos en modo edición
    void SetOperacionID(const wxString& operacionID); // Declarar este método
    wxButton* GetExportarButton(); // Método para obtener el botón Exportar
    wxButton* GetGuardarButton(); // Declaración en la clase HabitacionFrame
    bool HayRegistrosEnTabla();
    void SetSelectedIDHabValue(const wxString& idHabValue);
    void SetSelectedDescripcionValue(const wxString& descripcionValue);
    void SetSelectedUMValue(const wxString& umValue);
    bool VerificarIDHabEnBaseDeDatos(const wxString& idHab);
    void ObtenerUMYDescripcion(const wxString& idHab, wxString& umValue, wxString& descripcionValue);
    void ActivarModoEdicion(); // Declarar un método público para activar el modo de edición
    bool enModoEdicion; // Indica si el frame está en modo de edición
    bool IsIDHabDuplicated(const wxString& idHab); // Declaración del nuevo método
    wxGrid* GetGrid();
    void DesactivarEventosGrid();
    void ActivarEventosGrid();
    wxString GetOperacionID() const;
    void OnKeyDown(wxKeyEvent& event);

    // --- Nuevos métodos ---
    bool NumeroOperacionExiste(const wxString& numeroOperacion); // Verifica si la operación ya existe
    int ObtenerSiguienteOperacion(); // Obtiene el siguiente número de operación disponible

    // *** Nuevo método ***
    void ActualizarEstadoBotonesNavegacion(); // Actualiza los botones de navegación tras guardar
    int ObtenerMaximoOperacion(); // Obtiene el mayor número de operación
    int ObtenerMinimoOperacion(); // Obtiene el menor número de operación
    wxButton* GetButtonIzquierda(); // Método para acceder al botón de navegación izquierda
    wxButton* GetButtonDerecha();   // Método para acceder al botón de navegación derecha

private:
    // Manejadores de eventos
    void OnNuevo(wxCommandEvent& event);
    void OnGuardar(wxCommandEvent& event);  // Modificado: Incluye actualización de botones de navegación
    void OnEditar(wxCommandEvent& event);
    void OnCancelar(wxCommandEvent& event);
    void OnEliminar(wxCommandEvent& event); // Maneja la lógica del botón eliminar
    void OnBuscar(wxCommandEvent& event);
    void OnInsertar(wxCommandEvent& event);
    void OnBorrar(wxCommandEvent& event);
    void OnImportarExcel(wxCommandEvent& event);
    void OnExportarExcel(wxCommandEvent& event);
    void OnIzquierda(wxCommandEvent& event);
    void OnDerecha(wxCommandEvent& event);
    void OnClose(wxCloseEvent& event);
    void OnCellChange(wxGridEvent& event);
    void OnMonedaEnter(wxCommandEvent& event);
    void OnGridSelectCell(wxGridEvent& event);
    void OnGridKeyDown(wxKeyEvent& event);
    void OnMonedaTextChange(wxCommandEvent& event);


    // Métodos auxiliares
    void EnableInputs(bool enable);
    void ClearInputs();
    void SetDefaultDates();
    void UpdateButtonStates(bool nuevoSelected, bool editarSelected, bool buscarSelected);
    void UpdateRowLabels(int selectedRow);
    void CenterGridColumns();
    void UpdateMonedaColumn();
    void ShowErrorDialog(const wxString& value, int row, const wxString& columnName);
    bool IsGridEmpty(); // Verifica si la tabla grid está vacía
    void LimpiarVentana(); // Limpia los datos de la ventana después de eliminar
    void InicializarGridEnModoLectura();

    // Métodos de base de datos
    bool ConectarBaseDeDatos(); // Establece la conexión a la base de datos
    void CerrarConexion(); // Cierra la conexión a la base de datos
    void GuardarDatosEnBaseDeDatos(); // Guarda los datos en la base de datos

    int ObtenerSiguienteOperacionDespues(const wxString& operacionID); // Obtiene la siguiente operación después de eliminar
    int ObtenerSiguienteOperacionDespues(const wxString& operacionID, bool haciaDerecha); // Obtiene la siguiente o anterior operación
    void CargarDatosOperacion(int numeroOperacion); // Carga los datos de una operación específica
    void MostrarErrorODBC(SQLSMALLINT handleType, SQLHANDLE handle); // Muestra errores de la base de datos
    void OnGridEditorKeyDown(wxKeyEvent& event);    // Maneja teclas presionadas mientras se edita una celda en el grid

    // Variables de la clase
    wxGrid* grid; // Grilla de datos
    wxTextCtrl* txtFechaC; // Campo de texto para la fecha de creación
    wxTextCtrl* txtFI; // Campo de texto para la fecha de inicio
    wxTextCtrl* txtFV; // Campo de texto para la fecha de vencimiento
    wxTextCtrl* txtDescripcion; // Campo de texto para la descripción
    wxTextCtrl* txtMoneda; // Campo de texto para la moneda
    wxTextCtrl* txtOperacion; // Campo de texto para el número de operación
    wxButton* btnNuevo; // Botón para agregar un nuevo registro
    wxButton* btnGuardar; // Botón para guardar los datos
    wxButton* btnEditar; // Botón para editar un registro
    wxButton* btnCancelar; // Botón para cancelar la operación
    wxButton* btnEliminar; // Botón para eliminar un registro
    wxButton* btnBuscar; // Botón para buscar registros
    wxButton* btnInsertar; // Botón para insertar una fila en la grilla
    wxButton* btnBorrar; // Botón para borrar una fila de la grilla
    wxButton* btnImportar; // Botón para importar datos desde Excel
    wxButton* btnExportar; // Botón para exportar datos a Excel
    wxButton* btnIzquierda; // Botón para navegar a la operación anterior
    wxButton* btnDerecha; // Botón para navegar a la siguiente operación
    wxFont defaultFont; // Fuente predeterminada para la ventana
    wxString usuarioAutenticado; // Almacena el usuario autenticado
    wxString descripcionInicial;
    wxString monedaTInicial;
    wxString fiInicial;
    wxString fvInicial;
    MainFrame* mainFrameParent; // Referencia al marco principal
    SQLHENV hEnv; // Handle de entorno ODBC
    SQLHDBC hDbc; // Handle de conexión ODBC
    bool conexionAbierta; // Indica si la conexión a la base de datos está abierta

    bool estabaEnModoNuevo; // Indica si el frame está en modo de creación de un nuevo registro
    std::set<int> idHabSet; // Conjunto para verificar duplicados de IdHab
    std::vector<std::vector<wxString>> gridValoresIniciales;  // Para almacenar los valores de texto del grid
    std::vector<std::vector<double>> gridValoresInicialesPrecios;  // Para almacenar los precios numéricos del grid
    std::vector<int> filasEliminadas;

    wxDECLARE_EVENT_TABLE(); // Tabla de eventos
};

#endif // HABITACIONFRAME_H
