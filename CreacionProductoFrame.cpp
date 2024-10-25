#include "CreacionProductoFrame.h"
#include "MainFrame.h"
#include "IDs.h"
#include <wx/grid.h>
#include <sqlext.h>
#include <algorithm>
#include <locale>
#include <unordered_map>
#include <wx/dir.h>

wxBEGIN_EVENT_TABLE(CreacionProductoFrame, wxFrame)
EVT_BUTTON(ID_BTN_NUEVO, CreacionProductoFrame::OnNuevo)
EVT_BUTTON(ID_BTN_GUARDAR, CreacionProductoFrame::OnGuardar)
EVT_BUTTON(ID_BTN_EDITAR, CreacionProductoFrame::OnEditar)
EVT_BUTTON(ID_BTN_CANCELAR, CreacionProductoFrame::OnCancelar)
EVT_BUTTON(ID_BTN_ELIMINAR, CreacionProductoFrame::OnEliminar)
EVT_BUTTON(ID_BTN_BUSCAR, CreacionProductoFrame::OnBuscar)
EVT_BUTTON(ID_BTN_IZQUIERDA, CreacionProductoFrame::OnIzquierda)
EVT_BUTTON(ID_BTN_DERECHA, CreacionProductoFrame::OnDerecha)
EVT_BUTTON(ID_BTN_EXPORTAR, CreacionProductoFrame::OnExportarExcel) // Añadido para Exportar Excel
EVT_TEXT(wxID_ANY, CreacionProductoFrame::OnDescripcionChange)
EVT_CHAR_HOOK(CreacionProductoFrame::OnKeyDown)  // Capturar tecla Esc
EVT_CLOSE(CreacionProductoFrame::OnClose)
wxEND_EVENT_TABLE()

CreacionProductoFrame::CreacionProductoFrame(const wxString& title, const wxPoint& pos, const wxSize& size, MainFrame* parent, const wxString& usuario)
    : wxFrame(parent, wxID_ANY, title, pos, size, wxDEFAULT_FRAME_STYLE & ~(wxMAXIMIZE_BOX) | wxFRAME_NO_TASKBAR | wxFRAME_FLOAT_ON_PARENT),  // Bloqueo del botón de maximizar
    mainFrameParent(parent), usuarioAutenticado(usuario), conexionAbierta(false), isEditing(false), isNewOperation(false), hDbc(NULL)
{
    // Set default font
    defaultFont = wxFont(10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "DejaVu Sans Mono");
    SetFont(defaultFont);

    wxPanel* panel = new wxPanel(this, wxID_ANY);

    // Initialize buttons
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

    // Botón de Exportar Excel añadido en la parte inferior derecha
    btnExportar = new wxButton(panel, ID_BTN_EXPORTAR, "Exportar Excel", wxPoint(570, 580), wxSize(120, 30));  // Añadido para Exportar Excel
    btnExportar->SetFont(defaultFont);
    btnExportar->Enable(true);  // Habilitado por defecto

    // Initialize text fields
    wxStaticText* lblOperacion = new wxStaticText(panel, wxID_ANY, "Operacion:", wxPoint(10, 50));
    lblOperacion->SetFont(defaultFont);

    txtOperacionID = new wxTextCtrl(panel, wxID_ANY, "", wxPoint(140, 47), wxSize(50, 23));
    txtOperacionID->SetFont(defaultFont);
    txtOperacionID->Enable(false);

    wxStaticText* lblDescripcion = new wxStaticText(panel, wxID_ANY, "Descripcion:", wxPoint(10, 90));
    lblDescripcion->SetFont(defaultFont);

    txtDescripcion = new wxTextCtrl(panel, wxID_ANY, "", wxPoint(140, 87), wxSize(200, 23));
    txtDescripcion->SetFont(defaultFont);
    txtDescripcion->Enable(false);

    lblSimilitud = new wxStaticText(panel, wxID_ANY, "", wxPoint(355, 91), wxSize(200, 25));
    lblSimilitud->SetFont(defaultFont);

    // Conectar a la base de datos
    ConectarBaseDeDatos();

    // Cargar el último registro si existen registros
    if (HayRegistrosEnTabla()) {
        CargarUltimoRegistro();
        int firstID = ObtenerPrimerOperacion();
        int lastID = ObtenerUltimaOperacion();
        int currentID = wxAtoi(txtOperacionID->GetValue()) - 100;

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

void CreacionProductoFrame::OnExportarExcel(wxCommandEvent& event) {
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
        wxString exportPath = folderPath + "\\producto.xlsx";  // Cambiado el nombre a producto.xlsx

        // Cargar el archivo de plantilla de Excel
        xlnt::workbook wb;
        wb.load("D:\\RSC Sistema\\exportar_atributos.xlsx");  // Cargar la plantilla exportar_producto.xlsx

        // Acceder a la primera hoja
        xlnt::worksheet ws = wb.active_sheet();

        // Consultar la base de datos para obtener los datos de la tabla Producto
        SQLHSTMT hStmt = NULL;
        SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
        wxString query = L"SELECT OperacionID, Descripcion FROM Producto;";
        SQLRETURN ret = SQLExecDirect(hStmt, (SQLWCHAR*)query.wc_str(), SQL_NTS);

        if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
            wxMessageBox("Error al ejecutar la consulta SQL para exportar los datos.", "Error", wxOK | wxICON_ERROR);
            SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
            return;
        }

        // Variables para almacenar los datos extraídos de la base de datos
        SQLWCHAR operacionID[4];
        SQLWCHAR descripcion[255];
        int row = 2; // Empezar desde la fila 2 en el Excel

        // Extraer los datos fila por fila y escribirlos en el Excel
        while (SQLFetch(hStmt) == SQL_SUCCESS) {
            SQLGetData(hStmt, 1, SQL_C_WCHAR, operacionID, sizeof(operacionID), NULL);
            SQLGetData(hStmt, 2, SQL_C_WCHAR, descripcion, sizeof(descripcion), NULL);

            // Escribir los datos en las celdas
            ws.cell("A" + std::to_string(row)).value(wxString(operacionID).ToStdString());  // OperacionID
            ws.cell("B" + std::to_string(row)).value(wxString(descripcion).ToStdString());  // Descripcion
            row++;
        }

        // Guardar el archivo Excel en la ruta generada
        wb.save(exportPath.ToStdString());

        // Abrir automáticamente el archivo exportado
        ShellExecute(NULL, L"open", exportPath.wc_str(), NULL, NULL, SW_SHOW);

        // Liberar el manejador de la consulta
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    }
    catch (const std::exception& ex) {
        wxMessageBox(wxString::Format("Error al exportar Excel: %s", ex.what()), "Error", wxOK | wxICON_ERROR);
    }
}

