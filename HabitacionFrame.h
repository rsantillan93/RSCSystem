#ifndef HABITACIONFRAME_H
#define HABITACIONFRAME_H

#include <wx/wx.h>
#include <wx/grid.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <sql.h>
#include <xlnt/xlnt.hpp>
#include <set>

// Declaraci�n adelantada de la clase MainFrame
class MainFrame;

class HabitacionFrame : public wxFrame
{
public:
    HabitacionFrame(const wxString& title, const wxPoint& pos, const wxSize& size, MainFrame* parent = nullptr, const wxString& usuarioAutenticado = "");
    ~HabitacionFrame();

    // M�todos p�blicos
    void ActualizarDatosEnBaseDeDatos(); // Actualiza los datos en la base de datos en modo edici�n
    void SetOperacionID(const wxString& operacionID); // Declarar este m�todo
    wxButton* GetExportarButton(); // M�todo para obtener el bot�n Exportar
    wxButton* GetGuardarButton(); // Declaraci�n en la clase HabitacionFrame
    bool HayRegistrosEnTabla();
    void SetSelectedIDHabValue(const wxString& idHabValue);
    void SetSelectedDescripcionValue(const wxString& descripcionValue);
    void SetSelectedUMValue(const wxString& umValue);
    bool VerificarIDHabEnBaseDeDatos(const wxString& idHab);
    void ObtenerUMYDescripcion(const wxString& idHab, wxString& umValue, wxString& descripcionValue);
    void ActivarModoEdicion(); // Declarar un m�todo p�blico para activar el modo de edici�n
    bool enModoEdicion; // Indica si el frame est� en modo de edici�n
    bool IsIDHabDuplicated(const wxString& idHab); // Declaraci�n del nuevo m�todo
    wxGrid* GetGrid();
    void DesactivarEventosGrid();
    void ActivarEventosGrid();
    wxString GetOperacionID() const;
    void OnKeyDown(wxKeyEvent& event);

    // --- Nuevos m�todos ---
    bool NumeroOperacionExiste(const wxString& numeroOperacion); // Verifica si la operaci�n ya existe
    int ObtenerSiguienteOperacion(); // Obtiene el siguiente n�mero de operaci�n disponible

    // *** Nuevo m�todo ***
    void ActualizarEstadoBotonesNavegacion(); // Actualiza los botones de navegaci�n tras guardar
    int ObtenerMaximoOperacion(); // Obtiene el mayor n�mero de operaci�n
    int ObtenerMinimoOperacion(); // Obtiene el menor n�mero de operaci�n
    wxButton* GetButtonIzquierda(); // M�todo para acceder al bot�n de navegaci�n izquierda
    wxButton* GetButtonDerecha();   // M�todo para acceder al bot�n de navegaci�n derecha

private:
    // Manejadores de eventos
    void OnNuevo(wxCommandEvent& event);
    void OnGuardar(wxCommandEvent& event);  // Modificado: Incluye actualizaci�n de botones de navegaci�n
    void OnEditar(wxCommandEvent& event);
    void OnCancelar(wxCommandEvent& event);
    void OnEliminar(wxCommandEvent& event); // Maneja la l�gica del bot�n eliminar
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


    // M�todos auxiliares
    void EnableInputs(bool enable);
    void ClearInputs();
    void SetDefaultDates();
    void UpdateButtonStates(bool nuevoSelected, bool editarSelected, bool buscarSelected);
    void UpdateRowLabels(int selectedRow);
    void CenterGridColumns();
    void UpdateMonedaColumn();
    void ShowErrorDialog(const wxString& value, int row, const wxString& columnName);
    bool IsGridEmpty(); // Verifica si la tabla grid est� vac�a
    void LimpiarVentana(); // Limpia los datos de la ventana despu�s de eliminar
    void InicializarGridEnModoLectura();

    // M�todos de base de datos
    bool ConectarBaseDeDatos(); // Establece la conexi�n a la base de datos
    void CerrarConexion(); // Cierra la conexi�n a la base de datos
    void GuardarDatosEnBaseDeDatos(); // Guarda los datos en la base de datos

    int ObtenerSiguienteOperacionDespues(const wxString& operacionID); // Obtiene la siguiente operaci�n despu�s de eliminar
    int ObtenerSiguienteOperacionDespues(const wxString& operacionID, bool haciaDerecha); // Obtiene la siguiente o anterior operaci�n
    void CargarDatosOperacion(int numeroOperacion); // Carga los datos de una operaci�n espec�fica
    void MostrarErrorODBC(SQLSMALLINT handleType, SQLHANDLE handle); // Muestra errores de la base de datos
    void OnGridEditorKeyDown(wxKeyEvent& event);    // Maneja teclas presionadas mientras se edita una celda en el grid

    // Variables de la clase
    wxGrid* grid; // Grilla de datos
    wxTextCtrl* txtFechaC; // Campo de texto para la fecha de creaci�n
    wxTextCtrl* txtFI; // Campo de texto para la fecha de inicio
    wxTextCtrl* txtFV; // Campo de texto para la fecha de vencimiento
    wxTextCtrl* txtDescripcion; // Campo de texto para la descripci�n
    wxTextCtrl* txtMoneda; // Campo de texto para la moneda
    wxTextCtrl* txtOperacion; // Campo de texto para el n�mero de operaci�n
    wxButton* btnNuevo; // Bot�n para agregar un nuevo registro
    wxButton* btnGuardar; // Bot�n para guardar los datos
    wxButton* btnEditar; // Bot�n para editar un registro
    wxButton* btnCancelar; // Bot�n para cancelar la operaci�n
    wxButton* btnEliminar; // Bot�n para eliminar un registro
    wxButton* btnBuscar; // Bot�n para buscar registros
    wxButton* btnInsertar; // Bot�n para insertar una fila en la grilla
    wxButton* btnBorrar; // Bot�n para borrar una fila de la grilla
    wxButton* btnImportar; // Bot�n para importar datos desde Excel
    wxButton* btnExportar; // Bot�n para exportar datos a Excel
    wxButton* btnIzquierda; // Bot�n para navegar a la operaci�n anterior
    wxButton* btnDerecha; // Bot�n para navegar a la siguiente operaci�n
    wxFont defaultFont; // Fuente predeterminada para la ventana
    wxString usuarioAutenticado; // Almacena el usuario autenticado
    wxString descripcionInicial;
    wxString monedaTInicial;
    wxString fiInicial;
    wxString fvInicial;
    MainFrame* mainFrameParent; // Referencia al marco principal
    SQLHENV hEnv; // Handle de entorno ODBC
    SQLHDBC hDbc; // Handle de conexi�n ODBC
    bool conexionAbierta; // Indica si la conexi�n a la base de datos est� abierta

    bool estabaEnModoNuevo; // Indica si el frame est� en modo de creaci�n de un nuevo registro
    std::set<int> idHabSet; // Conjunto para verificar duplicados de IdHab
    std::vector<std::vector<wxString>> gridValoresIniciales;  // Para almacenar los valores de texto del grid
    std::vector<std::vector<double>> gridValoresInicialesPrecios;  // Para almacenar los precios num�ricos del grid
    std::vector<int> filasEliminadas;

    wxDECLARE_EVENT_TABLE(); // Tabla de eventos
};

#endif // HABITACIONFRAME_H
