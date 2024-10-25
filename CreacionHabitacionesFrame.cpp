#include "CreacionHabitacionesFrame.h"
#include "MainFrame.h"
#include "IDs.h"
#include <wx/grid.h>
#include <sqlext.h>
#include <wx/dir.h>

wxBEGIN_EVENT_TABLE(CreacionHabitacionesFrame, wxFrame)
EVT_BUTTON(ID_BTN_NUEVO, CreacionHabitacionesFrame::OnNuevo)
EVT_BUTTON(ID_BTN_GUARDAR, CreacionHabitacionesFrame::OnGuardar)
EVT_BUTTON(ID_BTN_EDITAR, CreacionHabitacionesFrame::OnEditar)
EVT_BUTTON(ID_BTN_CANCELAR, CreacionHabitacionesFrame::OnCancelar)
EVT_BUTTON(ID_BTN_ELIMINAR, CreacionHabitacionesFrame::OnEliminar)
EVT_BUTTON(ID_BTN_BUSCAR, CreacionHabitacionesFrame::OnBuscar)
EVT_BUTTON(ID_BTN_IZQUIERDA, CreacionHabitacionesFrame::OnIzquierda)
EVT_BUTTON(ID_BTN_DERECHA, CreacionHabitacionesFrame::OnDerecha)
EVT_BUTTON(ID_BTN_EXPORTAR, CreacionHabitacionesFrame::OnExportarExcel) // Añadido para el botón Exportar
EVT_CLOSE(CreacionHabitacionesFrame::OnClose)
EVT_CHAR_HOOK(CreacionHabitacionesFrame::OnKeyDown)
wxEND_EVENT_TABLE()

CreacionHabitacionesFrame::CreacionHabitacionesFrame(const wxString& title, const wxPoint& pos, const wxSize& size, MainFrame* parent, const wxString& usuario)
    : wxFrame(parent, wxID_ANY, title, pos, size, wxDEFAULT_FRAME_STYLE & ~(wxMAXIMIZE_BOX) | wxFRAME_NO_TASKBAR | wxFRAME_FLOAT_ON_PARENT),
    mainFrameParent(parent), usuarioAutenticado(usuario), conexionAbierta(false), isEditing(false), isNewOperation(false), hDbc(NULL)
{
    // Deshabilitar la ventana principal
    if (mainFrameParent != nullptr) {
        mainFrameParent->Disable();
    }

    // Set default font
    defaultFont = wxFont(10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "DejaVu Sans Mono");
    SetFont(defaultFont);

    wxPanel* panel = new wxPanel(this, wxID_ANY);

    // Inicializar los botones
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

    btnIzquierda = new wxButton(panel, ID_BTN_IZQUIERDA, "<", wxPoint(460, 10), wxSize(40, 30));
    btnIzquierda->SetFont(defaultFont);
    btnIzquierda->Enable(false);

    btnDerecha = new wxButton(panel, ID_BTN_DERECHA, ">", wxPoint(510, 10), wxSize(40, 30));
    btnDerecha->SetFont(defaultFont);
    btnDerecha->Enable(false);

    btnEliminar = new wxButton(panel, ID_BTN_ELIMINAR, "Eliminar", wxPoint(610, 10), wxSize(80, 30));
    btnEliminar->SetFont(defaultFont);

    // Inicializar campos de texto
    wxStaticText* lblNroOperacion = new wxStaticText(panel, wxID_ANY, "Nro de Operacion:", wxPoint(10, 50));
    lblNroOperacion->SetFont(defaultFont);

    txtNroOperacion = new wxTextCtrl(panel, wxID_ANY, "", wxPoint(150, 47), wxSize(50, 25));
    txtNroOperacion->SetFont(defaultFont);
    txtNroOperacion->Enable(false);

    wxStaticText* lblIDHab = new wxStaticText(panel, wxID_ANY, "ID de Habitacion:", wxPoint(10, 90));
    lblIDHab->SetFont(defaultFont);

    txtIDHab = new wxTextCtrl(panel, wxID_ANY, "", wxPoint(150, 87), wxSize(50, 25));
    txtIDHab->SetFont(defaultFont);
    txtIDHab->Enable(false);

    wxStaticText* lblDescripcion = new wxStaticText(panel, wxID_ANY, "Descripcion:", wxPoint(10, 130));
    lblDescripcion->SetFont(defaultFont);

    txtDescripcion = new wxTextCtrl(panel, wxID_ANY, "", wxPoint(150, 127), wxSize(350, 25));
    txtDescripcion->SetFont(defaultFont);
    txtDescripcion->Enable(false);

    wxStaticText* lblUM = new wxStaticText(panel, wxID_ANY, "Unidad de Medida:", wxPoint(10, 170));
    lblUM->SetFont(defaultFont);

    txtUM = new wxTextCtrl(panel, wxID_ANY, "", wxPoint(150, 167), wxSize(30, 25));
    txtUM->SetFont(defaultFont);
    txtUM->Enable(false);

    btnExportar = new wxButton(panel, ID_BTN_EXPORTAR, "Exportar Excel", wxPoint(570, 580), wxSize(120, 30));  // Cambiado a la parte inferior derecha
    btnExportar->SetFont(defaultFont);
    btnExportar->Enable(true);

    // Conectar a la base de datos
    ConectarBaseDeDatos();

    if (HayRegistrosEnTabla()) {
        CargarUltimoRegistro();
        int firstID = ObtenerPrimerIDHab();
        int lastID = ObtenerUltimoIDHab();
        int currentID = wxAtoi(txtNroOperacion->GetValue());

        if (currentID == firstID) {
            btnIzquierda->Enable(false);
        }
        else {
            btnIzquierda->Enable(true);
        }

        if (currentID == lastID) {
            btnDerecha->Enable(false);
        }
        else {
            btnDerecha->Enable(true);
        }
    }
    else {
        UpdateButtonStates(false, false);
        btnGuardar->Enable(false);
        btnEditar->Enable(false);
        btnEliminar->Enable(false);
        btnBuscar->Enable(false);
        btnCancelar->Enable(false);
        btnIzquierda->Enable(false);
        btnDerecha->Enable(false);
    }
}