CreacionProductoFrame::~CreacionProductoFrame() {
    CerrarConexion();
}

void CreacionProductoFrame::OnDescripcionChange(wxCommandEvent& event) {
    if (!txtDescripcion->IsEnabled()) {
        lblSimilitud->SetLabel("");
        return;
    }

    long posicionCursor = txtDescripcion->GetInsertionPoint();

    wxString descripcion = txtDescripcion->GetValue();
    descripcion.MakeUpper();

    txtDescripcion->ChangeValue(descripcion);

    txtDescripcion->SetInsertionPoint(posicionCursor);

    if (!descripcion.IsEmpty()) {
        wxString descripcionSimilar = BuscarDescripcionSimilar(descripcion);

        if (!descripcionSimilar.IsEmpty() && descripcion != descripcionSimilar) {
            lblSimilitud->SetLabel(wxString::Format("Parecido a %s", descripcionSimilar));
        }
        else {
            lblSimilitud->SetLabel("");
        }
    }
    else {
        lblSimilitud->SetLabel("");
    }
}

wxString CreacionProductoFrame::BuscarDescripcionSimilar(const wxString& descripcion) {
    const int UMBRAL_SIMILITUD = 3;

    SQLHSTMT hStmt = NULL;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
    wxString query = L"SELECT Descripcion FROM Producto;";
    SQLRETURN ret = SQLExecDirect(hStmt, (SQLWCHAR*)query.wc_str(), SQL_NTS);

    wxString descripcionNormalizada = NormalizarCadena(descripcion);
    wxString descripcionMasParecida;
    int distanciaMinima = std::numeric_limits<int>::max();

    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
        while (SQLFetch(hStmt) == SQL_SUCCESS) {
            SQLWCHAR descripcionExistente[255];
            SQLGetData(hStmt, 1, SQL_C_WCHAR, descripcionExistente, sizeof(descripcionExistente), NULL);
            wxString descripcionExistenteStr(descripcionExistente);
            wxString descripcionExistenteNormalizada = NormalizarCadena(descripcionExistenteStr);

            int distancia = CalcularDistanciaLevenshtein(descripcionNormalizada, descripcionExistenteNormalizada);

            int longitudMaxima = std::max(descripcionNormalizada.Length(), descripcionExistenteNormalizada.Length());
            if (longitudMaxima > 0) {
                double similitudRelativa = static_cast<double>(distancia) / longitudMaxima;
                if (similitudRelativa <= 0.35) {
                    if (distancia < distanciaMinima) {
                        distanciaMinima = distancia;
                        descripcionMasParecida = descripcionExistenteStr;
                    }
                }
            }
            else if (distancia <= UMBRAL_SIMILITUD) {
                if (distancia < distanciaMinima) {
                    distanciaMinima = distancia;
                    descripcionMasParecida = descripcionExistenteStr;
                }
            }
        }
    }

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    return descripcionMasParecida;
}

