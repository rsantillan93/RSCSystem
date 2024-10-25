// Incluye las cabeceras necesarias para la clase HabitacionFrame
#include "HabitacionFrame.h"
#include "CreacionHabitacionesFrame.h" // Agrega esta línea para incluir el SubMenú
#include "MainFrame.h"
#include "IDs.h"
#include <wx/clipbrd.h>
#include "GridBuscarIDHabFrame.h"
#include <wx/tokenzr.h>
#include <xlnt/xlnt.hpp>
#include <wx/datetime.h>
#include <wx/msw/ole/automtn.h>
#include <set> // Agregar esta línea
#include "BuscarFrameHabitacion.h"
#include <wx/dir.h>

wxBEGIN_EVENT_TABLE(HabitacionFrame, wxFrame)
EVT_BUTTON(ID_BTN_NUEVO, HabitacionFrame::OnNuevo)
EVT_BUTTON(ID_BTN_GUARDAR, HabitacionFrame::OnGuardar)
EVT_BUTTON(ID_BTN_EDITAR, HabitacionFrame::OnEditar)
EVT_BUTTON(ID_BTN_CANCELAR, HabitacionFrame::OnCancelar)
EVT_BUTTON(ID_BTN_ELIMINAR, HabitacionFrame::OnEliminar)
EVT_BUTTON(ID_BTN_BUSCAR, HabitacionFrame::OnBuscar)
EVT_BUTTON(ID_BTN_IMPORTAR, HabitacionFrame::OnImportarExcel)
EVT_BUTTON(ID_BTN_EXPORTAR, HabitacionFrame::OnExportarExcel)
EVT_BUTTON(ID_BTN_INSERTAR, HabitacionFrame::OnInsertar)
EVT_BUTTON(ID_BTN_BORRAR, HabitacionFrame::OnBorrar)
EVT_BUTTON(ID_BTN_IZQUIERDA, HabitacionFrame::OnIzquierda)  // Botón <
EVT_BUTTON(ID_BTN_DERECHA, HabitacionFrame::OnDerecha)      // Botón >
EVT_TEXT_ENTER(wxID_ANY, HabitacionFrame::OnMonedaEnter)
EVT_GRID_CELL_CHANGED(HabitacionFrame::OnCellChange)
EVT_GRID_SELECT_CELL(HabitacionFrame::OnGridSelectCell)
EVT_CHAR_HOOK(HabitacionFrame::OnKeyDown)  // Captura la tecla "Esc"
EVT_CLOSE(HabitacionFrame::OnClose)
wxEND_EVENT_TABLE()

HabitacionFrame::HabitacionFrame(const wxString& title, const wxPoint& pos, const wxSize& size, MainFrame* parent, const wxString& usuario)
    : wxFrame(parent, wxID_ANY, title, pos, size, wxDEFAULT_FRAME_STYLE & ~(wxMAXIMIZE_BOX) | wxFRAME_NO_TASKBAR | wxFRAME_FLOAT_ON_PARENT),
    mainFrameParent(parent), usuarioAutenticado(usuario), estabaEnModoNuevo(false), enModoEdicion(false)
{
    // Set default font for the frame
    defaultFont = wxFont(10, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    SetFont(defaultFont);

    wxPanel* panel = new wxPanel(this, wxID_ANY);

    // Action buttons
    btnNuevo = new wxButton(panel, ID_BTN_NUEVO, "Nuevo", wxPoint(10, 10), wxSize(80, 30));
    btnNuevo->SetFont(defaultFont);

    btnGuardar = new wxButton(panel, ID_BTN_GUARDAR, "Guardar", wxPoint(100, 10), wxSize(80, 30));
    btnGuardar->SetFont(defaultFont);
    btnGuardar->Enable(false);

    btnEditar = new wxButton(panel, ID_BTN_EDITAR, "Editar", wxPoint(190, 10), wxSize(80, 30));
    btnEditar->SetFont(defaultFont);

    btnBuscar = new wxButton(panel, ID_BTN_BUSCAR, "Buscar", wxPoint(280, 10), wxSize(80, 30));
    btnBuscar->SetFont(defaultFont);

    btnCancelar = new wxButton(panel, ID_BTN_CANCELAR, "Cancelar", wxPoint(370, 10), wxSize(80, 30));
    btnCancelar->SetFont(defaultFont);
    btnCancelar->Enable(false);

    // Botón <
    btnIzquierda = new wxButton(panel, ID_BTN_IZQUIERDA, "<", wxPoint(460, 10), wxSize(40, 30));
    btnIzquierda->SetFont(defaultFont);
    btnIzquierda->Enable(true); // Habilitado al inicio

    // Botón >
    btnDerecha = new wxButton(panel, ID_BTN_DERECHA, ">", wxPoint(510, 10), wxSize(40, 30));
    btnDerecha->SetFont(defaultFont);
    btnDerecha->Enable(true); // Habilitado al inicio

    btnEliminar = new wxButton(panel, ID_BTN_ELIMINAR, "Eliminar", wxPoint(610, 10), wxSize(80, 30));
    btnEliminar->SetFont(defaultFont);

    wxStaticText* lblFechaC = new wxStaticText(panel, wxID_ANY, "Fecha de Creacion:", wxPoint(10, 50), wxDefaultSize, wxALIGN_CENTER_VERTICAL);
    lblFechaC->SetFont(defaultFont);

    txtFechaC = new wxTextCtrl(panel, wxID_ANY, "", wxPoint(155, 47), wxSize(90, 25));
    txtFechaC->SetFont(defaultFont);
    txtFechaC->Enable(false);

    wxStaticText* lblMoneda = new wxStaticText(panel, wxID_ANY, "Moneda:", wxPoint(280, 50));
    lblMoneda->SetFont(defaultFont);

    txtMoneda = new wxTextCtrl(panel, wxID_ANY, "", wxPoint(340, 47), wxSize(30, 25), wxTE_PROCESS_ENTER | wxTE_CAPITALIZE);
    txtMoneda->SetMaxLength(1);
    txtMoneda->SetFont(defaultFont);
    txtMoneda->Enable(false);  // Deshabilitado por defecto, solo habilitar en Nuevo/Editar

    // Bind the text change event to ensure text is always uppercase
    txtMoneda->Bind(wxEVT_TEXT, &HabitacionFrame::OnMonedaTextChange, this);

    wxStaticText* lblOperacion = new wxStaticText(panel, wxID_ANY, "Nro de Operacion:", wxPoint(415, 50), wxDefaultSize, wxALIGN_CENTER_VERTICAL);
    lblOperacion->SetFont(defaultFont);

    txtOperacion = new wxTextCtrl(panel, wxID_ANY, "", wxPoint(550, 47), wxSize(150, 25));
    txtOperacion->SetFont(defaultFont);
    txtOperacion->Enable(false);

    wxStaticText* lblDescripcion = new wxStaticText(panel, wxID_ANY, "Descripcion:", wxPoint(10, 90), wxDefaultSize, wxALIGN_CENTER_VERTICAL);
    lblDescripcion->SetFont(defaultFont);

    txtDescripcion = new wxTextCtrl(panel, wxID_ANY, "", wxPoint(110, 87), wxSize(590, 25));
    txtDescripcion->SetFont(defaultFont);
    txtDescripcion->Enable(false);

    wxStaticText* lblFI = new wxStaticText(panel, wxID_ANY, "Fecha de Inicio:", wxPoint(10, 130), wxDefaultSize, wxALIGN_CENTER_VERTICAL);
    lblFI->SetFont(defaultFont);

    txtFI = new wxTextCtrl(panel, wxID_ANY, "", wxPoint(140, 127), wxSize(150, 25));
    txtFI->SetFont(defaultFont);
    txtFI->Enable(false);

    wxStaticText* lblFV = new wxStaticText(panel, wxID_ANY, "Fecha de Vencimiento:", wxPoint(310, 130), wxDefaultSize, wxALIGN_CENTER_VERTICAL);
    lblFV->SetFont(defaultFont);

    txtFV = new wxTextCtrl(panel, wxID_ANY, "", wxPoint(480, 127), wxSize(150, 25));
    txtFV->SetFont(defaultFont);
    txtFV->Enable(false);

    // Create grid with 100 initial rows and 7 columns
    grid = new wxGrid(panel, wxID_ANY, wxPoint(10, 160), wxSize(700, 400));
    grid->CreateGrid(100, 7);  // Start with 100 rows and 7 columns
    grid->SetDefaultCellFont(defaultFont);
    grid->SetRowLabelSize(18);

    // Set column headers
    grid->SetColLabelValue(0, "IdHab");
    grid->SetColLabelValue(1, "U.M");
    grid->SetColLabelValue(2, "Tipo de habitación");
    grid->SetColLabelValue(3, "Moneda");
    grid->SetColLabelValue(4, "Precio 01");
    grid->SetColLabelValue(5, "Precio 02");
    grid->SetColLabelValue(6, "Precio 03");

    // Set column sizes
    grid->SetColSize(0, 50);  // Width for "IdHab"
    grid->SetColSize(1, 30);  // Width for "U.M"
    grid->SetColSize(2, 285); // Width for "Tipo de habitación"
    grid->SetColSize(3, 60);  // Width for "Moneda"
    grid->SetColSize(4, 80);  // Width for "Precio 01"
    grid->SetColSize(5, 80);  // Width for "Precio 02"
    grid->SetColSize(6, 80);  // Width for "Precio 03"

    grid->SetLabelFont(defaultFont);
    grid->SetSelectionMode(wxGrid::wxGridSelectCells);
    grid->EnableEditing(false);  // Todas las celdas en modo lectura inicialmente

    // Center specific columns
    CenterGridColumns();

    // Bind grid events directly to the grid
    grid->PushEventHandler(new wxEvtHandler);
    grid->Bind(wxEVT_KEY_DOWN, &HabitacionFrame::OnGridKeyDown, this);
    grid->Bind(wxEVT_GRID_EDITOR_CREATED, [&](wxGridEditorCreatedEvent& event) {
        // Obtener el editor creado y asociar un evento de tecla personalizado
        wxWindow* editor = event.GetControl();
        editor->Bind(wxEVT_KEY_DOWN, &HabitacionFrame::OnGridEditorKeyDown, this);
        });

    // Initialize row labels to be empty
    UpdateRowLabels(-1);  // Call to ensure labels are initialized empty

    // Other button setups
    btnImportar = new wxButton(panel, ID_BTN_IMPORTAR, "Importar Excel", wxPoint(10, 570), wxSize(120, 30));
    btnImportar->SetFont(defaultFont);
    btnImportar->Enable(true);

    btnExportar = new wxButton(panel, ID_BTN_EXPORTAR, "Exportar Excel", wxPoint(150, 570), wxSize(120, 30));
    btnExportar->SetFont(defaultFont);
    btnExportar->Enable(false);  // Habilitado al inicio

    btnInsertar = new wxButton(panel, ID_BTN_INSERTAR, "Insertar", wxPoint(290, 570), wxSize(80, 30));
    btnInsertar->SetFont(defaultFont);
    btnInsertar->Enable(true);

    btnBorrar = new wxButton(panel, ID_BTN_BORRAR, "Borrar", wxPoint(380, 570), wxSize(80, 30));
    btnBorrar->SetFont(defaultFont);
    btnBorrar->Enable(true);

    // Estado inicial de los botones
    UpdateButtonStates(false, false, false);

    // Inicializar grid en modo lectura
    InicializarGridEnModoLectura();

    // Connect to the database on start
    if (ConectarBaseDeDatos()) {
        // Verificar si hay registros en la tabla
        if (HayRegistrosEnTabla()) {
            int ultimaOperacion = ObtenerMaximoOperacion();
            txtOperacion->SetValue(wxString::Format("%08d", ultimaOperacion));
            CargarDatosOperacion(ultimaOperacion);

            // Obtener el primer y último número de operación
            int primeraOperacion = ObtenerMinimoOperacion();

            // Deshabilitar botones de navegación si es el primer o el último registro
            if (primeraOperacion == ultimaOperacion) {
                btnIzquierda->Enable(false);
                btnDerecha->Enable(false);
            }
            else {
                if (wxAtoi(txtOperacion->GetValue()) == primeraOperacion) {
                    btnIzquierda->Enable(false);
                }
                else {
                    btnIzquierda->Enable(true);
                }

                if (wxAtoi(txtOperacion->GetValue()) == ultimaOperacion) {
                    btnDerecha->Enable(false);
                }
                else {
                    btnDerecha->Enable(true);
                }
            }
        }
        else {
            UpdateButtonStates(false, false, false);
            btnNuevo->Enable(true);
            btnGuardar->Enable(false);
            btnEditar->Enable(false);
            btnEliminar->Enable(false);
            btnBuscar->Enable(false);
            btnCancelar->Enable(false);
            btnIzquierda->Enable(false);
            btnDerecha->Enable(false);
            btnExportar->Enable(false);
        }
    }
    else {
        wxMessageBox("Error al conectar a la base de datos.", "Error", wxOK | wxICON_ERROR);
    }
}

HabitacionFrame::~HabitacionFrame() {
    CerrarConexion(); // Cerrar la conexión a la base de datos al destruir el objeto
}

void HabitacionFrame::OnKeyDown(wxKeyEvent& event) {
    if (event.GetKeyCode() == WXK_ESCAPE) {
        // Deshabilitar temporalmente el evento para evitar ciclos
        this->Unbind(wxEVT_CHAR_HOOK, &HabitacionFrame::OnKeyDown, this);

        // Si estás en modo nuevo o edición
        if (estabaEnModoNuevo || enModoEdicion) {
            int respuesta = wxMessageBox("Hay cambios sin guardar. ¿Desea cancelar los cambios?", "Confirmar cancelación", wxYES_NO | wxICON_QUESTION);
            if (respuesta == wxYES) {
                // Simular comportamiento de "Cancelar"
                wxCommandEvent evt;  // Crear un evento wxCommandEvent vacío
                OnCancelar(evt);     // Pasar el evento vacío
            }
        }
        else {
            // Si no estás en modo nuevo o edición, cerrar la ventana
            if (mainFrameParent != nullptr) {
                mainFrameParent->Enable();
            }
            Destroy();
        }

        // Volver a habilitar el evento después de la operación
        this->Bind(wxEVT_CHAR_HOOK, &HabitacionFrame::OnKeyDown, this);
    }
    else {
        event.Skip();  // Continuar con otros eventos
    }
}

bool HabitacionFrame::IsIDHabDuplicated(const wxString& idHab) {
    // Recorre todas las filas para comprobar si el IDHab ya existe
    for (int row = 0; row < grid->GetNumberRows(); ++row) {
        if (grid->GetCellValue(row, 0) == idHab) { // Columna 0 es la columna de IDHab
            return true; // Si se encuentra un duplicado, devuelve true
        }
    }
    return false; // Si no hay duplicados, devuelve false
}

wxButton* HabitacionFrame::GetExportarButton()
{
    return btnExportar; // Retorna el botón Exportar
}

wxButton* HabitacionFrame::GetGuardarButton()
{
    return btnGuardar; // Retorna el botón Guardar
}

wxButton* HabitacionFrame::GetButtonIzquierda() {
    return btnIzquierda;  // Devuelve el botón de navegación izquierda
}

wxButton* HabitacionFrame::GetButtonDerecha() {
    return btnDerecha;  // Devuelve el botón de navegación derecha
}

wxString HabitacionFrame::GetOperacionID() const {
    return txtOperacion->GetValue();
}

void HabitacionFrame::SetOperacionID(const wxString& operacionID)
{
    txtOperacion->SetValue(operacionID);
    int numeroOperacion = wxAtoi(operacionID);
    CargarDatosOperacion(numeroOperacion);
}

int HabitacionFrame::ObtenerSiguienteOperacion() {
    SQLHSTMT hStmt = NULL;
    SQLRETURN ret;
    int siguienteOperacion = 1; // Valor por defecto si no hay datos en la tabla

    // Asegurarse de que la conexión a la base de datos está abierta
    if (!conexionAbierta && !ConectarBaseDeDatos()) {
        wxMessageBox("Error al conectar a la base de datos.", "Error", wxOK | wxICON_ERROR);
        return siguienteOperacion; // Retornar el valor por defecto si no se pudo conectar
    }

    // Asignar el handle de declaración SQL
    ret = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        MostrarErrorODBC(SQL_HANDLE_DBC, hDbc); // Mostrar el error al asignar el handle
        return siguienteOperacion; // Retornar el valor por defecto si no se pudo asignar el handle
    }

    // Consulta para obtener el siguiente número de operación
    wxString query = L"SELECT ISNULL(MAX(Operacion), 0) + 1 FROM PrecioHabitacion;";
    ret = SQLExecDirect(hStmt, (SQLWCHAR*)query.wc_str(), SQL_NTS);

    // Validar si la consulta se ejecutó correctamente
    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
        SQLFetch(hStmt);
        SQLGetData(hStmt, 1, SQL_C_LONG, &siguienteOperacion, 0, NULL);
    }
    else {
        MostrarErrorODBC(SQL_HANDLE_STMT, hStmt); // Mostrar error en caso de fallo en la consulta
    }

    // Liberar el handle de declaración SQL
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);

    return siguienteOperacion;
}