CreacionHabitacionesFrame::~CreacionHabitacionesFrame() {
    CerrarConexion(); // Ensure connection is closed when the object is destroyed
}

void CreacionHabitacionesFrame::OnKeyDown(wxKeyEvent& event) {
    if (event.GetKeyCode() == WXK_ESCAPE) {
        // Verificar si está en modo edición o modo nuevo
        if (isEditing || isNewOperation) {
            int respuesta = wxMessageBox("Hay cambios sin guardar.¿Desea cancelar los cambios? ", "Confirmar cierre", wxYES_NO | wxICON_QUESTION);
            if (respuesta == wxYES) {
                // Simular comportamiento de "Cancelar"
                wxCommandEvent evt;  // Crear un evento wxCommandEvent vacío
                OnCancelar(evt);     // Pasar el evento vacío
            }
            else {
                return;  // No cerrar, el usuario canceló
            }
        }
        else {
            if (mainFrameParent != nullptr) {
                mainFrameParent->Enable();  // Habilitar de nuevo la ventana principal
            }
            Destroy();  // Elimina la ventana sin disparar OnClose
        }
    }
    else {
        event.Skip();  // Permitir que otros eventos de teclado se manejen
    }
}

bool CreacionHabitacionesFrame::HayRegistrosEnTabla() {
    SQLHSTMT hStmt = NULL;
    if (hDbc == NULL) {
        wxMessageBox("Conexión a la base de datos no está inicializada.", "Error", wxOK | wxICON_ERROR);
        return false;
    }

    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
    wxString query = L"SELECT COUNT(*) FROM Habitacion;";
    SQLRETURN ret = SQLExecDirect(hStmt, (SQLWCHAR*)query.wc_str(), SQL_NTS);

    int count = 0;
    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
        SQLFetch(hStmt);
        SQLGetData(hStmt, 1, SQL_C_LONG, &count, 0, NULL);
    }

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    return count > 0;
}

void CreacionHabitacionesFrame::OnNuevo(wxCommandEvent& event) {
    EnableInputs(true);
    ClearInputs();
    isNewOperation = true;
    isEditing = false;

    int siguienteOperacionID = ObtenerSiguienteOperacionID();
    txtNroOperacion->SetValue(wxString::Format("%d", siguienteOperacionID));

    UpdateButtonStates(true, false);
    btnIzquierda->Enable(false);
    btnDerecha->Enable(false);
}