wxString CreacionProductoFrame::NormalizarCadena(const wxString& cadena) {
    wxString resultado = cadena.Lower();

    std::unordered_map<char, char> reemplazos = {
        {'á', 'a'}, {'é', 'e'}, {'í', 'i'}, {'ó', 'o'}, {'ú', 'u'},
        {'Á', 'a'}, {'É', 'e'}, {'Í', 'i'}, {'Ó', 'o'}, {'Ú', 'u'},
        {'ü', 'u'}, {'Ü', 'u'}
    };
    resultado.Replace("h", "");

    wxString cadenaNormalizada;
    for (auto ch : resultado) {
        if (reemplazos.find(ch) != reemplazos.end()) {
            cadenaNormalizada += reemplazos[ch];
        }
        else {
            cadenaNormalizada += ch;
        }
    }
    return cadenaNormalizada;
}

bool CreacionProductoFrame::EsNombreRepetitivo(const wxString& descripcion) {
    if (descripcion.Length() <= 1) {
        return true;
    }

    char primerCaracter = descripcion[0];
    for (size_t i = 1; i < descripcion.Length(); ++i) {
        if (descripcion[i] != primerCaracter) {
            return false;
        }
    }
    return true;
}

int CreacionProductoFrame::CalcularDistanciaLevenshtein(const wxString& source, const wxString& target) {
    size_t len1 = source.length(), len2 = target.length();
    std::vector<std::vector<int>> dp(len1 + 1, std::vector<int>(len2 + 1));

    for (size_t i = 0; i <= len1; ++i) dp[i][0] = i;
    for (size_t j = 0; j <= len2; ++j) dp[0][j] = j;

    for (size_t i = 1; i <= len1; ++i) {
        for (size_t j = 1; j <= len2; ++j) {
            int cost = (source[i - 1] == target[j - 1]) ? 0 : 1;
            dp[i][j] = std::min({ dp[i - 1][j] + 1, dp[i][j - 1] + 1, dp[i - 1][j - 1] + cost });
        }
    }

    return dp[len1][len2];
}

bool CreacionProductoFrame::HayRegistrosEnTabla() {
    SQLHSTMT hStmt = NULL;
    if (hDbc == NULL) {
        wxMessageBox("Conexión a la base de datos no está inicializada.", "Error", wxOK | wxICON_ERROR);
        return false;
    }

    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
    wxString query = L"SELECT COUNT(*) FROM Producto;";
    SQLRETURN ret = SQLExecDirect(hStmt, (SQLWCHAR*)query.wc_str(), SQL_NTS);

    int count = 0;
    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
        SQLFetch(hStmt);
        SQLGetData(hStmt, 1, SQL_C_LONG, &count, 0, NULL);
    }

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    return count > 0;
}

void CreacionProductoFrame::OnNuevo(wxCommandEvent& event) {
    EnableInputs(true);
    ClearInputs();
    lblSimilitud->SetLabel("");
    isNewOperation = true;
    isEditing = false;

    int siguienteOperacion = ObtenerSiguienteOperacion();
    txtOperacionID->SetValue(wxString::Format("%d", siguienteOperacion + 100));

    UpdateButtonStates(true, false);
    btnIzquierda->Enable(false);
    btnDerecha->Enable(false);
}