int HabitacionFrame::ObtenerSiguienteOperacionDespues(const wxString& operacionID) {
    SQLHSTMT hStmt = NULL;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

    // Consulta para obtener el siguiente número de operación después del actual
    wxString querySiguiente = wxString::Format(L"SELECT MIN(Operacion) FROM PrecioHabitacion WHERE Operacion > %d;", wxAtoi(operacionID));
    SQLRETURN ret = SQLExecDirect(hStmt, (SQLWCHAR*)querySiguiente.wc_str(), SQL_NTS);

    int siguienteOperacion = 0;

    // Obtener el siguiente número de operación si la consulta fue exitosa
    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
        SQLFetch(hStmt);
        SQLGetData(hStmt, 1, SQL_C_LONG, &siguienteOperacion, 0, NULL);
    }

    // Liberar el handle de la consulta SQL
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);

    // Si se encontró un número de operación válido, devolverlo
    if (siguienteOperacion > 0) {
        return siguienteOperacion;
    }

    // Si no se encontró un número siguiente, buscar la operación anterior
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
    wxString queryAnterior = wxString::Format(L"SELECT MAX(Operacion) FROM PrecioHabitacion WHERE Operacion < %d;", wxAtoi(operacionID));
    ret = SQLExecDirect(hStmt, (SQLWCHAR*)queryAnterior.wc_str(), SQL_NTS);

    int operacionAnterior = 0;

    // Obtener el número de operación anterior si la consulta fue exitosa
    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
        SQLFetch(hStmt);
        SQLGetData(hStmt, 1, SQL_C_LONG, &operacionAnterior, 0, NULL);
    }

    // Liberar el handle de la consulta SQL
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);

    // Si se encontró un número de operación anterior, devolverlo
    if (operacionAnterior > 0) {
        return operacionAnterior;
    }

    // Si no hay números anteriores ni siguientes, buscar la única operación que queda
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
    wxString queryUnica = L"SELECT MIN(Operacion) FROM PrecioHabitacion;";
    ret = SQLExecDirect(hStmt, (SQLWCHAR*)queryUnica.wc_str(), SQL_NTS);

    int unicaOperacion = 0;

    // Obtener el número de la única operación que queda si la consulta fue exitosa
    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
        SQLFetch(hStmt);
        SQLGetData(hStmt, 1, SQL_C_LONG, &unicaOperacion, 0, NULL);
    }

    // Liberar el handle de la consulta SQL
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);

    // Retornar el número de la única operación o 0 si no existe ninguna
    return unicaOperacion;
}

int HabitacionFrame::ObtenerSiguienteOperacionDespues(const wxString& operacionID, bool haciaDerecha) {
    SQLHSTMT hStmt = NULL;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

    wxString query;

    if (haciaDerecha) {
        // Buscar la siguiente operación mayor
        query = wxString::Format(L"SELECT MIN(Operacion) FROM PrecioHabitacion WHERE Operacion > %d;", wxAtoi(operacionID));
    }
    else {
        // Buscar la operación anterior menor
        query = wxString::Format(L"SELECT MAX(Operacion) FROM PrecioHabitacion WHERE Operacion < %d;", wxAtoi(operacionID));
    }

    SQLRETURN ret = SQLExecDirect(hStmt, (SQLWCHAR*)query.wc_str(), SQL_NTS);

    int siguienteOperacion = 0;

    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
        SQLFetch(hStmt);
        SQLGetData(hStmt, 1, SQL_C_LONG, &siguienteOperacion, 0, NULL);
    }

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);

    return siguienteOperacion;
}

void HabitacionFrame::CargarDatosOperacion(int numeroOperacion) {
    SQLHSTMT hStmt = NULL;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

    // Consulta SQL para obtener los datos
    wxString query = wxString::Format(L"SELECT FechaC, Descripcion, FI, FV, MonedaT, IdHab, UM, TipoHabitacion, Moneda, Precio01, Precio02, Precio03 FROM PrecioHabitacion WHERE Operacion = %d;", numeroOperacion);
    SQLRETURN ret = SQLExecDirect(hStmt, (SQLWCHAR*)query.wc_str(), SQL_NTS);

    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
        grid->ClearGrid();  // Limpiar la grilla antes de agregar nuevas filas
        grid->ClearSelection();  // Limpiar cualquier selección visual previa
        int row = 0;

        // Limpiar y preparar para almacenar los valores iniciales
        gridValoresIniciales.clear();
        gridValoresInicialesPrecios.clear();

        // Procesar los datos fila por fila
        while (SQLFetch(hStmt) == SQL_SUCCESS) {
            // Variables para almacenar los datos recuperados
            SQLWCHAR fechaC[25], descripcion[255], fi[25], fv[25], monedaT[50];
            int idHab;
            SQLWCHAR um[50], tipoHabitacion[255], moneda[10];
            double precio01, precio02, precio03;

            // Obtener datos de la consulta
            SQLGetData(hStmt, 1, SQL_C_WCHAR, fechaC, sizeof(fechaC), NULL);
            SQLGetData(hStmt, 2, SQL_C_WCHAR, descripcion, sizeof(descripcion), NULL);
            SQLGetData(hStmt, 3, SQL_C_WCHAR, fi, sizeof(fi), NULL);
            SQLGetData(hStmt, 4, SQL_C_WCHAR, fv, sizeof(fv), NULL);
            SQLGetData(hStmt, 5, SQL_C_WCHAR, monedaT, sizeof(monedaT), NULL);
            SQLGetData(hStmt, 6, SQL_C_LONG, &idHab, 0, NULL);
            SQLGetData(hStmt, 7, SQL_C_WCHAR, um, sizeof(um), NULL);
            SQLGetData(hStmt, 8, SQL_C_WCHAR, tipoHabitacion, sizeof(tipoHabitacion), NULL);
            SQLGetData(hStmt, 9, SQL_C_WCHAR, moneda, sizeof(moneda), NULL);
            SQLGetData(hStmt, 10, SQL_C_DOUBLE, &precio01, 0, NULL);
            SQLGetData(hStmt, 11, SQL_C_DOUBLE, &precio02, 0, NULL);
            SQLGetData(hStmt, 12, SQL_C_DOUBLE, &precio03, 0, NULL);

            // Verificar si la grilla tiene suficientes filas, si no, agregar más filas
            if (row >= grid->GetNumberRows()) {
                grid->AppendRows(1);
            }

            // Establecer los valores en los campos de texto y la grilla
            if (row == 0) {
                // Conversión de las fechas al formato adecuado
                wxDateTime fechaC_dt, fi_dt, fv_dt;
                fechaC_dt.ParseFormat(wxString(fechaC), "%Y-%m-%d %H:%M:%S");
                fi_dt.ParseFormat(wxString(fi), "%Y-%m-%d %H:%M:%S");
                fv_dt.ParseFormat(wxString(fv), "%Y-%m-%d %H:%M:%S");

                // Establecer los valores formateados en los controles de texto
                txtFechaC->SetValue(fechaC_dt.IsValid() ? fechaC_dt.Format("%d/%m/%Y %H:%M:%S") : wxString(fechaC));
                txtFI->SetValue(fi_dt.IsValid() ? fi_dt.Format("%d/%m/%Y %H:%M:%S") : wxString(fi));
                txtFV->SetValue(fv_dt.IsValid() ? fv_dt.Format("%d/%m/%Y %H:%M:%S") : wxString(fv));
                txtDescripcion->SetValue(wxString(descripcion));
                txtMoneda->SetValue(wxString(monedaT));

                // Guardar los valores iniciales de los TextBox
                descripcionInicial = txtDescripcion->GetValue();
                monedaTInicial = txtMoneda->GetValue();
                fiInicial = txtFI->GetValue();
                fvInicial = txtFV->GetValue();
            }

            // Establecer los valores en la grilla
            grid->SetCellValue(row, 0, wxString::Format("%d", idHab));
            grid->SetCellValue(row, 1, wxString(um));
            grid->SetCellValue(row, 2, wxString(tipoHabitacion));
            grid->SetCellValue(row, 3, wxString(moneda));
            grid->SetCellValue(row, 4, wxString::Format("%.4f", precio01));
            grid->SetCellValue(row, 5, wxString::Format("%.4f", precio02));
            grid->SetCellValue(row, 6, wxString::Format("%.4f", precio03));

            // Guardar los valores iniciales del grid
            std::vector<wxString> filaValores = { wxString::Format("%d", idHab), wxString(um), wxString(tipoHabitacion), wxString(moneda) };
            std::vector<double> filaPrecios = { precio01, precio02, precio03 };

            gridValoresIniciales.push_back(filaValores);
            gridValoresInicialesPrecios.push_back(filaPrecios);

            row++;
        }
    }
    else {
        MostrarErrorODBC(SQL_HANDLE_STMT, hStmt);
    }

    // Liberar el cursor después de usarlo
    SQLFreeStmt(hStmt, SQL_CLOSE);
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);

    // Limpiar cualquier selección visual previa
    grid->ClearSelection();
}

int HabitacionFrame::ObtenerMinimoOperacion() {
    SQLHSTMT hStmt = NULL;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

    wxString query = L"SELECT ISNULL(MIN(Operacion), 0) FROM PrecioHabitacion;";
    SQLRETURN ret = SQLExecDirect(hStmt, (SQLWCHAR*)query.wc_str(), SQL_NTS);

    int minimoOperacion = 0;

    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
        SQLFetch(hStmt);
        SQLGetData(hStmt, 1, SQL_C_LONG, &minimoOperacion, 0, NULL);
    }
    else {
        MostrarErrorODBC(SQL_HANDLE_STMT, hStmt);
    }

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    return minimoOperacion;
}