void CreacionHabitacionesFrame::OnGuardar(wxCommandEvent& event) {
    // Validación de campos obligatorios
    if (!ValidateInputs()) {
        wxMessageBox("Todos los campos son obligatorios.", "Error de Validación", wxOK | wxICON_WARNING);
        return;
    }

    // Validación del valor numérico de ID de Habitación
    long idHabValue;
    if (!txtIDHab->GetValue().ToLong(&idHabValue)) {
        wxMessageBox("El campo 'IDHab' solo admite valores numéricos.", "Error de Validación", wxOK | wxICON_WARNING);
        return;
    }

    // Verificar la conexión a la base de datos
    if (!conexionAbierta) {
        wxMessageBox("Error: No hay conexión con la base de datos.", "Error", wxOK | wxICON_ERROR);
        return;
    }

    int operacionID = wxAtoi(txtNroOperacion->GetValue());

    // Verificación exclusiva para modo "Nuevo"
    if (isNewOperation) {
        if (OperacionIDExists(operacionID)) {
            wxMessageBox("El número de operación ya existe. Se generará uno nuevo.", "Número de Operación Duplicado", wxOK | wxICON_INFORMATION);

            // Generar y establecer un nuevo ID para la operación
            int siguienteOperacionID = ObtenerSiguienteOperacionID();
            txtNroOperacion->SetValue(wxString::Format("%d", siguienteOperacionID));
            return;  // No guardar, esperar al siguiente intento con el nuevo ID
        }
    }

    // Variables adicionales necesarias para guardar los datos
    wxString usuarioID = usuarioAutenticado;
    wxString ordenador = wxGetHostName();

    // Preparar la consulta SQL dependiendo del modo (Nuevo o Editar)
    SQLHSTMT hStmt = NULL;
    SQLRETURN ret;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

    wxString query;
    if (isNewOperation) {
        query = L"INSERT INTO Habitacion (OperacionID, IdHab, Descripcion, UM, UsuarioID, Ordenador) VALUES (?, ?, ?, ?, ?, ?);";
    }
    else if (isEditing) {
        query = L"UPDATE Habitacion SET IdHab = ?, Descripcion = ?, UM = ?, UsuarioID = ?, Ordenador = ? WHERE OperacionID = ?;";
    }

    ret = SQLPrepare(hStmt, (SQLWCHAR*)query.wc_str(), SQL_NTS);

    // Valores que se usarán en la inserción o actualización
    int idHab = wxAtoi(txtIDHab->GetValue());
    wxString descripcion = txtDescripcion->GetValue();
    wxString um = txtUM->GetValue();

    // Bindeo de parámetros SQL según el modo (Nuevo o Editar)
    if (isEditing) {
        SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &idHab, 0, NULL);
        SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, descripcion.length(), 0, (SQLPOINTER)descripcion.wc_str(), 0, NULL);
        SQLBindParameter(hStmt, 3, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, um.length(), 0, (SQLPOINTER)um.wc_str(), 0, NULL);
        SQLBindParameter(hStmt, 4, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, usuarioID.length(), 0, (SQLPOINTER)usuarioID.wc_str(), 0, NULL);
        SQLBindParameter(hStmt, 5, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, ordenador.length(), 0, (SQLPOINTER)ordenador.wc_str(), 0, NULL);
        SQLBindParameter(hStmt, 6, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &operacionID, 0, NULL);
    }
    else {
        SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &operacionID, 0, NULL);
        SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &idHab, 0, NULL);
        SQLBindParameter(hStmt, 3, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, descripcion.length(), 0, (SQLPOINTER)descripcion.wc_str(), 0, NULL);
        SQLBindParameter(hStmt, 4, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, um.length(), 0, (SQLPOINTER)um.wc_str(), 0, NULL);
        SQLBindParameter(hStmt, 5, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, usuarioID.length(), 0, (SQLPOINTER)usuarioID.wc_str(), 0, NULL);
        SQLBindParameter(hStmt, 6, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, ordenador.length(), 0, (SQLPOINTER)ordenador.wc_str(), 0, NULL);
    }

    // Ejecutar la consulta SQL
    ret = SQLExecute(hStmt);

    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        SQLWCHAR sqlState[6], messageText[256];
        SQLINTEGER nativeError;
        SQLSMALLINT textLength;
        SQLGetDiagRec(SQL_HANDLE_STMT, hStmt, 1, sqlState, &nativeError, messageText, sizeof(messageText), &textLength);
        wxString errorMsg = wxString::Format("Error al guardar los datos.\nSQL State: %s\nError: %d\n%s",
            wxString(sqlState), nativeError, wxString(messageText));
        wxMessageBox(errorMsg, "Error", wxOK | wxICON_ERROR);

        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return;
    }

    // Liberar el handle de la consulta
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);

    // Actualizar estados y UI después del guardado
    EnableInputs(false);
    UpdateButtonStates(false, false);

    // Determinar si bloquear botones de navegación
    int firstID = ObtenerPrimerIDHab();
    int lastID = ObtenerUltimoIDHab();
    int currentID = wxAtoi(txtNroOperacion->GetValue());

    // Manejar el estado de los botones de navegación después del guardado
    if (isNewOperation || isEditing) {
        // Si es la primera operación, deshabilitar el botón izquierda
        if (currentID == firstID) {
            btnIzquierda->Enable(false);
        }
        else {
            btnIzquierda->Enable(true);
        }

        // Si es la última operación, deshabilitar el botón derecha
        if (currentID == lastID) {
            btnDerecha->Enable(false);
        }
        else {
            btnDerecha->Enable(true);
        }
    }

    // Finalizar el modo de edición o creación
    isEditing = false;
    isNewOperation = false;
}