bool CreacionProductoFrame::OperacionExists(int operacion) {
    SQLHSTMT hStmt = NULL;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
    wxString query = wxString::Format(L"SELECT COUNT(*) FROM Producto WHERE Operacion = %d;", operacion);
    SQLRETURN ret = SQLExecDirect(hStmt, (SQLWCHAR*)query.wc_str(), SQL_NTS);

    int count = 0;
    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
        SQLFetch(hStmt);
        SQLGetData(hStmt, 1, SQL_C_LONG, &count, 0, NULL);
    }

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    return count > 0;
}

void CreacionProductoFrame::OnGuardar(wxCommandEvent& event) {
    lblSimilitud->SetLabel("");

    if (!ValidateInputs()) {
        wxMessageBox("Todos los campos son obligatorios.", "Error de Validación", wxOK | wxICON_WARNING);
        return;
    }

    wxString descripcion = txtDescripcion->GetValue();

    if (EsNombreRepetitivo(descripcion)) {
        wxMessageBox("El nombre ingresado no puede consistir únicamente de caracteres repetidos.", "Error de Validación", wxOK | wxICON_ERROR);
        return;
    }

    long operacionIDValue;
    if (!txtOperacionID->GetValue().ToLong(&operacionIDValue)) {
        wxMessageBox("El campo 'Operacion ID' solo admite valores numéricos.", "Error de Validación", wxOK | wxICON_ERROR);
        return;
    }

    int operacionValue = operacionIDValue - 100;
    if (!isEditing && OperacionExists(operacionValue)) {
        wxMessageBox("El número de operación ya existe. Se generará uno nuevo.", "Número de Operación Duplicado", wxOK | wxICON_INFORMATION);
        operacionValue = ObtenerSiguienteOperacion();
        txtOperacionID->SetValue(wxString::Format("%d", operacionValue + 100));
        return;
    }

    if (!conexionAbierta) {
        wxMessageBox("Error: No hay conexión con la base de datos.", "Error", wxOK | wxICON_ERROR);
        return;
    }

    wxString usuarioID = usuarioAutenticado;
    wxString ordenador = wxGetHostName();

    SQLHSTMT hStmt = NULL;
    SQLRETURN ret;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

    wxString query;
    if (isEditing) {
        query = L"UPDATE Producto SET Descripcion = ?, UsuarioID = ?, Ordenador = ? WHERE Operacion = ?;";
    }
    else {
        query = L"INSERT INTO Producto (Operacion, OperacionID, Descripcion, UsuarioID, Ordenador) VALUES (?, ?, ?, ?, ?);";
    }

    ret = SQLPrepare(hStmt, (SQLWCHAR*)query.wc_str(), SQL_NTS);

    int operacion = operacionValue;

    if (isEditing) {
        SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, descripcion.length(), 0, (SQLPOINTER)descripcion.wc_str(), 0, NULL);
        SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, usuarioID.length(), 0, (SQLPOINTER)usuarioID.wc_str(), 0, NULL);
        SQLBindParameter(hStmt, 3, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, ordenador.length(), 0, (SQLPOINTER)ordenador.wc_str(), 0, NULL);
        SQLBindParameter(hStmt, 4, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &operacion, 0, NULL);
    }
    else {
        SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &operacion, 0, NULL);
        SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &operacionIDValue, 0, NULL);
        SQLBindParameter(hStmt, 3, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, descripcion.length(), 0, (SQLPOINTER)descripcion.wc_str(), 0, NULL);
        SQLBindParameter(hStmt, 4, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, usuarioID.length(), 0, (SQLPOINTER)usuarioID.wc_str(), 0, NULL);
        SQLBindParameter(hStmt, 5, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, ordenador.length(), 0, (SQLPOINTER)ordenador.wc_str(), 0, NULL);
    }

    ret = SQLExecute(hStmt);

    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        SQLWCHAR sqlState[6], messageText[256];
        SQLINTEGER nativeError;
        SQLSMALLINT textLength;
        SQLGetDiagRec(SQL_HANDLE_STMT, hStmt, 1, sqlState, &nativeError, messageText, sizeof(messageText), &textLength);

        if (wxString(sqlState) == "23000") {
            wxMessageBox("El nombre ingresado ya existe en la base de datos.", "Error de Validación", wxOK | wxICON_WARNING);
        }
        else {
            wxString errorMsg = wxString::Format("Error al guardar los datos.\nSQL State: %s\nError: %d\n%s",
                wxString(sqlState), nativeError, wxString(messageText));
            wxMessageBox(errorMsg, "Error", wxOK | wxICON_ERROR);
        }

        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return;
    }

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);

    EnableInputs(false);
    UpdateButtonStates(false, false);

    // Verificar si es la primera o última operación y ajustar botones
    int firstID = ObtenerPrimerOperacion();
    int lastID = ObtenerUltimaOperacion();

    if (operacionValue == firstID) {
        btnIzquierda->Enable(false);  // Deshabilitar botón izquierda
    }
    else {
        btnIzquierda->Enable(true);
    }

    if (operacionValue == lastID) {
        btnDerecha->Enable(false);  // Deshabilitar botón derecha
    }
    else {
        btnDerecha->Enable(true);
    }

    // *** Aquí actualizamos las banderas, ya que los cambios fueron guardados ***
    isEditing = false;
    isNewOperation = false;
}