int HabitacionFrame::ObtenerMaximoOperacion() {
    SQLHSTMT hStmt = NULL;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

    wxString query = L"SELECT ISNULL(MAX(Operacion), 0) FROM PrecioHabitacion;";
    SQLRETURN ret = SQLExecDirect(hStmt, (SQLWCHAR*)query.wc_str(), SQL_NTS);

    int maximoOperacion = 0;

    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
        SQLFetch(hStmt);
        SQLGetData(hStmt, 1, SQL_C_LONG, &maximoOperacion, 0, NULL);
    }
    else {
        MostrarErrorODBC(SQL_HANDLE_STMT, hStmt);
    }

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    return maximoOperacion;
}

void HabitacionFrame::OnIzquierda(wxCommandEvent& event) {
    wxString operacionActual = txtOperacion->GetValue();
    int operacionAnterior = ObtenerSiguienteOperacionDespues(operacionActual, false);  // Buscar la operación anterior (hacia la izquierda)

    if (operacionAnterior > 0) {
        // Cargar el registro anterior
        txtOperacion->SetValue(wxString::Format("%08d", operacionAnterior));
        CargarDatosOperacion(operacionAnterior);
    }

    // Actualizar los estados de los botones de navegación
    ActualizarEstadoBotonesNavegacion();

    // Posicionar el cursor en la primera fila y columna del grid
    grid->SetGridCursor(0, 0);
    grid->MakeCellVisible(0, 0);
    grid->SetFocus();
}

void HabitacionFrame::OnDerecha(wxCommandEvent& event) {
    wxString operacionActual = txtOperacion->GetValue();
    int operacionSiguiente = ObtenerSiguienteOperacionDespues(operacionActual, true);  // Buscar la siguiente operación (hacia la derecha)

    if (operacionSiguiente > 0) {
        // Cargar el siguiente registro
        txtOperacion->SetValue(wxString::Format("%08d", operacionSiguiente));
        CargarDatosOperacion(operacionSiguiente);
    }

    // Actualizar los estados de los botones de navegación
    ActualizarEstadoBotonesNavegacion();

    // Posicionar el cursor en la primera fila y columna del grid
    grid->SetGridCursor(0, 0);
    grid->MakeCellVisible(0, 0);
    grid->SetFocus();
}

void HabitacionFrame::GuardarDatosEnBaseDeDatos() {
    // Verificar si la conexión está abierta, si no, intentar conectarse a la base de datos
    if (!conexionAbierta && !ConectarBaseDeDatos()) {
        wxMessageBox("Error al conectar a la base de datos.", "Error", wxOK | wxICON_ERROR);
        return;
    }

    SQLHSTMT hStmt = NULL;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

    // Iniciar la transacción
    SQLRETURN ret = SQLExecDirect(hStmt, (SQLWCHAR*)L"BEGIN TRANSACTION;", SQL_NTS);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        MostrarErrorODBC(SQL_HANDLE_STMT, hStmt);
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return;
    }

    // Obtener el siguiente número de operación
    int siguienteOperacion = ObtenerSiguienteOperacion();
    wxString operacionID = txtOperacion->GetValue();

    // Capturar el UsuarioID y Ordenador
    wxString usuarioID = usuarioAutenticado;  // Nombre del usuario actual de la sesión
    wxString ordenador = wxGetHostName();  // Nombre del ordenador actual

    // Consulta de inserción incluyendo UsuarioID y Ordenador
    wxString query = L"INSERT INTO PrecioHabitacion (Operacion, OperacionID, FechaC, Descripcion, FI, FV, MonedaT, IdHab, UM, TipoHabitacion, Moneda, Precio01, Precio02, Precio03, UsuarioID, Ordenador) "
        L"VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";

    ret = SQLPrepare(hStmt, (SQLWCHAR*)query.wc_str(), SQL_NTS);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        SQLExecDirect(hStmt, (SQLWCHAR*)L"ROLLBACK;", SQL_NTS);
        MostrarErrorODBC(SQL_HANDLE_STMT, hStmt);
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return;
    }

    // Convertir las fechas a formato SQL (YYYY-MM-DD HH:MM:SS)
    wxDateTime fechaC_dt, fi_dt, fv_dt;
    if (!fechaC_dt.ParseFormat(txtFechaC->GetValue(), "%d/%m/%Y %H:%M:%S") ||
        !fi_dt.ParseFormat(txtFI->GetValue(), "%d/%m/%Y %H:%M:%S") ||
        !fv_dt.ParseFormat(txtFV->GetValue(), "%d/%m/%Y %H:%M:%S")) {
        wxMessageBox("Error: El formato de fecha es incorrecto.", "Error", wxOK | wxICON_ERROR);
        SQLExecDirect(hStmt, (SQLWCHAR*)L"ROLLBACK;", SQL_NTS);
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return;
    }

    wxString fechaC = fechaC_dt.FormatISOCombined(' ');
    wxString fi = fi_dt.FormatISOCombined(' ');
    wxString fv = fv_dt.FormatISOCombined(' ');

    wxString descripcion = txtDescripcion->GetValue();
    wxString monedaT = txtMoneda->GetValue().Upper();

    // Recorre las filas del grid y guarda los datos en la base de datos
    for (int row = 0; row < grid->GetNumberRows(); ++row) {
        wxString idHab = grid->GetCellValue(row, 0);
        wxString um = grid->GetCellValue(row, 1);
        wxString tipoHabitacion = grid->GetCellValue(row, 2);
        wxString moneda = grid->GetCellValue(row, 3);
        wxString precio01Str = grid->GetCellValue(row, 4);
        wxString precio02Str = grid->GetCellValue(row, 5);
        wxString precio03Str = grid->GetCellValue(row, 6);

        // Validar que los campos obligatorios no estén vacíos
        if (idHab.IsEmpty() || tipoHabitacion.IsEmpty() || moneda.IsEmpty()) {
            continue; // Ignorar filas incompletas
        }

        // Convertir precios a formato numérico
        double precio01 = 0.0, precio02 = 0.0, precio03 = 0.0;
        precio01Str.ToDouble(&precio01);
        precio02Str.ToDouble(&precio02);
        precio03Str.ToDouble(&precio03);

        // Convertir y vincular el IdHab
        long idHabVal;
        if (!idHab.ToLong(&idHabVal)) {
            wxMessageBox("Error: El valor de IdHab no es un número válido.", "Error de Validación", wxOK | wxICON_WARNING);
            SQLExecDirect(hStmt, (SQLWCHAR*)L"ROLLBACK;", SQL_NTS);
            SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
            return;
        }

        // Vincular los parámetros a la consulta preparada
        SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &siguienteOperacion, 0, NULL);
        SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, operacionID.length(), 0, (SQLPOINTER)operacionID.wc_str(), 0, NULL);
        SQLBindParameter(hStmt, 3, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, fechaC.length(), 0, (SQLPOINTER)fechaC.wc_str(), 0, NULL);
        SQLBindParameter(hStmt, 4, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, descripcion.length(), 0, (SQLPOINTER)descripcion.wc_str(), 0, NULL);
        SQLBindParameter(hStmt, 5, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, fi.length(), 0, (SQLPOINTER)fi.wc_str(), 0, NULL);
        SQLBindParameter(hStmt, 6, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, fv.length(), 0, (SQLPOINTER)fv.wc_str(), 0, NULL);
        SQLBindParameter(hStmt, 7, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, monedaT.length(), 0, (SQLPOINTER)monedaT.wc_str(), 0, NULL);
        SQLBindParameter(hStmt, 8, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &idHabVal, 0, NULL);
        SQLBindParameter(hStmt, 9, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, um.length(), 0, (SQLPOINTER)um.wc_str(), 0, NULL);
        SQLBindParameter(hStmt, 10, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, tipoHabitacion.length(), 0, (SQLPOINTER)tipoHabitacion.wc_str(), 0, NULL);
        SQLBindParameter(hStmt, 11, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, moneda.length(), 0, (SQLPOINTER)moneda.wc_str(), 0, NULL);
        SQLBindParameter(hStmt, 12, SQL_PARAM_INPUT, SQL_C_DOUBLE, SQL_DOUBLE, 0, 0, &precio01, 0, NULL);
        SQLBindParameter(hStmt, 13, SQL_PARAM_INPUT, SQL_C_DOUBLE, SQL_DOUBLE, 0, 0, &precio02, 0, NULL);
        SQLBindParameter(hStmt, 14, SQL_PARAM_INPUT, SQL_C_DOUBLE, SQL_DOUBLE, 0, 0, &precio03, 0, NULL);
        SQLBindParameter(hStmt, 15, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, usuarioID.length(), 0, (SQLPOINTER)usuarioID.wc_str(), 0, NULL);
        SQLBindParameter(hStmt, 16, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, ordenador.length(), 0, (SQLPOINTER)ordenador.wc_str(), 0, NULL);

        // Ejecutar la consulta preparada
        ret = SQLExecute(hStmt);
        if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
            MostrarErrorODBC(SQL_HANDLE_STMT, hStmt);
            SQLExecDirect(hStmt, (SQLWCHAR*)L"ROLLBACK;", SQL_NTS);  // Revertir los cambios en caso de error
            SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
            return;
        }

        // Liberar el cursor después de cada inserción
        SQLFreeStmt(hStmt, SQL_CLOSE);
    }

    // Confirmar la transacción
    ret = SQLExecDirect(hStmt, (SQLWCHAR*)L"COMMIT;", SQL_NTS);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        SQLExecDirect(hStmt, (SQLWCHAR*)L"ROLLBACK;", SQL_NTS);  // Revertir los cambios si ocurre un error al confirmar
        MostrarErrorODBC(SQL_HANDLE_STMT, hStmt);
    }

    // Liberar el cursor final y el handle
    SQLFreeStmt(hStmt, SQL_CLOSE);
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
}

void HabitacionFrame::OnGuardar(wxCommandEvent& event) {
    // Verificar si el cuadro de texto txtMoneda está vacío
    if (txtMoneda->GetValue().Trim().IsEmpty()) {
        wxMessageBox("Error: El campo de 'Moneda' está vacío. Complete este campo antes de guardar.",
            "Error de Validación", wxOK | wxICON_ERROR);
        txtMoneda->SetFocus();
        return;
    }

    if (txtDescripcion->GetValue().Trim().IsEmpty()) {
        wxMessageBox("Error: El campo de 'Descripcion' está vacío. Complete este campo antes de guardar.",
            "Error de Validación", wxOK | wxICON_ERROR);
        txtDescripcion->SetFocus();
        return;
    }

    // Verificar si la tabla está completamente vacía
    if (IsGridEmpty()) {
        wxMessageBox("Error: La tabla está completamente vacía. No se puede proceder con el guardado.",
            "Advertencia", wxOK | wxICON_WARNING);
        return;
    }

    wxString operacionActual = txtOperacion->GetValue();

    // Verificación de duplicados SOLO en modo nuevo
    if (estabaEnModoNuevo) {
        if (NumeroOperacionExiste(operacionActual)) {
            wxMessageBox("El número de operación ya existe. Se generará uno nuevo.",
                "Número de Operación Duplicado", wxOK | wxICON_INFORMATION);

            int siguienteOperacion = ObtenerSiguienteOperacion();
            txtOperacion->SetValue(wxString::Format("%08d", siguienteOperacion));
            return; // Salir para que el usuario intente guardar con el nuevo número
        }
    }

    // Validar las filas de la tabla para datos duplicados
    bool datosValidos = true;
    std::set<wxString> idHabSet; // Usar un conjunto para verificar duplicados en IdHab

    for (int row = 0; row < grid->GetNumberRows(); ++row) {
        // Verificar si la fila actual está completamente vacía
        bool filaVacia = true;
        for (int col = 0; col < grid->GetNumberCols(); ++col) {
            if (!grid->GetCellValue(row, col).Trim().IsEmpty()) {
                filaVacia = false;
                break;
            }
        }
        if (filaVacia) continue; // Continuar si la fila está vacía

        // Validar las celdas de la fila
        wxString idHab = grid->GetCellValue(row, 0).Trim().Trim(false);
        wxString tipoHabitacion = grid->GetCellValue(row, 2).Trim().Trim(false);
        wxString moneda = grid->GetCellValue(row, 3).Trim().Trim(false);

        // Verificación de valores vacíos
        if (idHab.IsEmpty() || tipoHabitacion.IsEmpty() || moneda.IsEmpty()) {
            wxMessageBox(wxString::Format("Error: Falta el valor en una celda en la fila %d.", row + 1),
                "Error de Validación", wxOK | wxICON_WARNING);
            grid->SetGridCursor(row, 0);
            grid->MakeCellVisible(row, 0);
            datosValidos = false;
            break;
        }

        // Verificación de duplicados en IdHab
        if (idHabSet.find(idHab) != idHabSet.end()) {
            wxMessageBox("Error: El valor de IdHab está repetido. Ingrese un valor único.",
                "Error de Validación", wxOK | wxICON_WARNING);
            grid->SetGridCursor(row, 0);
            grid->MakeCellVisible(row, 0);
            grid->SelectBlock(row, 0, row, 0);
            grid->SetFocus();
            return; // Salir sin guardar
        }
        idHabSet.insert(idHab); // Agregar el IdHab al conjunto para validación
    }

    if (!datosValidos) return; // Salir si hay errores en los datos

    // Guardar o actualizar según el modo (nuevo o edición)
    if (estabaEnModoNuevo) {
        GuardarDatosEnBaseDeDatos(); // Insertar nueva operación
    }
    else {
        ActualizarDatosEnBaseDeDatos(); // Actualizar la operación existente
    }

    // Desactivar modo edición y limpiar estado
    enModoEdicion = false;
    estabaEnModoNuevo = false;

    // Deshabilitar los inputs y actualizar los botones
    EnableInputs(false);
    UpdateButtonStates(false, false, false);

    // Habilitar el botón Exportar
    btnExportar->Enable(true);

    // Llamada a la función que actualiza los botones de navegación
    ActualizarEstadoBotonesNavegacion();

    // Posicionar el cursor en la primera celda visible
    if (grid->GetNumberRows() > 0) {
        grid->SetGridCursor(0, 0);
        grid->MakeCellVisible(0, 0);
        grid->SetFocus();
    }

    // Desactivar cualquier selección en el grid para evitar restos visuales
    grid->ClearSelection();
}