bool CreacionHabitacionesFrame::OperacionIDExists(int operacionID) {
    SQLHSTMT hStmt = NULL;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

    wxString query = wxString::Format(L"SELECT COUNT(*) FROM Habitacion WHERE OperacionID = %d;", operacionID);
    SQLRETURN ret = SQLExecDirect(hStmt, (SQLWCHAR*)query.wc_str(), SQL_NTS);

    int count = 0;
    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
        SQLFetch(hStmt);
        SQLGetData(hStmt, 1, SQL_C_LONG, &count, 0, NULL);
    }

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    return count > 0;  // Si el count es mayor a 0, el OperacionID ya existe
}

bool CreacionHabitacionesFrame::IDHabExists(int idHab) {
    SQLHSTMT hStmt = NULL;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
    wxString query = wxString::Format(L"SELECT COUNT(*) FROM Habitacion WHERE IdHab = %d;", idHab);
    SQLRETURN ret = SQLExecDirect(hStmt, (SQLWCHAR*)query.wc_str(), SQL_NTS);

    int count = 0;
    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
        SQLFetch(hStmt);
        SQLGetData(hStmt, 1, SQL_C_LONG, &count, 0, NULL);
    }

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    return count > 0;
}

void CreacionHabitacionesFrame::OnEditar(wxCommandEvent& event) {
    originalNroOperacion = wxAtoi(txtNroOperacion->GetValue());
    originalIDHab = txtIDHab->GetValue();
    originalDescripcion = txtDescripcion->GetValue();
    originalUM = txtUM->GetValue();

    EnableInputs(true);
    UpdateButtonStates(false, true);
    btnIzquierda->Enable(false);
    btnDerecha->Enable(false);
    isEditing = true;
}

void CreacionHabitacionesFrame::OnBuscar(wxCommandEvent& event) {
    BuscarCreacionFrame* buscarFrame = new BuscarCreacionFrame(this, "Buscar Habitaciones", wxDefaultPosition, wxSize(750, 600));
    buscarFrame->CentreOnParent();
    buscarFrame->Show(true);
    Disable();
}