void CreacionProductoFrame::OnEditar(wxCommandEvent& event) {
    originalOperacionID = wxAtoi(txtOperacionID->GetValue());
    originalDescripcion = txtDescripcion->GetValue();

    EnableInputs(true);
    lblSimilitud->SetLabel("");
    UpdateButtonStates(false, true);
    btnIzquierda->Enable(false);
    btnDerecha->Enable(false);
    isEditing = true;
}

void CreacionProductoFrame::OnBuscar(wxCommandEvent& event) {
    lblSimilitud->SetLabel("");

    BuscarFrameProducto* buscarFrame = new BuscarFrameProducto(this, "Buscar Productos", wxDefaultPosition, wxSize(750, 500));
    buscarFrame->CentreOnParent();
    buscarFrame->Show(true);
    Disable();
}

void CreacionProductoFrame::OnCancelar(wxCommandEvent& event) {
    // Unbind temporalmente el evento de cierre
    this->Unbind(wxEVT_CLOSE_WINDOW, &CreacionProductoFrame::OnClose, this);

    lblSimilitud->SetLabel("");

    if (isNewOperation) {
        int operacionID = wxAtoi(txtOperacionID->GetValue());
        if (operacionID == 0 || !HayRegistrosEnTabla()) {
            ClearInputs();
            EnableInputs(false);
            UpdateButtonStates(false, false);
            btnEditar->Enable(false);
            btnIzquierda->Enable(false);
            btnDerecha->Enable(false);
            btnBuscar->Enable(false);
            btnEliminar->Enable(false);

            // Restablecer el estado
            isNewOperation = false;
            isEditing = false;

            // Rebind el evento de cierre
            this->Bind(wxEVT_CLOSE_WINDOW, &CreacionProductoFrame::OnClose, this);
            return;
        }
        else {
            CargarUltimoRegistro();
            isNewOperation = false;
        }
    }
    else if (isEditing) {
        // Restaurar los valores originales si está en edición
        txtOperacionID->SetValue(wxString::Format("%d", originalOperacionID));
        txtDescripcion->SetValue(originalDescripcion);
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

        // Rebind el evento de cierre
        this->Bind(wxEVT_CLOSE_WINDOW, &CreacionProductoFrame::OnClose, this);
        return;
    }

    // Ajustar botones después de cancelar
    int currentID = wxAtoi(txtOperacionID->GetValue()) - 100;
    int firstID = ObtenerPrimerOperacion();
    int lastID = ObtenerUltimaOperacion();

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

    // Desactivar edición y restablecer botones
    EnableInputs(false);
    UpdateButtonStates(false, false);

    // Asegurar que se restablezcan las banderas de estado
    isNewOperation = false;
    isEditing = false;

    // Rebind el evento de cierre
    this->Bind(wxEVT_CLOSE_WINDOW, &CreacionProductoFrame::OnClose, this);
}