void HabitacionFrame::ActualizarEstadoBotonesNavegacion() {
    int numeroOperacion = wxAtoi(txtOperacion->GetValue());
    int primeraOperacion = ObtenerMinimoOperacion();
    int ultimaOperacion = ObtenerMaximoOperacion();

    if (numeroOperacion == primeraOperacion) {
        btnIzquierda->Enable(false);  // Deshabilitar botón "<"
    }
    else {
        btnIzquierda->Enable(true);   // Habilitar botón "<"
    }

    if (numeroOperacion == ultimaOperacion) {
        btnDerecha->Enable(false);  // Deshabilitar botón ">"
    }
    else {
        btnDerecha->Enable(true);   // Habilitar botón ">"
    }
}

bool HabitacionFrame::NumeroOperacionExiste(const wxString& numeroOperacion) {
    SQLHSTMT hStmt = NULL;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

    wxString query = wxString::Format(L"SELECT COUNT(*) FROM PrecioHabitacion WHERE Operacion = %d;",
        wxAtoi(numeroOperacion));
    SQLRETURN ret = SQLExecDirect(hStmt, (SQLWCHAR*)query.wc_str(), SQL_NTS);

    int count = 0;
    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
        SQLFetch(hStmt);
        SQLGetData(hStmt, 1, SQL_C_LONG, &count, 0, NULL);
    }
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);

    return count > 0;
}

void HabitacionFrame::ActualizarDatosEnBaseDeDatos() {
    // Verificar si la conexión está abierta, si no, intentar conectarse a la base de datos
    if (!conexionAbierta && !ConectarBaseDeDatos()) {
        wxMessageBox("Error al conectar a la base de datos.", "Error", wxOK | wxICON_ERROR);
        return;
    }

    SQLHSTMT hStmt = NULL;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

    // Iniciar la transacción
    SQLRETURN ret = SQLExecDirect(hStmt, (SQLWCHAR*)L"BEGIN TRANSACTION;", SQL_NTS);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        MostrarErrorODBC(SQL_HANDLE_STMT, hStmt);
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return;
    }

    int numeroOperacion = wxAtoi(txtOperacion->GetValue());
    wxString usuarioID = usuarioAutenticado;
    wxString ordenador = wxGetHostName();

    // Recuperar las fechas de los campos de texto y formatearlas
    wxDateTime fechaC_dt, fi_dt, fv_dt;
    fechaC_dt.ParseFormat(txtFechaC->GetValue(), "%d/%m/%Y %H:%M:%S");
    fi_dt.ParseFormat(txtFI->GetValue(), "%d/%m/%Y %H:%M:%S");
    fv_dt.ParseFormat(txtFV->GetValue(), "%d/%m/%Y %H:%M:%S");

    wxString fechaC = fechaC_dt.FormatISOCombined(' ');
    wxString fi = fi_dt.FormatISOCombined(' ');
    wxString fv = fv_dt.FormatISOCombined(' ');

    // Obtener el valor de txtOperacion como OperacionID, quitando los ceros a la izquierda
    wxString operacionID = txtOperacion->GetValue();
    operacionID.Trim(false);  // Elimina los ceros a la izquierda

    // Obtener los valores de txtMoneda y txtDescripcion
    wxString monedaT = txtMoneda->GetValue().Trim().Upper();
    wxString descripcion = txtDescripcion->GetValue().Trim();

    // --- PASO 1: Verificar si alguno de los TextCtrl (fechas, descripción, moneda) ha sido modificado ---
    bool cambioGlobal = txtMoneda->IsModified() || txtDescripcion->IsModified() ||
        txtFechaC->IsModified() || txtFI->IsModified() || txtFV->IsModified();

    // --- PASO 2: Recuperar los IdHab actuales en la base de datos para esta operación ---
    std::map<int, std::vector<wxString>> datosEnBD;
    wxString selectQuery = wxString::Format(L"SELECT IdHab, UM, TipoHabitacion, Moneda, Precio01, Precio02, Precio03 FROM PrecioHabitacion WHERE Operacion = %d ORDER BY PrecioHabitacionID;", numeroOperacion);
    ret = SQLExecDirect(hStmt, (SQLWCHAR*)selectQuery.wc_str(), SQL_NTS);

    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
        int idHab;
        SQLWCHAR um[50], tipoHabitacion[255], moneda[10];
        double precio01, precio02, precio03;

        while (SQLFetch(hStmt) == SQL_SUCCESS) {
            SQLGetData(hStmt, 1, SQL_C_LONG, &idHab, 0, NULL);
            SQLGetData(hStmt, 2, SQL_C_WCHAR, um, sizeof(um), NULL);
            SQLGetData(hStmt, 3, SQL_C_WCHAR, tipoHabitacion, sizeof(tipoHabitacion), NULL);
            SQLGetData(hStmt, 4, SQL_C_WCHAR, moneda, sizeof(moneda), NULL);
            SQLGetData(hStmt, 5, SQL_C_DOUBLE, &precio01, 0, NULL);
            SQLGetData(hStmt, 6, SQL_C_DOUBLE, &precio02, 0, NULL);
            SQLGetData(hStmt, 7, SQL_C_DOUBLE, &precio03, 0, NULL);

            datosEnBD[idHab] = { wxString(um), wxString(tipoHabitacion), wxString(moneda),
                                 wxString::Format("%.4f", precio01), wxString::Format("%.4f", precio02), wxString::Format("%.4f", precio03) };
        }
    }
    else {
        MostrarErrorODBC(SQL_HANDLE_STMT, hStmt);
        SQLExecDirect(hStmt, (SQLWCHAR*)L"ROLLBACK;", SQL_NTS);
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return;
    }
    SQLFreeStmt(hStmt, SQL_CLOSE);

    // --- PASO 3: Iterar sobre las filas actuales del grid ---
    std::set<int> idHabsProcesados;
    for (int row = 0; row < grid->GetNumberRows(); ++row) {
        wxString idHabStr = grid->GetCellValue(row, 0).Trim();
        if (idHabStr.IsEmpty()) continue;  // Ignorar filas vacías

        long idHab;
        idHabStr.ToLong(&idHab);
        wxString um = grid->GetCellValue(row, 1);
        wxString tipoHabitacion = grid->GetCellValue(row, 2);
        wxString moneda = grid->GetCellValue(row, 3);
        wxString precio01Str = grid->GetCellValue(row, 4);
        wxString precio02Str = grid->GetCellValue(row, 5);
        wxString precio03Str = grid->GetCellValue(row, 6);

        double precio01 = 0.0, precio02 = 0.0, precio03 = 0.0;
        precio01Str.ToDouble(&precio01);
        precio02Str.ToDouble(&precio02);
        precio03Str.ToDouble(&precio03);

        // --- PASO 4: Si el IdHab ya está en la base de datos, verificar si se ha modificado ---
        if (datosEnBD.find(idHab) != datosEnBD.end()) {
            std::vector<wxString> datosOriginales = datosEnBD[idHab];

            // Comprobar si ha habido algún cambio en los valores de la fila actual o si hubo un cambio global
            bool modificado = (um != datosOriginales[0] ||
                tipoHabitacion != datosOriginales[1] ||
                moneda != datosOriginales[2] ||
                precio01Str != datosOriginales[3] ||
                precio02Str != datosOriginales[4] ||
                precio03Str != datosOriginales[5] ||
                cambioGlobal);  // Si hubo un cambio en los campos globales

            // Solo hacer UPDATE si hubo cambios en los datos de la fila o si hubo un cambio global
            if (modificado) {
                wxString updateQuery = L"UPDATE PrecioHabitacion SET UM = ?, TipoHabitacion = ?, Moneda = ?, Precio01 = ?, Precio02 = ?, Precio03 = ?, Descripcion = ?, MonedaT = ?, FechaC = ?, FI = ?, FV = ?, UsuarioID = ?, Ordenador = ? WHERE Operacion = ? AND IdHab = ?;";
                ret = SQLPrepare(hStmt, (SQLWCHAR*)updateQuery.wc_str(), SQL_NTS);
                if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
                    MostrarErrorODBC(SQL_HANDLE_STMT, hStmt);
                    SQLExecDirect(hStmt, (SQLWCHAR*)L"ROLLBACK;", SQL_NTS);
                    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
                    return;
                }

                // Vincular los parámetros para la actualización
                SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, um.length(), 0, (SQLPOINTER)um.wc_str(), 0, NULL);
                SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, tipoHabitacion.length(), 0, (SQLPOINTER)tipoHabitacion.wc_str(), 0, NULL);
                SQLBindParameter(hStmt, 3, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, moneda.length(), 0, (SQLPOINTER)moneda.wc_str(), 0, NULL);
                SQLBindParameter(hStmt, 4, SQL_PARAM_INPUT, SQL_C_DOUBLE, SQL_DOUBLE, 0, 0, &precio01, 0, NULL);
                SQLBindParameter(hStmt, 5, SQL_PARAM_INPUT, SQL_C_DOUBLE, SQL_DOUBLE, 0, 0, &precio02, 0, NULL);
                SQLBindParameter(hStmt, 6, SQL_PARAM_INPUT, SQL_C_DOUBLE, SQL_DOUBLE, 0, 0, &precio03, 0, NULL);
                SQLBindParameter(hStmt, 7, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, descripcion.length(), 0, (SQLPOINTER)descripcion.wc_str(), 0, NULL);
                SQLBindParameter(hStmt, 8, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, monedaT.length(), 0, (SQLPOINTER)monedaT.wc_str(), 0, NULL);
                SQLBindParameter(hStmt, 9, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, fechaC.length(), 0, (SQLPOINTER)fechaC.wc_str(), 0, NULL);
                SQLBindParameter(hStmt, 10, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, fi.length(), 0, (SQLPOINTER)fi.wc_str(), 0, NULL);
                SQLBindParameter(hStmt, 11, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, fv.length(), 0, (SQLPOINTER)fv.wc_str(), 0, NULL);
                SQLBindParameter(hStmt, 12, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, usuarioID.length(), 0, (SQLPOINTER)usuarioID.wc_str(), 0, NULL);
                SQLBindParameter(hStmt, 13, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, ordenador.length(), 0, (SQLPOINTER)ordenador.wc_str(), 0, NULL);
                SQLBindParameter(hStmt, 14, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &numeroOperacion, 0, NULL);
                SQLBindParameter(hStmt, 15, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &idHab, 0, NULL);

                ret = SQLExecute(hStmt);
                if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
                    MostrarErrorODBC(SQL_HANDLE_STMT, hStmt);
                    SQLExecDirect(hStmt, (SQLWCHAR*)L"ROLLBACK;", SQL_NTS);
                    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
                    return;
                }

                SQLFreeStmt(hStmt, SQL_CLOSE);  // Liberar el cursor después del UPDATE
            }

            idHabsProcesados.insert(idHab);  // Marcar el IdHab como procesado
        }
        // --- PASO 5: Si el IdHab no está en la base de datos, hacer un INSERT ---
        else {
            wxString insertQuery = L"INSERT INTO PrecioHabitacion (Operacion, OperacionID, FechaC, FI, FV, IdHab, UM, TipoHabitacion, Moneda, Precio01, Precio02, Precio03, MonedaT, Descripcion, UsuarioID, Ordenador) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";
            ret = SQLPrepare(hStmt, (SQLWCHAR*)insertQuery.wc_str(), SQL_NTS);
            if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
                MostrarErrorODBC(SQL_HANDLE_STMT, hStmt);
                SQLExecDirect(hStmt, (SQLWCHAR*)L"ROLLBACK;", SQL_NTS);
                SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
                return;
            }

            // Vincular los parámetros para el INSERT
            SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &numeroOperacion, 0, NULL);
            SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, operacionID.length(), 0, (SQLPOINTER)operacionID.wc_str(), 0, NULL);
            SQLBindParameter(hStmt, 3, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, fechaC.length(), 0, (SQLPOINTER)fechaC.wc_str(), 0, NULL);
            SQLBindParameter(hStmt, 4, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, fi.length(), 0, (SQLPOINTER)fi.wc_str(), 0, NULL);
            SQLBindParameter(hStmt, 5, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, fv.length(), 0, (SQLPOINTER)fv.wc_str(), 0, NULL);
            SQLBindParameter(hStmt, 6, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &idHab, 0, NULL);
            SQLBindParameter(hStmt, 7, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, um.length(), 0, (SQLPOINTER)um.wc_str(), 0, NULL);
            SQLBindParameter(hStmt, 8, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, tipoHabitacion.length(), 0, (SQLPOINTER)tipoHabitacion.wc_str(), 0, NULL);
            SQLBindParameter(hStmt, 9, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, moneda.length(), 0, (SQLPOINTER)moneda.wc_str(), 0, NULL);
            SQLBindParameter(hStmt, 10, SQL_PARAM_INPUT, SQL_C_DOUBLE, SQL_DOUBLE, 0, 0, &precio01, 0, NULL);
            SQLBindParameter(hStmt, 11, SQL_PARAM_INPUT, SQL_C_DOUBLE, SQL_DOUBLE, 0, 0, &precio02, 0, NULL);
            SQLBindParameter(hStmt, 12, SQL_PARAM_INPUT, SQL_C_DOUBLE, SQL_DOUBLE, 0, 0, &precio03, 0, NULL);
            SQLBindParameter(hStmt, 13, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, monedaT.length(), 0, (SQLPOINTER)monedaT.wc_str(), 0, NULL);
            SQLBindParameter(hStmt, 14, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, descripcion.length(), 0, (SQLPOINTER)descripcion.wc_str(), 0, NULL);
            SQLBindParameter(hStmt, 15, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, usuarioID.length(), 0, (SQLPOINTER)usuarioID.wc_str(), 0, NULL);
            SQLBindParameter(hStmt, 16, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, ordenador.length(), 0, (SQLPOINTER)ordenador.wc_str(), 0, NULL);

            ret = SQLExecute(hStmt);
            if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
                MostrarErrorODBC(SQL_HANDLE_STMT, hStmt);
                SQLExecDirect(hStmt, (SQLWCHAR*)L"ROLLBACK;", SQL_NTS);
                SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
                return;
            }

            SQLFreeStmt(hStmt, SQL_CLOSE);  // Liberar el cursor después del INSERT
        }
    }

    // --- PASO 6: Eliminar los IdHab que quedaron en la base de datos pero no están en el grid ---
    for (const auto& entry : datosEnBD) {
        int idHab = entry.first;
        if (idHabsProcesados.find(idHab) == idHabsProcesados.end()) {
            wxString deleteQuery = L"DELETE FROM PrecioHabitacion WHERE Operacion = ? AND IdHab = ?;";
            ret = SQLPrepare(hStmt, (SQLWCHAR*)deleteQuery.wc_str(), SQL_NTS);
            if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
                MostrarErrorODBC(SQL_HANDLE_STMT, hStmt);
                SQLExecDirect(hStmt, (SQLWCHAR*)L"ROLLBACK;", SQL_NTS);
                SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
                return;
            }

            // Vincular los parámetros para el DELETE
            SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &numeroOperacion, 0, NULL);
            SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &idHab, 0, NULL);

            ret = SQLExecute(hStmt);
            if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
                MostrarErrorODBC(SQL_HANDLE_STMT, hStmt);
                SQLExecDirect(hStmt, (SQLWCHAR*)L"ROLLBACK;", SQL_NTS);
                SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
                return;
            }

            SQLFreeStmt(hStmt, SQL_CLOSE);  // Liberar el cursor después del DELETE
        }
    }

    // Confirmar la transacción después de realizar todos los cambios
    ret = SQLExecDirect(hStmt, (SQLWCHAR*)L"COMMIT;", SQL_NTS);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        MostrarErrorODBC(SQL_HANDLE_STMT, hStmt);
        SQLExecDirect(hStmt, (SQLWCHAR*)L"ROLLBACK;", SQL_NTS);
    }

    // Liberar el handle de la sentencia SQL
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);

    // --- PASO 7: Desmarcar los TextCtrl como modificados después de guardar ---
    txtMoneda->SetModified(false);
    txtDescripcion->SetModified(false);
    txtFechaC->SetModified(false);
    txtFI->SetModified(false);
    txtFV->SetModified(false);
}