void CreacionHabitacionesFrame::OnCancelar(wxCommandEvent& event) {
    // Deshabilitar temporalmente el manejador de eventos para evitar ciclos.
    this->Unbind(wxEVT_CLOSE_WINDOW, &CreacionHabitacionesFrame::OnClose, this);

    if (isNewOperation) {
        int operacionID = wxAtoi(txtNroOperacion->GetValue());
        if (operacionID == 0 || !HayRegistrosEnTabla()) {
            ClearInputs();
            EnableInputs(false);
            UpdateButtonStates(false, false);
            btnEditar->Enable(false);
            btnIzquierda->Enable(false);
            btnDerecha->Enable(false);
            btnBuscar->Enable(false);
            btnEliminar->Enable(false);
            isNewOperation = false;
        }
        else {
            CargarUltimoRegistro();
            isNewOperation = false;
        }
    }
    else if (isEditing) {
        // Revertir a los valores originales en caso de edición
        txtNroOperacion->SetValue(wxString::Format("%d", originalNroOperacion));
        txtIDHab->SetValue(originalIDHab);
        txtDescripcion->SetValue(originalDescripcion);
        txtUM->SetValue(originalUM);
        isEditing = false;
    }

    if (!HayRegistrosEnTabla()) {
        ClearInputs();
        EnableInputs(false);
        UpdateButtonStates(false, false);
        btnNuevo->Enable(true);
        btnGuardar->Enable(false);
        btnEditar->Enable(false);
        btnEliminar->Enable(false);
        btnBuscar->Enable(false);
        btnCancelar->Enable(false);
        btnIzquierda->Enable(false);
        btnDerecha->Enable(false);
        return;
    }

    // Actualizar los botones de navegación según la operación actual
    int currentID = wxAtoi(txtNroOperacion->GetValue());
    int firstID = ObtenerPrimerIDHab();
    int lastID = ObtenerUltimoIDHab();

    if (currentID == firstID) {
        btnIzquierda->Enable(false);
    }
    else {
        btnIzquierda->Enable(true);
    }

    if (currentID == lastID) {
        btnDerecha->Enable(false);
    }
    else {
        btnDerecha->Enable(true);
    }

    EnableInputs(false);
    UpdateButtonStates(false, false);

    // Reactivar el manejador de eventos después de cancelar.
    this->Bind(wxEVT_CLOSE_WINDOW, &CreacionHabitacionesFrame::OnClose, this);
}

int CreacionHabitacionesFrame::ObtenerSiguienteOperacionID() {
    SQLHSTMT hStmt = NULL;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
    wxString query = L"SELECT ISNULL(MAX(OperacionID), 0) + 1 FROM Habitacion;";
    SQLRETURN ret = SQLExecDirect(hStmt, (SQLWCHAR*)query.wc_str(), SQL_NTS);

    int siguienteOperacionID = 1;

    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
        SQLFetch(hStmt);
        SQLGetData(hStmt, 1, SQL_C_LONG, &siguienteOperacionID, 0, NULL);
    }

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    return siguienteOperacionID;
}

void CreacionHabitacionesFrame::CargarUltimoRegistro() {
    int ultimoID = ObtenerUltimoIDHab();
    if (ultimoID != -1) {
        CargarDatosIDHab(ultimoID);

        if (ObtenerPrimerIDHab() == ultimoID && ObtenerUltimoIDHab() == ultimoID) {
            btnIzquierda->Enable(false);
            btnDerecha->Enable(false);
        }
        else {
            btnDerecha->Enable(false);
            btnIzquierda->Enable(true);
        }
    }
}

int CreacionHabitacionesFrame::ObtenerPrimerIDHab() {
    SQLHSTMT hStmt = NULL;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
    wxString query = L"SELECT MIN(OperacionID) FROM Habitacion;";
    SQLRETURN ret = SQLExecDirect(hStmt, (SQLWCHAR*)query.wc_str(), SQL_NTS);

    int firstID = -1;

    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
        SQLFetch(hStmt);
        SQLGetData(hStmt, 1, SQL_C_LONG, &firstID, 0, NULL);
    }

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);

    return firstID;
}

int CreacionHabitacionesFrame::ObtenerUltimoIDHab() {
    SQLHSTMT hStmt = NULL;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
    wxString query = L"SELECT MAX(OperacionID) FROM Habitacion;";
    SQLRETURN ret = SQLExecDirect(hStmt, (SQLWCHAR*)query.wc_str(), SQL_NTS);

    int lastID = -1;

    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
        SQLFetch(hStmt);
        SQLGetData(hStmt, 1, SQL_C_LONG, &lastID, 0, NULL);
    }

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);

    return lastID;
}