void CreacionProductoFrame::BuscarOperacionProducto(const wxString& operacionID, const wxString& descripcion) {

    if (!conexionAbierta) {
        wxMessageBox("Error: No hay conexión con la base de datos.", "Error", wxOK | wxICON_ERROR);
        return;
    }

    // Query para buscar detalles del producto utilizando OperacionID
    wxString query = wxString::Format(L"SELECT Operacion, OperacionID, Descripcion FROM Producto WHERE OperacionID = '%s';", operacionID);
    SQLHSTMT hStmt = NULL;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
    SQLRETURN ret = SQLExecDirect(hStmt, (SQLWCHAR*)query.wc_str(), SQL_NTS);

    wxString fetchedOperacionID, fetchedDescripcion;
    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
        if (SQLFetch(hStmt) == SQL_SUCCESS) {
            int fetchedOperacion;
            SQLWCHAR descripcionData[255];
            int operacionIDValue;

            SQLGetData(hStmt, 1, SQL_C_LONG, &fetchedOperacion, 0, NULL);
            SQLGetData(hStmt, 2, SQL_C_LONG, &operacionIDValue, 0, NULL);
            SQLGetData(hStmt, 3, SQL_C_WCHAR, descripcionData, sizeof(descripcionData), NULL);

            fetchedOperacionID = wxString::Format("%d", operacionIDValue);
            fetchedDescripcion = wxString(descripcionData);

            // Establecer los valores en los campos de la interfaz de usuario
            txtOperacionID->SetValue(fetchedOperacionID);  // Mostrar el OperacionID recuperado
            txtDescripcion->SetValue(fetchedDescripcion);  // Mostrar la descripción recuperada
        }
    }
    else {
        wxMessageBox("Error al buscar en la base de datos.", "Error", wxOK | wxICON_ERROR);
    }

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
}

int CreacionProductoFrame::ObtenerSiguienteOperacion() {
    SQLHSTMT hStmt = NULL;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
    wxString query = L"SELECT ISNULL(MAX(Operacion), 0) + 1 FROM Producto;";
    SQLRETURN ret = SQLExecDirect(hStmt, (SQLWCHAR*)query.wc_str(), SQL_NTS);

    int siguienteOperacion = 1;

    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
        SQLFetch(hStmt);
        SQLGetData(hStmt, 1, SQL_C_LONG, &siguienteOperacion, 0, NULL);
    }

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    return siguienteOperacion;
}

void CreacionProductoFrame::CargarUltimoRegistro() {
    int ultimoID = ObtenerUltimaOperacion();
    if (ultimoID != -1) {
        CargarDatosOperacion(ultimoID);

        if (ObtenerPrimerOperacion() == ultimoID && ObtenerUltimaOperacion() == ultimoID) {
            btnIzquierda->Enable(false);
            btnDerecha->Enable(false);
        }
        else {
            btnDerecha->Enable(false);
            btnIzquierda->Enable(true);
        }
    }
}

int CreacionProductoFrame::ObtenerPrimerOperacion() {
    SQLHSTMT hStmt = NULL;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
    wxString query = L"SELECT MIN(Operacion) FROM Producto;";
    SQLRETURN ret = SQLExecDirect(hStmt, (SQLWCHAR*)query.wc_str(), SQL_NTS);

    int firstID = -1;

    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
        SQLFetch(hStmt);
        SQLGetData(hStmt, 1, SQL_C_LONG, &firstID, 0, NULL);
    }

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);

    return firstID;
}

int CreacionProductoFrame::ObtenerUltimaOperacion() {
    SQLHSTMT hStmt = NULL;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
    wxString query = L"SELECT MAX(Operacion) FROM Producto;";
    SQLRETURN ret = SQLExecDirect(hStmt, (SQLWCHAR*)query.wc_str(), SQL_NTS);

    int lastID = -1;

    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
        SQLFetch(hStmt);
        SQLGetData(hStmt, 1, SQL_C_LONG, &lastID, 0, NULL);
    }

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);

    return lastID;
}

void CreacionProductoFrame::OnIzquierda(wxCommandEvent& event) {
    int currentID = wxAtoi(txtOperacionID->GetValue()) - 100;
    int previousID = ObtenerOperacionAnterior(currentID);

    if (previousID != -1) {
        CargarDatosOperacion(previousID);
    }
    else {
        btnIzquierda->Enable(false);
    }
}

void CreacionProductoFrame::OnDerecha(wxCommandEvent& event) {
    int currentID = wxAtoi(txtOperacionID->GetValue()) - 100;
    int nextID = ObtenerOperacionSiguiente(currentID);

    if (nextID != -1) {
        CargarDatosOperacion(nextID);
    }
    else {
        btnDerecha->Enable(false);
    }
}