bool HabitacionFrame::HayRegistrosEnTabla() {
    SQLHSTMT hStmt = NULL;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

    // Consulta SQL para contar los registros en la tabla
    wxString query = L"SELECT COUNT(*) FROM PrecioHabitacion;";
    SQLRETURN ret = SQLExecDirect(hStmt, (SQLWCHAR*)query.wc_str(), SQL_NTS);

    int count = 0;

    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
        SQLFetch(hStmt);
        SQLGetData(hStmt, 1, SQL_C_LONG, &count, 0, NULL);
    }

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);

    // Retornar verdadero si hay registros, falso si la tabla está vacía
    return count > 0;
}

void HabitacionFrame::OnNuevo(wxCommandEvent& event) {
    // Intentar conectar a la base de datos antes de cualquier operación
    if (!conexionAbierta) {
        if (!ConectarBaseDeDatos()) {
            wxMessageBox("Error al conectar a la base de datos.", "Error de Conexión", wxOK | wxICON_ERROR);
            return; // Salir si no se pudo conectar
        }
    }

    EnableInputs(true);
    ClearInputs();
    SetDefaultDates();
    UpdateButtonStates(true, false, false);

    // Obtener y mostrar el siguiente número de operación
    int siguienteOperacion = ObtenerSiguienteOperacion();
    txtOperacion->SetValue(wxString::Format("%08d", siguienteOperacion)); // Formato de 8 dígitos con ceros a la izquierda

    // Deshabilitar los botones de navegación
    btnIzquierda->Enable(false);
    btnDerecha->Enable(false);

    // Deshabilitar el botón Exportar mientras se está en modo nuevo
    btnExportar->Enable(false);

    // Establecer la variable de estado a true
    estabaEnModoNuevo = true;

    // Inicializar el grid en modo lectura
    InicializarGridEnModoLectura();  // Sin argumento
}

bool enModoEdicion = false;

void HabitacionFrame::OnEditar(wxCommandEvent& event) {
    EnableInputs(true);  // Habilita la edición de los inputs
    UpdateButtonStates(false, true, false);  // Actualiza los botones

    btnIzquierda->Enable(false);
    btnDerecha->Enable(false);
    btnExportar->Enable(false);  // Deshabilitar exportación en modo edición

    enModoEdicion = true;  // Activar el modo edición
    estabaEnModoNuevo = false;  // Asegurar que no estamos en modo nuevo

    // Inicializar el grid en modo lectura
    InicializarGridEnModoLectura();  // Sin argumento
}

void HabitacionFrame::OnCancelar(wxCommandEvent& event) {
    // Deshabilitar los campos de entrada y actualizar los botones.
    EnableInputs(false);
    UpdateButtonStates(false, false, false);

    if (estabaEnModoNuevo) {
        // Si estaba en modo "Nuevo", verificar si hay registros en la base de datos.
        if (!HayRegistrosEnTabla()) {
            ClearInputs();
            btnNuevo->Enable(true);
            btnGuardar->Enable(false);
            btnEditar->Enable(false);
            btnEliminar->Enable(false);
            btnBuscar->Enable(false);
            btnCancelar->Enable(false);
            btnIzquierda->Enable(false);
            btnDerecha->Enable(false);
            btnExportar->Enable(false);

            // Asegurar que se restablezcan las banderas de estado
            estabaEnModoNuevo = false;
            enModoEdicion = false;  // Restablecer enModoEdicion por seguridad

            // Limpiar cualquier selección previa y posicionar el cursor en la primera fila y columna.
            grid->ClearSelection();  // Limpiar cualquier selección previa
            grid->SetGridCursor(0, 0);  // Colocar el cursor en la primera fila y columna
            grid->MakeCellVisible(0, 0);  // Asegurarse de que la celda inicial sea visible
            grid->SetFocus();  // Enfocar el grid
            return;
        }
        else {
            // Cargar la última operación disponible.
            int ultimaOperacion = ObtenerMaximoOperacion();
            txtOperacion->SetValue(wxString::Format("%08d", ultimaOperacion));
            CargarDatosOperacion(ultimaOperacion);

            // Asegurar que se restablezcan las banderas de estado
            estabaEnModoNuevo = false;
            enModoEdicion = false;
        }
    }
    else {
        // Si se cancela la edición, recargar los datos de la operación actual.
        int numeroOperacion = wxAtoi(txtOperacion->GetValue());
        CargarDatosOperacion(numeroOperacion);

        // Restablecer las banderas de estado después de cancelar la edición
        enModoEdicion = false;
    }

    // Asegurar que los botones de navegación reflejen correctamente los límites.
    ActualizarEstadoBotonesNavegacion();

    // Limpiar cualquier selección previa y posicionar el cursor en la primera fila y columna.
    grid->ClearSelection();  // Limpiar cualquier selección previa
    grid->SetGridCursor(0, 0);  // Colocar el cursor en la primera fila y columna
    grid->MakeCellVisible(0, 0);  // Asegurarse de que la celda inicial sea visible
    grid->SetFocus();  // Enfocar el grid
}

void HabitacionFrame::OnEliminar(wxCommandEvent& event) {
    wxString operacionID = txtOperacion->GetValue().Trim();

    if (operacionID.IsEmpty() || operacionID == "00000000") {
        wxMessageBox("Operación Inválida: El número de operación no es válido o está vacío.",
            "Error", wxOK | wxICON_ERROR);
        return;
    }

    int respuesta = wxMessageBox(wxString::Format("¿Está seguro de que desea eliminar la operación %s?", operacionID),
        "Confirmar eliminación", wxYES_NO | wxICON_QUESTION);

    if (respuesta == wxNO) {
        return;
    }

    // Evitar operaciones en controles mientras se destruyen
    this->Disable();  // Nueva línea: Deshabilita la interfaz durante la operación

    if (!conexionAbierta && !ConectarBaseDeDatos()) {
        wxMessageBox("Error al conectar a la base de datos.",
            "Error", wxOK | wxICON_ERROR);
        this->Enable();  // Nueva línea: Asegura que la interfaz se habilite incluso tras un error
        return;
    }

    SQLHSTMT hStmt = NULL;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

    wxString deleteQuery = wxString::Format(L"DELETE FROM PrecioHabitacion WHERE OperacionID = ?;");
    SQLRETURN ret = SQLPrepare(hStmt, (SQLWCHAR*)deleteQuery.wc_str(), SQL_NTS);

    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        MostrarErrorODBC(SQL_HANDLE_STMT, hStmt);
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        this->Enable();  // Nueva línea: Asegura que la interfaz se habilite incluso tras un error
        return;
    }

    SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
        operacionID.length(), 0, (SQLPOINTER)operacionID.wc_str(), 0, NULL);
    ret = SQLExecute(hStmt);

    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        MostrarErrorODBC(SQL_HANDLE_STMT, hStmt);
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        this->Enable();  // Nueva línea: Asegura que la interfaz se habilite incluso tras un error
        return;
    }

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);

    // Actualiza el grid y asegura que los recursos se limpien adecuadamente
    LimpiarVentana();

    // Verificar si la base de datos quedó vacía después de la eliminación
    if (!HayRegistrosEnTabla()) {
        ClearInputs();
        btnNuevo->Enable(true);
        btnGuardar->Enable(false);
        btnEditar->Enable(false);
        btnEliminar->Enable(false);
        btnBuscar->Enable(false);
        btnCancelar->Enable(false);
        btnIzquierda->Enable(false);
        btnDerecha->Enable(false);
        btnExportar->Enable(false);
        this->Enable();  // Re-habilita la ventana
        return;
    }

    // Verificar si solo queda una operación en la tabla
    int primeraOperacion = ObtenerMinimoOperacion();
    int ultimaOperacion = ObtenerMaximoOperacion();

    if (primeraOperacion == ultimaOperacion) {
        btnIzquierda->Enable(false);
        btnDerecha->Enable(false);
        txtOperacion->SetValue(wxString::Format("%08d", primeraOperacion));
        CargarDatosOperacion(primeraOperacion);
    }
    else {
        int siguienteOperacion = ObtenerSiguienteOperacionDespues(operacionID);
        if (siguienteOperacion == 0) {
            txtOperacion->SetValue("00000000");
            LimpiarVentana();
        }
        else {
            txtOperacion->SetValue(wxString::Format("%08d", siguienteOperacion));
            CargarDatosOperacion(siguienteOperacion);
        }
    }

    this->Enable();  // Re-habilita la ventana tras completar el proceso
}

void HabitacionFrame::LimpiarVentana() {
    txtFechaC->Clear();
    txtFI->Clear();
    txtFV->Clear();
    txtDescripcion->Clear();
    txtMoneda->Clear();
    txtOperacion->Clear();
    grid->ClearGrid();
}

void HabitacionFrame::OnBuscar(wxCommandEvent& event) {
    // Actualizar el estado de los botones como en tu implementación actual
    btnNuevo->Enable(true);
    btnGuardar->Enable(false);
    btnEditar->Enable(true);
    btnCancelar->Enable(false);
    btnEliminar->Enable(true);
    btnInsertar->Enable(false);
    btnBorrar->Enable(false);
    btnImportar->Enable(false);
    btnExportar->Enable(false);
    EnableInputs(false);

    // Desactivar los eventos del grid de HabitacionFrame para evitar que se sincronicen
    DesactivarEventosGrid();

    // Crear y mostrar la ventana BuscarFrameHabitacion
    BuscarFrameHabitacion* buscarFrame = new BuscarFrameHabitacion(this, "Buscar Operación", wxDefaultPosition, wxSize(730, 550));

    // Asegurar que al cerrarse la ventana de búsqueda, se reactivan los eventos del grid de HabitacionFrame
    buscarFrame->Bind(wxEVT_CLOSE_WINDOW, [this](wxCloseEvent& evt) {
        ActivarEventosGrid();  // Reactivar los eventos del grid
        evt.Skip();            // Permitir que el evento de cierre continúe
    });

    buscarFrame->Show(true);  // Mostrar la ventana de búsqueda
}