void CreacionHabitacionesFrame::OnIzquierda(wxCommandEvent& event) {
    int currentID = wxAtoi(txtNroOperacion->GetValue());
    int previousID = ObtenerIDHabAnterior(currentID);

    if (previousID != -1) {
        CargarDatosIDHab(previousID);
    }
    else {
        btnIzquierda->Enable(false);
    }
}

void CreacionHabitacionesFrame::OnDerecha(wxCommandEvent& event) {
    int currentID = wxAtoi(txtNroOperacion->GetValue());
    int nextID = ObtenerIDHabSiguiente(currentID);

    if (nextID != -1) {
        CargarDatosIDHab(nextID);
    }
    else {
        btnDerecha->Enable(false);
    }
}

int CreacionHabitacionesFrame::ObtenerIDHabAnterior(int currentID) {
    SQLHSTMT hStmt = NULL;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
    wxString query = wxString::Format(L"SELECT MAX(OperacionID) FROM Habitacion WHERE OperacionID < %d;", currentID);
    SQLRETURN ret = SQLExecDirect(hStmt, (SQLWCHAR*)query.wc_str(), SQL_NTS);

    int previousID = -1;

    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
        SQLFetch(hStmt);
        SQLGetData(hStmt, 1, SQL_C_LONG, &previousID, 0, NULL);
    }

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    return previousID;
}

int CreacionHabitacionesFrame::ObtenerIDHabSiguiente(int currentID) {
    SQLHSTMT hStmt = NULL;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
    wxString query = wxString::Format(L"SELECT MIN(OperacionID) FROM Habitacion WHERE OperacionID > %d;", currentID);
    SQLRETURN ret = SQLExecDirect(hStmt, (SQLWCHAR*)query.wc_str(), SQL_NTS);

    int nextID = -1;

    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
        SQLFetch(hStmt);
        SQLGetData(hStmt, 1, SQL_C_LONG, &nextID, 0, NULL);
    }

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    return nextID;
}

void CreacionHabitacionesFrame::SetIDHab(const wxString& idHab) {
    txtIDHab->SetValue(idHab);
    CargarDatosIDHab(wxAtoi(idHab));
}

void CreacionHabitacionesFrame::OnEliminar(wxCommandEvent& event) {

    if (!conexionAbierta && !ConectarBaseDeDatos()) {
        wxMessageBox("Error al conectar a la base de datos.", "Error", wxOK | wxICON_ERROR);
        return;
    }

    int nroOperacion = wxAtoi(txtNroOperacion->GetValue());
    if (nroOperacion == 0) {
        wxMessageBox("No hay un Nro Operacion válido para eliminar.", "Error", wxOK | wxICON_ERROR);
        return;
    }

    int respuesta = wxMessageBox("¿Está seguro de que desea eliminar este registro?", "Confirmar eliminación", wxYES_NO | wxICON_QUESTION);
    if (respuesta == wxNO) {
        return;
    }

    // Deshabilitar la ventana durante la operación de eliminación
    this->Disable();

    SQLHSTMT hStmt = NULL;
    SQLRETURN ret;
    bool error = false;

    try {
        SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
        wxString query = L"DELETE FROM Habitacion WHERE OperacionID = ?;";
        ret = SQLPrepare(hStmt, (SQLWCHAR*)query.wc_str(), SQL_NTS);

        if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
            wxMessageBox("Error al preparar la consulta para eliminar.", "Error", wxOK | wxICON_ERROR);
            error = true;
            throw std::runtime_error("Error preparing SQL statement for deletion.");
        }

        SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &nroOperacion, 0, NULL);
        ret = SQLExecute(hStmt);

        if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
            wxMessageBox("Error al eliminar el registro.", "Error", wxOK | wxICON_ERROR);
            error = true;
            throw std::runtime_error("Error executing SQL statement for deletion.");
        }

        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);

        // Verificar si la tabla quedó vacía tras la eliminación
        if (!HayRegistrosEnTabla()) {
            ClearInputs();
            EnableInputs(false);
            UpdateButtonStates(false, false);
            btnNuevo->Enable(true);
            btnGuardar->Enable(false);
            btnEditar->Enable(false);
            btnEliminar->Enable(false);
            btnBuscar->Enable(false);
            btnCancelar->Enable(false);
            btnIzquierda->Enable(false);
            btnDerecha->Enable(false);
            this->Enable();  // Rehabilitar la ventana
            return;
        }

        // Cargar el siguiente registro o el anterior si no hay más
        int nextID = ObtenerIDHabSiguiente(nroOperacion);
        if (nextID != -1) {
            CargarDatosIDHab(nextID);
        }
        else {
            int previousID = ObtenerIDHabAnterior(nroOperacion);
            if (previousID != -1) {
                CargarDatosIDHab(previousID);
            }
            else {
                ClearInputs();
            }
        }

    }
    catch (const std::exception& ex) {
        if (hStmt != NULL) {
            SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        }
        wxMessageBox(wxString::Format("Se produjo un error durante la eliminación: %s", ex.what()), "Error", wxOK | wxICON_ERROR);
        error = true;
    }

    // Rehabilitar la ventana independientemente de si hubo un error o no
    this->Enable();
}