int CreacionProductoFrame::ObtenerOperacionAnterior(int currentID) {
    SQLHSTMT hStmt = NULL;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
    wxString query = wxString::Format(L"SELECT MAX(Operacion) FROM Producto WHERE Operacion < %d;", currentID);
    SQLRETURN ret = SQLExecDirect(hStmt, (SQLWCHAR*)query.wc_str(), SQL_NTS);

    int previousID = -1;

    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
        SQLFetch(hStmt);
        SQLGetData(hStmt, 1, SQL_C_LONG, &previousID, 0, NULL);
    }

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    return previousID;
}

int CreacionProductoFrame::ObtenerOperacionSiguiente(int currentID) {
    SQLHSTMT hStmt = NULL;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
    wxString query = wxString::Format(L"SELECT MIN(Operacion) FROM Producto WHERE Operacion > %d;", currentID);
    SQLRETURN ret = SQLExecDirect(hStmt, (SQLWCHAR*)query.wc_str(), SQL_NTS);

    int nextID = -1;

    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
        SQLFetch(hStmt);
        SQLGetData(hStmt, 1, SQL_C_LONG, &nextID, 0, NULL);
    }

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    return nextID;
}

void CreacionProductoFrame::OnEliminar(wxCommandEvent& event) {

    lblSimilitud->SetLabel("");

    if (!conexionAbierta && !ConectarBaseDeDatos()) {
        wxMessageBox("Error al conectar a la base de datos.", "Error", wxOK | wxICON_ERROR);
        return;
    }

    int operacionID = wxAtoi(txtOperacionID->GetValue());
    if (operacionID == 0) {
        wxMessageBox("No hay una Operacion ID válida para eliminar.", "Error", wxOK | wxICON_ERROR);
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
    bool error = false; // Variable para controlar si se produjo un error

    try {
        SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
        wxString query = L"DELETE FROM Producto WHERE OperacionID = ?;";
        ret = SQLPrepare(hStmt, (SQLWCHAR*)query.wc_str(), SQL_NTS);

        if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
            wxMessageBox("Error al preparar la consulta para eliminar.", "Error", wxOK | wxICON_ERROR);
            error = true;
            throw std::runtime_error("Error preparing SQL statement for deletion.");
        }

        SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &operacionID, 0, NULL);
        ret = SQLExecute(hStmt);

        if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
            wxMessageBox("Error al eliminar el registro.", "Error", wxOK | wxICON_ERROR);
            error = true;
            throw std::runtime_error("Error executing SQL statement for deletion.");
        }

        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);

        // Verificar si la tabla ha quedado vacía tras la eliminación
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

        // Si no está vacía, cargar el siguiente registro o el anterior
        int nextID = ObtenerOperacionSiguiente(operacionID - 100);
        if (nextID != -1) {
            CargarDatosOperacion(nextID);
        }
        else {
            int previousID = ObtenerOperacionAnterior(operacionID - 100);
            if (previousID != -1) {
                CargarDatosOperacion(previousID);
            }
            else {
                ClearInputs();
            }
        }

    }
    catch (const std::exception& ex) {
        // Manejo del error: liberar recursos y mostrar un mensaje
        if (hStmt != NULL) {
            SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        }
        wxMessageBox(wxString::Format("Se produjo un error durante la eliminación: %s", ex.what()), "Error", wxOK | wxICON_ERROR);
        error = true;
    }

    // Rehabilitar la ventana independientemente de si hubo un error o no
    this->Enable();
}

void CreacionProductoFrame::OnClose(wxCloseEvent& event) {
    // Si está en modo edición o nuevo, preguntar si hay cambios sin guardar
    if (isEditing || isNewOperation) {
        int respuesta = wxMessageBox("Hay cambios no guardados. ¿Desea cerrar?", "Confirmar cierre", wxYES_NO | wxICON_QUESTION);
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
        // Si no hay cambios sin guardar, cerrar la ventana directamente
        if (mainFrameParent != nullptr) {
            mainFrameParent->Enable();
        }
        event.Skip();  // Continuar con el cierre
    }
}