void HabitacionFrame::OnInsertar(wxCommandEvent& event) {
    int numRows = grid->GetNumberRows();
    int filaHabilitada = -1;

    // 1. Buscar la última fila con datos o fila habilitada para edición
    int ultimaFilaConDatos = -1;
    for (int row = 0; row < numRows; ++row) {
        wxString idHabValue = grid->GetCellValue(row, 0).Trim();  // Verificar la columna 0 (IdHab)

        // Si la fila tiene datos o ya ha sido habilitada para edición, actualizar última fila con datos
        if (!idHabValue.IsEmpty() || !grid->IsReadOnly(row, 0)) {
            ultimaFilaConDatos = row;
        }
    }

    // 2. Buscar la primera fila vacía inmediatamente después de la última fila con datos o fila editable
    for (int row = ultimaFilaConDatos + 1; row < numRows; ++row) {
        wxString idHabValue = grid->GetCellValue(row, 0).Trim();  // Verificar la columna 0 (IdHab)

        if (idHabValue.IsEmpty() && grid->IsReadOnly(row, 0)) {  // Si la fila está vacía y en modo lectura
            for (int col = 0; col < grid->GetNumberCols(); ++col) {
                grid->SetReadOnly(row, col, false);  // Habilitar la fila para edición
                grid->SetCellBackgroundColour(row, col, *wxWHITE);  // Cambiar fondo a blanco
            }

            filaHabilitada = row;  // Marcar esta fila como habilitada
            break;  // Detener el ciclo después de habilitar la fila
        }
    }

    // 3. Si no se encontró una fila vacía en modo lectura, agregar una nueva fila y habilitarla
    if (filaHabilitada == -1) {
        grid->AppendRows(1);  // Crear una nueva fila
        filaHabilitada = grid->GetNumberRows() - 1;  // La nueva fila es la última

        // Habilitar la nueva fila para edición
        for (int col = 0; col < grid->GetNumberCols(); ++col) {
            grid->SetReadOnly(filaHabilitada, col, false);  // Habilitar edición
            grid->SetCellBackgroundColour(filaHabilitada, col, *wxWHITE);  // Fondo blanco
        }
    }

    // 4. Posicionar el cursor en la primera celda de la fila habilitada (columna 0)
    grid->SetGridCursor(filaHabilitada, 0);  // Colocar el cursor en la primera celda de la fila (columna 0)
    grid->MakeCellVisible(filaHabilitada, 0);  // Asegurarse de que la fila sea visible
    grid->SetFocus();  // Establecer el foco en la grilla

    // 5. Cambiar el color de selección para que no se note (fondo blanco)
    grid->SetSelectionBackground(*wxWHITE);  // Hacer el fondo de la selección blanco
    grid->SetSelectionForeground(*wxBLACK);  // Mantener el texto seleccionado de color negro

    // 6. Seleccionar solo la primera celda (columna 0) de la fila habilitada
    grid->ClearSelection();  // Eliminar cualquier selección previa
    grid->SelectBlock(filaHabilitada, 0, filaHabilitada, 0);  // Seleccionar solo la celda de la columna 0
    grid->SetGridCursor(filaHabilitada, 0);  // Reestablecer el cursor en la celda (columna 0)
}

void HabitacionFrame::InicializarGridEnModoLectura() {
    int numRows = grid->GetNumberRows();
    bool hayDatosEnGrid = false;  // Bandera para verificar si hay alguna fila con datos

    for (int row = 0; row < numRows; ++row) {
        wxString idHabValue = grid->GetCellValue(row, 0).Trim();  // Verificar la columna 0 (IdHab)

        if (!idHabValue.IsEmpty()) {
            hayDatosEnGrid = true;  // Indicar que al menos una fila tiene datos

            // Si la fila tiene datos, habilitar la edición solo en modo edición
            if (enModoEdicion) {
                for (int col = 0; col < grid->GetNumberCols(); ++col) {
                    grid->SetReadOnly(row, col, false);  // Desbloquear la fila para edición
                    grid->SetCellBackgroundColour(row, col, *wxWHITE);  // Mantener el fondo blanco
                }
            }
            else {
                // Si no estamos en modo edición, bloquear las filas con datos
                for (int col = 0; col < grid->GetNumberCols(); ++col) {
                    grid->SetReadOnly(row, col, true);  // Bloquear la fila
                    grid->SetCellBackgroundColour(row, col, *wxWHITE);  // Mantener el fondo blanco
                }
            }
        }
        else {
            // Bloquear las filas vacías (modo solo lectura) y hacerlas no seleccionables
            for (int col = 0; col < grid->GetNumberCols(); ++col) {
                grid->SetReadOnly(row, col, true);  // Bloquear la fila
                grid->SetCellBackgroundColour(row, col, *wxWHITE);  // Mantener el fondo blanco
            }
        }
    }

    // Si no hay datos en el grid, asegurarse de que todo está en modo lectura
    if (!hayDatosEnGrid) {
        for (int row = 0; row < numRows; ++row) {
            for (int col = 0; col < grid->GetNumberCols(); ++col) {
                grid->SetReadOnly(row, col, true);  // Poner todas las celdas en modo lectura
                grid->SetCellBackgroundColour(row, col, *wxWHITE);  // Mantener el fondo blanco
            }
        }
    }

    grid->Refresh();  // Refrescar el grid para aplicar los cambios visualmente
}

void HabitacionFrame::OnBorrar(wxCommandEvent& event) {
    int currentRow = grid->GetGridCursorRow();
    int numRows = grid->GetNumberRows();

    // Verificar si hay filas editables en el grid
    bool filaEditableEncontrada = false;
    for (int row = 0; row < numRows; ++row) {
        if (!grid->IsReadOnly(row, 0)) {  // Si alguna fila es editable
            filaEditableEncontrada = true;
            break;
        }
    }

    // Si no hay filas editables, mostrar un mensaje y salir
    if (!filaEditableEncontrada) {
        wxMessageBox("No hay datos para borrar.", "Información", wxOK | wxICON_INFORMATION);
        return;
    }

    // Verificar si la fila actual es editable
    if (!grid->IsReadOnly(currentRow, 0)) {
        // 1. Eliminar la fila actual completamente
        grid->DeleteRows(currentRow, 1);  // Eliminar la fila en la posición actual

        // Actualizar el número de filas tras la eliminación
        numRows = grid->GetNumberRows();

        // 2. Buscar la siguiente fila editable más cercana
        int siguienteFilaEditable = -1;

        // Intentar primero buscar la siguiente fila editable hacia abajo
        for (int row = currentRow; row < numRows; ++row) {
            if (!grid->IsReadOnly(row, 0)) {
                siguienteFilaEditable = row;
                break;
            }
        }

        // Si no se encontró hacia abajo, buscar hacia arriba
        if (siguienteFilaEditable == -1) {
            for (int row = currentRow - 1; row >= 0; --row) {
                if (!grid->IsReadOnly(row, 0)) {
                    siguienteFilaEditable = row;
                    break;
                }
            }
        }

        // 3. Si se encontró una fila editable, mover el cursor a esa fila
        if (siguienteFilaEditable != -1) {
            grid->SetGridCursor(siguienteFilaEditable, 0);  // Colocar el cursor en la primera celda de la fila
            grid->MakeCellVisible(siguienteFilaEditable, 0);  // Asegurarse de que la fila sea visible
        }
        else if (numRows > 0) {
            // Si no hay filas editables, pero quedan filas, mover el cursor a la primera fila
            grid->SetGridCursor(0, 0);  // Mover el cursor a la primera celda
        }

        // 4. Refrescar el grid para reflejar los cambios visualmente
        grid->Refresh();  // Refrescar la grilla para actualizar los colores y visuales
    }
    else {
        wxMessageBox("No hay datos para borrar.", "Información", wxOK | wxICON_INFORMATION);
    }
}