void CreacionHabitacionesFrame::OnClose(wxCloseEvent& event) {
    if (isEditing || isNewOperation) {
        int respuesta = wxMessageBox("Hay cambios no guardados. ¿Seguro que desea cerrar?", "Confirmar cierre", wxYES_NO | wxICON_QUESTION);
        if (respuesta == wxYES) {
            if (mainFrameParent != nullptr) {
                mainFrameParent->Enable();  // Habilitar de nuevo la ventana principal
            }
            event.Skip();  // Permitir el cierre
        }
        else {
            event.Veto();  // Cancelar el cierre si el usuario rechaza
        }
    }
    else {
        if (mainFrameParent != nullptr) {
            mainFrameParent->Enable();  // Habilitar de nuevo la ventana principal
        }
        event.Skip();  // Permitir el cierre directamente si no hay edición o nuevo registro
    }
}

void CreacionHabitacionesFrame::EnableInputs(bool enable) {
    txtIDHab->Enable(enable);
    txtDescripcion->Enable(enable);
    txtUM->Enable(enable);
}

void CreacionHabitacionesFrame::ClearInputs() {
    txtNroOperacion->Clear();
    txtIDHab->Clear();
    txtDescripcion->Clear();
    txtUM->Clear();
}

void CreacionHabitacionesFrame::UpdateButtonStates(bool nuevoSelected, bool editarSelected) {
    btnNuevo->Enable(!nuevoSelected && !editarSelected);
    btnBuscar->Enable(!nuevoSelected && !editarSelected);
    btnEliminar->Enable(!nuevoSelected && !editarSelected);
    btnGuardar->Enable(nuevoSelected || editarSelected);
    btnCancelar->Enable(nuevoSelected || editarSelected);
    btnEditar->Enable(!nuevoSelected && !editarSelected);
    btnExportar->Enable(!nuevoSelected && !editarSelected);
}

void CreacionHabitacionesFrame::OnExportarExcel(wxCommandEvent& event) {
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
        wxString exportPath = folderPath + "\\habitacion.xlsx";

        // Cargar el archivo de plantilla de Excel
        xlnt::workbook wb;
        wb.load("D:\\RSC Sistema\\exportar_habitacion.xlsx");

        // Acceder a la primera hoja
        xlnt::worksheet ws = wb.active_sheet();

        // Conectar a la base de datos y obtener los datos de la tabla Habitacion
        SQLHSTMT hStmt = NULL;
        SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

        wxString query = L"SELECT IdHab, Descripcion, UM FROM Habitacion;";
        SQLRETURN ret = SQLExecDirect(hStmt, (SQLWCHAR*)query.wc_str(), SQL_NTS);

        if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
            int row = 2;  // Empezar desde la segunda fila (asumiendo la primera fila es el encabezado)
            while (SQLFetch(hStmt) == SQL_SUCCESS) {
                int idHab;
                SQLWCHAR descripcion[255], um[50];

                SQLGetData(hStmt, 1, SQL_C_LONG, &idHab, 0, NULL);
                SQLGetData(hStmt, 2, SQL_C_WCHAR, descripcion, sizeof(descripcion), NULL);
                SQLGetData(hStmt, 3, SQL_C_WCHAR, um, sizeof(um), NULL);

                // Escribir los datos en las celdas de Excel
                ws.cell("A" + std::to_string(row)).value(idHab);  // IdHab en la columna A
                ws.cell("B" + std::to_string(row)).value(wxString(descripcion).ToStdString());  // Descripcion en la columna B
                ws.cell("C" + std::to_string(row)).value(wxString(um).ToStdString());  // UM en la columna C

                ++row;
            }
        }

        // Liberar recursos del statement
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);

        // Guardar el archivo Excel en la ruta generada
        wb.save(exportPath.ToStdString());

        // Abrir automáticamente el archivo exportado
        ShellExecute(NULL, L"open", exportPath.wc_str(), NULL, NULL, SW_SHOW);
    }
    catch (const std::exception& ex) {
        wxMessageBox(wxString::Format("Error al exportar Excel: %s", ex.what()), "Error", wxOK | wxICON_ERROR);
    }
}