void CreacionProductoFrame::OnKeyDown(wxKeyEvent& event) {
    if (event.GetKeyCode() == WXK_ESCAPE) {
        // Si está en modo nuevo o edición
        if (isNewOperation || isEditing) {
            int respuesta = wxMessageBox("Hay cambios no guardados. ¿Desea cancelar los cambios?", "Confirmar cancelación", wxYES_NO | wxICON_QUESTION);
            if (respuesta == wxYES) {
                // Simular comportamiento de "Cancelar"
                wxCommandEvent evt;  // Crear un evento wxCommandEvent vacío
                OnCancelar(evt);     // Pasar el evento vacío
            }
        }
        else {
            // Si no está en modo nuevo o edición, cerrar la ventana directamente
            if (mainFrameParent != nullptr) {
                mainFrameParent->Enable();
            }
            Destroy();  // Cerrar la ventana
        }
    }
    else {
        event.Skip();  // Continuar con otros eventos si no es "Esc"
    }
}

void CreacionProductoFrame::EnableInputs(bool enable) {
    txtOperacionID->Enable(false);
    txtDescripcion->Enable(enable);

    if (!enable) {
        lblSimilitud->SetLabel("");
    }
}

void CreacionProductoFrame::ClearInputs() {
    txtOperacionID->Clear();
    txtDescripcion->Clear();
}

void CreacionProductoFrame::UpdateButtonStates(bool nuevoSelected, bool editarSelected) {
    btnNuevo->Enable(!nuevoSelected && !editarSelected);
    btnBuscar->Enable(!nuevoSelected && !editarSelected);
    btnEliminar->Enable(!nuevoSelected && !editarSelected);
    btnGuardar->Enable(nuevoSelected || editarSelected);
    btnCancelar->Enable(nuevoSelected || editarSelected);
    btnEditar->Enable(!nuevoSelected && !editarSelected);
    btnExportar->Enable(!nuevoSelected && !editarSelected);
}

bool CreacionProductoFrame::ConectarBaseDeDatos() {
    SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
    SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);

    SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);
    wxString connectionString = L"Driver={SQL Server};Server=IdeapadGaming;Database=RSCSystem;Trusted_Connection=yes;";
    SQLRETURN retcode = SQLDriverConnect(hDbc, NULL, (SQLWCHAR*)connectionString.wc_str(), SQL_NTS, NULL, 0, NULL, SQL_DRIVER_COMPLETE);

    conexionAbierta = (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO);
    return conexionAbierta;
}

void CreacionProductoFrame::CerrarConexion() {
    if (conexionAbierta) {
        SQLDisconnect(hDbc);
        SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
        SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
        conexionAbierta = false;
    }
}

void CreacionProductoFrame::CargarDatosOperacion(int operacion) {
    SQLHSTMT hStmt = NULL;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
    wxString query = wxString::Format(L"SELECT Operacion, OperacionID, Descripcion, UsuarioID, Ordenador FROM Producto WHERE Operacion = %d;", operacion);
    SQLRETURN ret = SQLExecDirect(hStmt, (SQLWCHAR*)query.wc_str(), SQL_NTS);

    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
        if (SQLFetch(hStmt) == SQL_SUCCESS) {
            int fetchedOperacion;
            SQLWCHAR descripcion[255], usuarioID[50], ordenador[250];
            int operacionID;

            SQLGetData(hStmt, 1, SQL_C_LONG, &fetchedOperacion, 0, NULL);
            SQLGetData(hStmt, 2, SQL_C_LONG, &operacionID, 0, NULL);
            SQLGetData(hStmt, 3, SQL_C_WCHAR, descripcion, sizeof(descripcion), NULL);
            SQLGetData(hStmt, 4, SQL_C_WCHAR, usuarioID, sizeof(usuarioID), NULL);
            SQLGetData(hStmt, 5, SQL_C_WCHAR, ordenador, sizeof(ordenador), NULL);

            txtOperacionID->SetValue(wxString::Format("%d", operacionID));
            txtDescripcion->SetValue(wxString(descripcion));

            int firstID = ObtenerPrimerOperacion();
            int lastID = ObtenerUltimaOperacion();

            if (fetchedOperacion == firstID) {
                btnIzquierda->Enable(false);
            }
            else {
                btnIzquierda->Enable(true);
            }

            if (fetchedOperacion == lastID) {
                btnDerecha->Enable(false);
            }
            else {
                btnDerecha->Enable(true);
            }
        }
    }
    else {
        wxMessageBox("Error al cargar los datos de la Operacion.", "Error", wxOK | wxICON_ERROR);
    }

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
}

bool CreacionProductoFrame::ValidateInputs() {
    return !txtDescripcion->GetValue().IsEmpty();
}