void HabitacionFrame::OnImportarExcel(wxCommandEvent& event) {
    try {
        // Preguntar al usuario si desea eliminar la información anterior
        int respuesta = wxMessageBox("¿Desea eliminar la información anterior?", "Confirmar", wxYES_NO | wxICON_QUESTION);

        bool limpiarGrid = (respuesta == wxYES);

        // Mostrar el diálogo para seleccionar el archivo Excel
        wxFileDialog openFileDialog(this, _("Importar Excel"), "", "", "Excel files (*.xlsx)|*.xlsx", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

        if (openFileDialog.ShowModal() == wxID_CANCEL) {
            return; // El usuario canceló la operación
        }

        wxString path = openFileDialog.GetPath();
        xlnt::workbook wb;
        wb.load(path.ToStdString());  // Cargar el archivo Excel seleccionado

        // Acceder a la primera hoja
        xlnt::worksheet ws = wb.active_sheet();

        // Obtener la cantidad máxima de filas y columnas para determinar el rango de datos
        int requiredRows = ws.highest_row();
        int requiredCols = std::min(static_cast<int>(ws.highest_column().index), 7); // Limitar a 7 columnas

        int filaInicio = 0;

        // Si el usuario elige "Sí", se limpia el grid y se pone en modo de lectura justo antes de la importación
        if (limpiarGrid) {
            grid->ClearGrid();
            for (int row = 0; row < grid->GetNumberRows(); ++row) {
                for (int col = 0; col < grid->GetNumberCols(); ++col) {
                    grid->SetReadOnly(row, col, true); // Poner en modo de lectura
                }
            }
            grid->Refresh();
        }
        else {
            // Si la respuesta es "No", encontrar la primera fila en modo lectura disponible
            for (int row = 0; row < grid->GetNumberRows(); ++row) {
                if (grid->IsReadOnly(row, 0)) {
                    filaInicio = row;
                    break;
                }
            }
        }

        // Asegurarse de que la grilla tenga suficientes filas y columnas
        if (grid->GetNumberRows() < (filaInicio + requiredRows - 1)) {
            grid->AppendRows((filaInicio + requiredRows - 1) - grid->GetNumberRows());
        }

        if (grid->GetNumberCols() < requiredCols) {
            grid->AppendCols(requiredCols - grid->GetNumberCols());
        }

        // Lista para almacenar las celdas con errores
        std::vector<std::pair<int, int>> celdasConError;

        // Llenar la grilla con los datos de Excel comenzando desde la segunda fila
        for (int row = 2; row <= requiredRows; ++row) {
            int filaActual = filaInicio + (row - 2); // Ajustar la fila actual según si hubo limpieza o no
            bool idHabValido = false;  // Variable para controlar si el IdHab es válido
            wxString idHabValue;       // Variable para almacenar el IdHab temporalmente

            for (int col = 0; col < requiredCols; ++col) {
                xlnt::cell cell = ws.cell(xlnt::cell_reference(col + 1, row));
                std::string cellValueStr = cell.to_string();
                wxString cellValue = wxString::FromUTF8(cellValueStr.c_str()).Trim();

                // Manejo de la columna de IdHab
                if (col == 0 && !cellValue.IsEmpty()) {
                    idHabValue = cellValue;
                    long id;
                    if (!cellValue.ToLong(&id) || !VerificarIDHabEnBaseDeDatos(cellValue)) {
                        // Añadir a la lista de errores en lugar de mostrar el mensaje inmediatamente
                        celdasConError.push_back(std::make_pair(filaActual, col));
                        grid->SetCellValue(filaActual, col, cellValue); // Dejar el valor visible para edición

                        // Limpiar U.M y Descripción si el IdHab es incorrecto
                        grid->SetCellValue(filaActual, 1, "");
                        grid->SetCellValue(filaActual, 2, "");
                        continue;  // Saltar la validación adicional y continuar con la siguiente columna
                    }

                    // Si el IDHab es válido, obtener UM y Descripción desde la base de datos
                    wxString umValue, descripcionValue;
                    ObtenerUMYDescripcion(cellValue, umValue, descripcionValue);

                    // Verificar y reemplazar los valores actuales de UM y Descripción si no coinciden con los correctos
                    wxString currentUM = grid->GetCellValue(filaActual, 1).Trim();
                    wxString currentDescripcion = grid->GetCellValue(filaActual, 2).Trim();

                    if (currentUM.IsEmpty() || currentUM != umValue) {
                        grid->SetCellValue(filaActual, 1, umValue); // Reemplazar con el valor correcto
                    }

                    if (currentDescripcion.IsEmpty() || currentDescripcion != descripcionValue) {
                        grid->SetCellValue(filaActual, 2, descripcionValue); // Reemplazar con el valor correcto
                    }

                    grid->SetCellValue(filaActual, 0, idHabValue); // Mantener el IDHab visible si es correcto
                    idHabValido = true;  // Marcar el IDHab como válido
                }

                // Manejo de las columnas de precios
                if ((col == 4 || col == 5 || col == 6)) {
                    double number;
                    if (cellValue.ToDouble(&number)) {
                        cellValue = wxString::Format("%.4f", number);
                    }
                    else {
                        cellValue = "0.0000";  // Formatear valores vacíos o incorrectos a 0.0000
                    }
                    grid->SetCellValue(filaActual, col, cellValue); // Establecer precios, independientemente de la validez de IdHab
                }

                // Mantener otros valores (como Moneda) tal como están o convertirlos si es necesario
                if (col == 3) {
                    grid->SetCellValue(filaActual, col, cellValue.Upper());
                }
            }

            // Hacer la fila editable solo si tiene datos
            for (int col = 0; col < grid->GetNumberCols(); ++col) {
                grid->SetReadOnly(filaActual, col, false);
            }
        }

        // Si hay celdas con errores, posicionarse en la primera y corregir secuencialmente
        if (!celdasConError.empty()) {
            int errorRow = celdasConError.front().first;
            int errorCol = celdasConError.front().second;
            wxMessageBox("Se encontraron errores en los valores de IdHab. Por favor, corrija los errores uno por uno.", "Errores de Importación", wxOK | wxICON_WARNING);
            grid->SetGridCursor(errorRow, errorCol);
            grid->MakeCellVisible(errorRow, errorCol);
            grid->SetFocus();
        }

        // Centrar las columnas después de importar
        CenterGridColumns();

        // Actualizar la columna de Moneda después de importar datos
        UpdateMonedaColumn();
    }
    catch (const std::exception& ex) {
        wxMessageBox(wxString::Format("Error al importar Excel: %s", ex.what()), "Error", wxOK | wxICON_ERROR);
    }
}


void HabitacionFrame::OnExportarExcel(wxCommandEvent& event) {
    try {
        // Obtener la fecha y hora actual
        wxDateTime now = wxDateTime::Now();
        wxString datePart = now.Format("%d%m%Y");  // Formato: DDMMYYYY
        wxString timePart = now.Format("%I%M%S%p"); // Formato: HHMMSSAM/PM

        // Ruta base donde se almacenarán los archivos
        wxString basePath = "D:\\RSC Sistema\\temprsc";

        // Comprobar si la carpeta "temprsc" existe, si no, crearla
        if (!wxDirExists(basePath)) {
            wxMkdir(basePath);
        }

        // Comprobar si ya existe una carpeta con la fecha de hoy
        wxString folderPath = basePath + "\\" + datePart + timePart;

        // Verificar si existe una carpeta para la fecha actual
        bool carpetaExiste = false;
        wxDir dir(basePath);
        wxString subdir;
        bool continuar = dir.GetFirst(&subdir);
        while (continuar) {
            if (subdir.StartsWith(datePart)) {
                carpetaExiste = true;
                folderPath = basePath + "\\" + subdir;  // Usar la carpeta existente
                break;
            }
            continuar = dir.GetNext(&subdir);
        }

        // Si no existe la carpeta para la fecha actual, crearla
        if (!carpetaExiste) {
            wxMkdir(folderPath);
        }

        // Crear el nombre completo del archivo de salida
        wxString exportPath = folderPath + "\\precio_habitaciones.xlsx";

        // Cargar el archivo de plantilla de Excel
        xlnt::workbook wb;
        wb.load("D:\\RSC Sistema\\exportar_precio_habitacion.xlsx");  // Asegúrate de que esta ruta y archivo existan

        // Acceder a la primera hoja
        xlnt::worksheet ws = wb.active_sheet();

        // Escribir los datos de la grilla al archivo Excel, comenzando desde la segunda fila
        for (int row = 0; row < grid->GetNumberRows(); ++row) {
            for (int col = 0; col < grid->GetNumberCols(); ++col) {
                wxString cellValue = grid->GetCellValue(row, col);

                // Determinar el tipo de valor y establecerlo en la celda de Excel
                if (col == 0 || col == 4 || col == 5 || col == 6) {
                    double number;
                    if (cellValue.ToDouble(&number)) {
                        ws.cell(xlnt::cell_reference(col + 1, row + 2)).value(number);
                    }
                    else {
                        ws.cell(xlnt::cell_reference(col + 1, row + 2)).value(cellValue.ToStdString());
                    }
                }
                else {
                    ws.cell(xlnt::cell_reference(col + 1, row + 2)).value(cellValue.ToStdString());
                }
            }
        }

        // Guardar el archivo Excel en la ruta generada
        wb.save(exportPath.ToStdString());

        // Abrir automáticamente el archivo exportado
        ShellExecute(NULL, L"open", exportPath.wc_str(), NULL, NULL, SW_SHOW);
    }
    catch (const std::exception& ex) {
        wxMessageBox(wxString::Format("Error al exportar Excel: %s", ex.what()), "Error", wxOK | wxICON_ERROR);
    }
}

void HabitacionFrame::EnableInputs(bool enable) {
    txtFI->Enable(enable);
    txtFV->Enable(enable);
    txtDescripcion->Enable(enable);
    txtMoneda->Enable(enable);
    grid->Enable(enable);
    btnImportar->Enable(enable);
    btnExportar->Enable(enable);
    btnInsertar->Enable(enable);
    btnBorrar->Enable(enable);
}

void HabitacionFrame::ClearInputs() {
    txtFI->Clear();
    txtFV->Clear();
    txtOperacion->Clear();
    txtDescripcion->Clear();
    txtMoneda->Clear();
    txtFechaC->Clear();
    grid->ClearGrid();
}

void HabitacionFrame::SetDefaultDates() {
    wxDateTime now = wxDateTime::Now();
    txtFI->SetValue(now.Format("%d/%m/%Y %H:%M:%S"));  // Formato correcto para txtFI
    txtFechaC->SetValue(now.Format("%d/%m/%Y %H:%M:%S")); // Formato correcto para txtFechaC
    wxDateTime tomorrow = now.Add(wxTimeSpan(24, 0, 0, 0));
    txtFV->SetValue(tomorrow.Format("%d/%m/%Y %H:%M:%S")); // Formato correcto para txtFV
}

void HabitacionFrame::UpdateButtonStates(bool nuevoSelected, bool editarSelected, bool buscarSelected) {
    btnNuevo->Enable(!nuevoSelected && !editarSelected && !buscarSelected);
    btnBuscar->Enable(!nuevoSelected && !editarSelected);
    btnEliminar->Enable(!nuevoSelected && !editarSelected);
    btnGuardar->Enable(nuevoSelected || editarSelected);
    btnCancelar->Enable(nuevoSelected || editarSelected);
    btnEditar->Enable(!nuevoSelected && !buscarSelected && !editarSelected);
    btnInsertar->Enable(nuevoSelected || editarSelected);
    btnBorrar->Enable(nuevoSelected || editarSelected);
    btnImportar->Enable(nuevoSelected || editarSelected);
    btnExportar->Enable(!(nuevoSelected || editarSelected));
    btnIzquierda->Enable(!(nuevoSelected || editarSelected)); // Habilitar solo si no se está en Nuevo o Editar
    btnDerecha->Enable(!(nuevoSelected || editarSelected));   // Habilitar solo si no se está en Nuevo o Editar
    grid->EnableEditing(nuevoSelected || editarSelected);
    grid->Enable(nuevoSelected || editarSelected);
}

void HabitacionFrame::CenterGridColumns() {
    wxGridCellAttr* attr = new wxGridCellAttr();
    attr->SetAlignment(wxALIGN_CENTER, wxALIGN_CENTER);

    grid->SetColAttr(0, attr->Clone());
    grid->SetColAttr(1, attr->Clone());
    grid->SetColAttr(3, attr->Clone());
    grid->SetColAttr(4, attr->Clone());
    grid->SetColAttr(5, attr->Clone());
    grid->SetColAttr(6, attr->Clone());

    attr->DecRef();
}

void HabitacionFrame::OnClose(wxCloseEvent& event) {
    // Deshabilitar temporalmente el evento para evitar ciclos
    this->Unbind(wxEVT_CLOSE_WINDOW, &HabitacionFrame::OnClose, this);

    if (estabaEnModoNuevo || enModoEdicion) {
        int respuesta = wxMessageBox("Hay cambios sin guardar. ¿Desea cerrar?", "Confirmar cierre", wxYES_NO | wxICON_QUESTION);
        if (respuesta == wxYES) {
            if (mainFrameParent != nullptr) {
                mainFrameParent->Enable();
            }
            event.Skip();  // Continuar con el cierre
        }
        else {
            event.Veto();  // Cancelar el cierre
        }
    }
    else {
        // Si no estás en modo edición o nuevo, solo cerrar
        if (mainFrameParent != nullptr) {
            mainFrameParent->Enable();
        }
        event.Skip();  // Continuar con el cierre sin preguntar
    }

    // Volver a habilitar el evento después de la operación
    this->Bind(wxEVT_CLOSE_WINDOW, &HabitacionFrame::OnClose, this);
}

void HabitacionFrame::OnCellChange(wxGridEvent& event) {
    int col = event.GetCol();
    int row = event.GetRow();
    wxString value = grid->GetCellValue(row, col);

    // Agregar una fila nueva si se está editando la última fila
    if (row == grid->GetNumberRows() - 1) {
        grid->AppendRows(1);
    }

    wxString columnName = grid->GetColLabelValue(col);

    // Detectar si se borró el contenido de IdHab o si se vacía la celda por un error
    if (col == 0 && value.IsEmpty()) {
        // Borrar toda la fila si IdHab está vacío
        for (int i = 0; i < grid->GetNumberCols(); ++i) {
            grid->SetCellValue(row, i, "");
        }
        grid->SetGridCursor(row, col);    // Mantener el cursor en la celda de IDHab
        grid->MakeCellVisible(row, col);  // Asegurarse de que la celda sea visible
        grid->SetFocus();                 // Enfocar la celda para seguir trabajando
        return; // Salir porque toda la fila ha sido borrada
    }

    // Validar valores únicos en la columna IdHab y autocompletar UM y Descripcion
    if (col == 0 && !value.IsEmpty()) {
        long id;
        if (!value.ToLong(&id)) {
            ShowErrorDialog(value, row, columnName);
            grid->SetCellValue(row, col, "");
            grid->SetGridCursor(row, col);
            grid->MakeCellVisible(row, col);
            grid->SetFocus();
            // Aquí aplicamos la misma lógica para borrar la fila entera si el valor es incorrecto
            for (int i = 0; i < grid->GetNumberCols(); ++i) {
                grid->SetCellValue(row, i, "");
            }
            return;  // Se detiene aquí y limpia la fila si hay un error
        }

        // Verificar duplicados en la columna IdHab
        for (int i = 0; i < grid->GetNumberRows(); ++i) {
            if (i != row && grid->GetCellValue(i, 0) == value) {
                wxMessageBox("Error: El valor de IdHab está repetido. Ingrese un valor único.", "Error de Validación", wxOK | wxICON_WARNING);
                grid->SetCellValue(row, col, "");
                grid->SetGridCursor(row, col);
                grid->MakeCellVisible(row, col);
                grid->SetFocus();
                // Borra la fila completa si el valor es duplicado
                for (int i = 0; i < grid->GetNumberCols(); ++i) {
                    grid->SetCellValue(row, i, "");
                }
                return;  // Se detiene aquí y limpia la fila si hay un error
            }
        }

        // Verificar si el IDHab existe en la base de datos
        if (!VerificarIDHabEnBaseDeDatos(value)) {
            wxMessageBox("El IDHab ingresado no existe en la base de datos.", "Error", wxOK | wxICON_ERROR);
            grid->SetCellValue(row, col, ""); // Borrar el valor de la celda
            grid->SetGridCursor(row, col);
            grid->MakeCellVisible(row, col);
            grid->SetFocus();
            // Borra la fila completa si el IDHab no existe
            for (int i = 0; i < grid->GetNumberCols(); ++i) {
                grid->SetCellValue(row, i, "");
            }
            return;  // Se detiene aquí y limpia la fila si hay un error
        }

        // Si el IDHab existe, obtener y completar UM y Descripcion
        wxString umValue, descripcionValue;
        ObtenerUMYDescripcion(value, umValue, descripcionValue);

        grid->SetCellValue(row, 1, umValue);       // Completar UM en la columna 1
        grid->SetCellValue(row, 2, descripcionValue); // Completar Descripcion en la columna 2

        // Restaurar Moneda y Precios si IdHab está presente
        wxString defaultMoneda = txtMoneda->GetValue().Trim().Upper();
        if (grid->GetCellValue(row, 3).IsEmpty()) {
            grid->SetCellValue(row, 3, defaultMoneda);
        }
        for (int precioCol = 4; precioCol <= 6; ++precioCol) {
            if (grid->GetCellValue(row, precioCol).IsEmpty()) {
                grid->SetCellValue(row, precioCol, "0.0000");
            }
        }
        UpdateMonedaColumn();
    }

    // Validar campos numéricos en las columnas de precio
    if ((col == 4 || col == 5 || col == 6) && !grid->GetCellValue(row, 0).IsEmpty()) {
        double number;
        if (!value.ToDouble(&number) || value.IsEmpty()) {
            grid->SetCellValue(row, col, "0.0000"); // Restaurar precios a 0.0000 si están vacíos
        }
        else {
            grid->SetCellValue(row, col, wxString::Format("%.4f", number));
        }
    }

    // Convertir texto a mayúsculas para la columna Moneda y restaurar si IdHab está presente
    if (col == 3) {
        wxString idHabValue = grid->GetCellValue(row, 0).Trim();
        if (!idHabValue.IsEmpty()) {
            wxString defaultMoneda = txtMoneda->GetValue().Trim().Upper();
            if (value.IsEmpty()) {
                grid->SetCellValue(row, col, defaultMoneda); // Restaurar al valor predeterminado de txtMoneda
            }
            else {
                grid->SetCellValue(row, col, value.Upper());
            }
        }
        else {
            grid->SetCellValue(row, col, ""); // Permitir borrar si IdHab está vacío
        }
    }

    event.Skip(); // Permitir que otros manejadores de eventos continúen
}

void HabitacionFrame::ObtenerUMYDescripcion(const wxString& idHab, wxString& umValue, wxString& descripcionValue) {
    SQLHSTMT hStmt = NULL;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

    wxString query = wxString::Format(L"SELECT UM, Descripcion FROM Habitacion WHERE IdHab = '%s';", idHab);
    SQLRETURN ret = SQLExecDirect(hStmt, (SQLWCHAR*)query.wc_str(), SQL_NTS);

    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
        SQLFetch(hStmt);

        SQLWCHAR um[50], descripcion[255];
        SQLGetData(hStmt, 1, SQL_C_WCHAR, um, sizeof(um), NULL);
        SQLGetData(hStmt, 2, SQL_C_WCHAR, descripcion, sizeof(descripcion), NULL);

        umValue = wxString(um);
        descripcionValue = wxString(descripcion);
    }

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
}