bool CreacionHabitacionesFrame::ConectarBaseDeDatos() {
    SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
    SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);

    SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);
    wxString connectionString = L"Driver={SQL Server};Server=IdeapadGaming;Database=RSCSystem;Trusted_Connection=yes;";
    SQLRETURN retcode = SQLDriverConnect(hDbc, NULL, (SQLWCHAR*)connectionString.wc_str(), SQL_NTS, NULL, 0, NULL, SQL_DRIVER_COMPLETE);

    conexionAbierta = (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO);
    return conexionAbierta;
}

void CreacionHabitacionesFrame::CerrarConexion() {
    if (conexionAbierta) {
        SQLDisconnect(hDbc);
        SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
        SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
        conexionAbierta = false;
    }
}

void CreacionHabitacionesFrame::CargarDatosIDHab(int operacionID) {
    SQLHSTMT hStmt = NULL;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
    wxString query = wxString::Format(L"SELECT OperacionID, IdHab, Descripcion, UM, UsuarioID, Ordenador FROM Habitacion WHERE OperacionID = %d;", operacionID);
    SQLRETURN ret = SQLExecDirect(hStmt, (SQLWCHAR*)query.wc_str(), SQL_NTS);

    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
        if (SQLFetch(hStmt) == SQL_SUCCESS) {
            int fetchedOperacionID, fetchedIdHab;
            SQLWCHAR descripcion[255], um[50], usuarioID[50], ordenador[250];

            SQLGetData(hStmt, 1, SQL_C_LONG, &fetchedOperacionID, 0, NULL);
            SQLGetData(hStmt, 2, SQL_C_LONG, &fetchedIdHab, 0, NULL);
            SQLGetData(hStmt, 3, SQL_C_WCHAR, descripcion, sizeof(descripcion), NULL);
            SQLGetData(hStmt, 4, SQL_C_WCHAR, um, sizeof(um), NULL);
            SQLGetData(hStmt, 5, SQL_C_WCHAR, usuarioID, sizeof(usuarioID), NULL);
            SQLGetData(hStmt, 6, SQL_C_WCHAR, ordenador, sizeof(ordenador), NULL);

            txtNroOperacion->SetValue(wxString::Format("%d", fetchedOperacionID));
            txtIDHab->SetValue(wxString::Format("%d", fetchedIdHab));
            txtDescripcion->SetValue(wxString(descripcion));
            txtUM->SetValue(wxString(um));

            int firstID = ObtenerPrimerIDHab();
            int lastID = ObtenerUltimoIDHab();

            if (fetchedOperacionID == firstID) {
                btnIzquierda->Enable(false);
            }
            else {
                btnIzquierda->Enable(true);
            }

            if (fetchedOperacionID == lastID) {
                btnDerecha->Enable(false);
            }
            else {
                btnDerecha->Enable(true);
            }
        }
    }
    else {
        wxMessageBox("Error al cargar los datos de la OperacionID.", "Error", wxOK | wxICON_ERROR);
    }

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
}

bool CreacionHabitacionesFrame::ValidateInputs() {
    return !txtIDHab->GetValue().IsEmpty() && !txtDescripcion->GetValue().IsEmpty() && !txtUM->GetValue().IsEmpty();
}