void HabitacionFrame::OnMonedaTextChange(wxCommandEvent& event) {
    static bool isHandlingChange = false;

    if (isHandlingChange) {
        event.Skip();
        return;
    }

    isHandlingChange = true;

    wxString value = txtMoneda->GetValue().Upper();
    txtMoneda->SetValue(value);

    UpdateMonedaColumn();

    isHandlingChange = false;

    event.Skip();
}

void HabitacionFrame::UpdateMonedaColumn() {
    wxString value = txtMoneda->GetValue().Upper().Trim();

    for (int row = 0; row < grid->GetNumberRows(); ++row) {
        wxString idHabValue = grid->GetCellValue(row, 0); // Columna de IdHab

        // Si el campo txtMoneda está vacío, vaciar la columna de moneda
        if (value.IsEmpty()) {
            grid->SetCellValue(row, 3, ""); // Limpiar Moneda si txtMoneda está vacío
        }
        else if (!idHabValue.IsEmpty()) {
            // Restaurar Moneda según el valor de txtMoneda
            if (value == "S" || value == "SOLES") {
                grid->SetCellValue(row, 3, "SOLES");
            }
            else if (value == "D" || value == "DOLARES") {
                grid->SetCellValue(row, 3, "DOLARES");
            }
        }
        else {
            grid->SetCellValue(row, 3, ""); // Limpiar Moneda si IdHab está vacío
        }
    }
}


void HabitacionFrame::OnMonedaEnter(wxCommandEvent& event) {
    wxString value = txtMoneda->GetValue().Upper();
    txtMoneda->SetValue(value);
    UpdateMonedaColumn();
    event.Skip();
}

void HabitacionFrame::OnGridKeyDown(wxKeyEvent& event) {
    int keyCode = event.GetKeyCode();

    // Capturar la tecla F1 y verificar si está en la columna de IdHab
    if (keyCode == WXK_F1) {
        int row = grid->GetGridCursorRow();
        int col = grid->GetGridCursorCol();

        // Verificar si la columna es la de IdHab (columna 0)
        if (col == 0) {
            // Abrir la ventana de búsqueda de IdHab
            GridBuscarIDHabFrame* buscarFrame = new GridBuscarIDHabFrame(this, "Buscar ID Habitación", wxDefaultPosition, wxSize(730, 550));
            buscarFrame->Show(true);
            return;  // Evitar que otros manejadores continúen
        }
    }

    // Verificar si la tecla presionada es DELETE o BACKSPACE
    if (keyCode == WXK_DELETE || keyCode == WXK_BACK) {
        int row = grid->GetGridCursorRow();
        int col = grid->GetGridCursorCol();

        // Si una celda está seleccionada
        if (row != -1 && col != -1) {
            wxString idHabValue = grid->GetCellValue(row, 0).Trim();

            // Si se intenta borrar IdHab, limpiar toda la fila
            if (col == 0 && (keyCode == WXK_DELETE || keyCode == WXK_BACK)) {
                for (int i = 0; i < grid->GetNumberCols(); ++i) {
                    grid->SetCellValue(row, i, ""); // Borra todas las celdas de la fila
                }
                grid->DeleteRows(row, 1); // Eliminar la fila si está vacía
            }
            // Si se intenta borrar Moneda y IdHab tiene un valor, restaurar Moneda
            else if (col == 3 && !idHabValue.IsEmpty()) {
                // Restaurar el valor de Moneda utilizando UpdateMonedaColumn
                UpdateMonedaColumn();
            }
            // Si IdHab tiene un valor, restaurar Precios cuando se borran
            else if (!idHabValue.IsEmpty() && col >= 4 && col <= 6) {
                grid->SetCellValue(row, col, "0.0000"); // Restaurar precios a 0.0000
            }
            // Si IdHab está vacío, permitir la limpieza normal de la celda
            else {
                grid->SetCellValue(row, col, ""); // Borra el contenido de la celda
            }
        }
    }
    else {
        event.Skip(); // Permitir que otros eventos de teclado sigan funcionando
    }
}

void HabitacionFrame::OnGridEditorKeyDown(wxKeyEvent& event) {
    int keyCode = event.GetKeyCode();

    // Capturar la tecla F1 durante la edición de la celda
    if (keyCode == WXK_F1) {
        int row = grid->GetGridCursorRow();
        int col = grid->GetGridCursorCol();

        // Verificar si la columna es la de IdHab (columna 0)
        if (col == 0) {
            // Abrir la ventana de búsqueda de IdHab
            GridBuscarIDHabFrame* buscarFrame = new GridBuscarIDHabFrame(this, "Buscar ID Habitación", wxDefaultPosition, wxSize(730, 550));
            buscarFrame->Show(true);
            return;  // Evitar que otros manejadores continúen
        }
    }

    event.Skip(); // Continuar con otros manejadores de eventos de teclado
}

void HabitacionFrame::SetSelectedIDHabValue(const wxString& idHabValue) {
    int row = grid->GetGridCursorRow();  // Obtener la fila actual seleccionada en el grid
    int col = grid->GetGridCursorCol();  // Obtener la columna actual seleccionada en el grid

    // Verifica si estamos en la columna de IdHab (columna 0)
    if (col == 0) {
        // Colocar el valor recibido en la celda seleccionada o editada
        grid->SetCellValue(row, col, idHabValue);

        // Forzar el evento de cambio de celda para que OnCellChange maneje la lógica de ajuste
        wxGridEvent event(
            grid->GetId(),                    // ID del grid
            wxEVT_GRID_CELL_CHANGED,          // Tipo de evento que simula una edición
            grid,                             // El grid que generó el evento
            row,                              // Fila afectada
            col                               // Columna afectada
            );

        // Llama directamente a OnCellChange con el evento simulado
        OnCellChange(event);

        // Posicionar el cursor en la celda que acaba de ser llenada
        grid->SetGridCursor(row, col);      // Mueve el cursor a la celda
        grid->MakeCellVisible(row, col);    // Asegura que la celda esté visible
        grid->SetFocus();                   // Pone el foco en la grilla para permitir la edición
    }
}

void HabitacionFrame::SetSelectedDescripcionValue(const wxString& descripcionValue) {
    int row = grid->GetGridCursorRow();
    int col = 2; // Columna 2 para Descripcion
    grid->SetCellValue(row, col, descripcionValue);

    // Simula el evento de cambio para aplicar ajustes y validaciones
    wxGridEvent event(grid->GetId(), wxEVT_GRID_CELL_CHANGED, grid, row, col);
    OnCellChange(event);
}

void HabitacionFrame::SetSelectedUMValue(const wxString& umValue) {
    int row = grid->GetGridCursorRow();
    int col = 1; // Columna 1 para UM
    grid->SetCellValue(row, col, umValue);

    // Simula el evento de cambio para aplicar ajustes y validaciones
    wxGridEvent event(grid->GetId(), wxEVT_GRID_CELL_CHANGED, grid, row, col);
    OnCellChange(event);
}

bool HabitacionFrame::IsGridEmpty() {
    // Recorremos todas las filas de la grilla
    for (int row = 0; row < grid->GetNumberRows(); ++row) {
        bool rowIsComplete = true; // Asumimos que la fila está completa

        // Recorremos todas las columnas de la fila actual
        for (int col = 0; col < grid->GetNumberCols(); ++col) {
            // Si encontramos una celda vacía, la fila no está completa
            if (grid->GetCellValue(row, col).IsEmpty()) {
                rowIsComplete = false;
                break; // No necesitamos seguir revisando las celdas de esta fila
            }
        }

        // Si encontramos una fila que está completamente llena, la grilla no está vacía
        if (rowIsComplete) {
            return false;
        }
    }

    // Si ninguna fila está completamente llena, entonces consideramos la grilla vacía
    return true;
}

bool HabitacionFrame::VerificarIDHabEnBaseDeDatos(const wxString& idHab) {
    SQLHSTMT hStmt = NULL;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt); // Asegúrate de tener una conexión válida a la base de datos

    wxString query = wxString::Format(L"SELECT COUNT(*) FROM Habitacion WHERE IdHab = '%s'", idHab);
    SQLRETURN ret = SQLExecDirect(hStmt, (SQLWCHAR*)query.wc_str(), SQL_NTS);

    bool exists = false;

    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
        SQLLEN count = 0;
        SQLFetch(hStmt);
        SQLGetData(hStmt, 1, SQL_C_SLONG, &count, 0, NULL);
        exists = (count > 0);
    }

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    return exists;
}

void HabitacionFrame::ShowErrorDialog(const wxString& value, int row, const wxString& columnName) {
    wxMessageBox(wxString::Format("Error en la celda: [%d, %s] con valor: '%s'. Solo se permiten valores numéricos.",
        row + 1, columnName, value), "Error", wxOK | wxICON_ERROR);
}

void HabitacionFrame::OnGridSelectCell(wxGridEvent& event) {
    if (IsActive()) {  // Solo actualizar si la ventana está activa
        int row = event.GetRow();
        int col = event.GetCol();

        // Verificar si la celda seleccionada está en modo lectura
        if (grid->IsReadOnly(row, col)) {
            // Evitar que la celda en modo lectura sea seleccionada
            event.Veto();  // Cancelar el evento de selección
            return;  // Salir de la función para no ejecutar la actualización de etiquetas
        }

        // Si la celda es editable, permitir la selección y actualizar etiquetas
        UpdateRowLabels(row);  // Actualizar etiquetas de las filas
    }

    event.Skip();  // Permitir que otros manejadores sigan funcionando
}

void HabitacionFrame::DesactivarEventosGrid() {
    grid->Unbind(wxEVT_GRID_SELECT_CELL, &HabitacionFrame::OnGridSelectCell, this);
}

void HabitacionFrame::ActivarEventosGrid() {
    grid->Bind(wxEVT_GRID_SELECT_CELL, &HabitacionFrame::OnGridSelectCell, this);
}

void HabitacionFrame::UpdateRowLabels(int selectedRow) {
    for (int row = 0; row < grid->GetNumberRows(); ++row) {
        if (selectedRow == -1 || row != selectedRow) {
            // Si no hay fila seleccionada o no es la fila seleccionada, limpiar etiqueta
            grid->SetRowLabelValue(row, "");  // Vaciar la etiqueta
        }
        else {
            // Si es la fila seleccionada, mostrar ">"
            grid->SetRowLabelValue(row, ">");
        }
    }
    grid->ForceRefresh();  // Asegurarse de que el grid se actualice visualmente
}

wxGrid* HabitacionFrame::GetGrid() {
    return grid; // Retorna el puntero al objeto grid de HabitacionFrame
}

void HabitacionFrame::CerrarConexion() {
    if (conexionAbierta) {
        SQLDisconnect(hDbc);
        SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
        SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
        conexionAbierta = false;
    }
}

void HabitacionFrame::ActivarModoEdicion()
{
    wxCommandEvent dummyEvent; // Crear un evento vacío para pasar a OnEditar
    OnEditar(dummyEvent); // Llamar al método privado/protegido OnEditar
}

bool HabitacionFrame::ConectarBaseDeDatos() {
    SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
    SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);

    SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);
    wxString connectionString = L"Driver={SQL Server};Server=IdeapadGaming;Database=RSCSystem;Trusted_Connection=yes;";
    SQLRETURN retcode = SQLDriverConnect(hDbc, NULL, (SQLWCHAR*)connectionString.wc_str(), SQL_NTS, NULL, 0, NULL, SQL_DRIVER_COMPLETE);

    return (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO);
}

void HabitacionFrame::MostrarErrorODBC(SQLSMALLINT handleType, SQLHANDLE handle) {
    SQLWCHAR sqlState[1024];
    SQLWCHAR message[1024];
    SQLINTEGER nativeError;
    SQLSMALLINT messageLen;

    SQLGetDiagRec(handleType, handle, 1, sqlState, &nativeError, message, 1024, &messageLen);

    wxMessageBox(wxString::Format("Error ODBC\nError SQL: %s\nEstado SQL: %s\nError Nativo: %d",
        wxString(message), wxString(sqlState), nativeError), "Error", wxOK | wxICON_ERROR);
}